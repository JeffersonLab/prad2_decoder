#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <exception>
#include "TTree.h"
#include "TFile.h"
#include "EvChannel.h"
#include "ConfigArgs.h"
#include "Fadc250Decoder.h"
#include "WfAnalyzer.h"
#include "read_modules.h"

#define PROGRESS_COUNT 1000


void write_raw_data(const std::string &dpath, const std::string &opath, const std::string &mpath, int nev,
                    int res = 3, double thres = 20, int npeds = 5, double flat = 1.0);

// event types
enum EvType {
    CODA_PRST = 0xffd1,
    CODA_GO = 0xffd2,
    CODA_END = 0xffd4,
    CODA_PHY1 = 0xff50,
    CODA_PHY2 = 0xff70,
};

int main(int argc, char*argv[])
{
    ConfigArgs arg_parser;
    arg_parser.AddHelps({"-h", "--help"});
    arg_parser.AddPositional("raw_data", "raw data in evio format");
    arg_parser.AddPositional("root_file", "output data file in root format");
    arg_parser.AddArg<int>("-n", "nev", "number of events to process (< 0 means all)", -1);
    arg_parser.AddArgs<std::string>({"-m", "--module"}, "module", "json file for module configuration",
                                    "database/modules/mapmt_module.json");
    arg_parser.AddArg<int>("-r", "res", "resolution for waveform analysis", 3);
    arg_parser.AddArg<double>("-t", "thres", "peak threshold for waveform analysis", 20.0);
    arg_parser.AddArg<int>("-p", "npeds", "sample window width for pedestal searching", 8);
    arg_parser.AddArg<double>("-f", "flat", "flatness requirement for pedestal searching", 1.0);

    auto args = arg_parser.ParseArgs(argc, argv);

    // show arguments
    for(auto &it : args) {
        std::cout << it.first << ": " << it.second.String() << std::endl;
    }

    write_raw_data(args["raw_data"].String(),
                   args["root_file"].String(),
                   args["module"].String(),
                   args["nev"].Int(),
                   args["res"].Int(),
                   args["thres"].Double(),
                   args["npeds"].Int(),
                   args["flat"].Double());
    return 0;
}

// create an event tree according to modules
TTree *create_tree(std::vector<Module> &modules, const std::string tname = "EvTree",
                   const std::string &ttitle = "Cherenkov Test Events")
{
    auto tree = new TTree(tname.c_str(), ttitle.c_str());
    for (auto &m : modules) {
        switch (m.type) {
        case kFADC250:
            {
                // 16 channels
                auto event = new fdec::Fadc250Event(0, 16);
                m.event = static_cast<void*>(event);
                for (auto &ch : m.channels) {
                    auto n = ch.name;
                    tree->Branch(n.c_str(), &event->channels[ch.id], 32000, 0);
                }
            }
            break;
        default:
            std::cout << "Unsupported module type " << m.type << std::endl;
            break;
        }
    }
    return tree;
}

// read raw data in evio format, and extract information
void write_raw_data(const std::string &dpath, const std::string &opath, const std::string &mpath, int nev,
                    int res, double thres, int npeds, double flat)
{
    // read modules
    auto modules = read_modules(mpath);

    if (modules.empty()) {
        std::cout << "Cannot find any modules in configuration file \"" << mpath << "\"" << std::endl;
        return;
    }

    // raw data
    evc::EvChannel evchan;
    if (evchan.Open(dpath) != evc::status::success) {
        std::cout << "Cannot open evchannel at " << dpath << std::endl;
        return;
    }

    // output
    auto *hfile = new TFile(opath.c_str(), "RECREATE", "MAPMT test results");
    auto tree = create_tree(modules);

    fdec::Fadc250Decoder fdecoder;
    // waveform analyzer
    fdec::Analyzer analyzer(res, thres, npeds, flat);
    int count = 0;
    auto &ref = modules.front();
    std::vector<uint64_t> times;
    while ((evchan.Read() == evc::status::success) && (nev-- != 0)) {
        if((count % PROGRESS_COUNT) == 0) {
            std::cout << "Processed events - " << count << "\r" << std::flush;
        }

        switch(evchan.GetEvHeader().tag) {
        // only want physics events
        case CODA_PHY1:
        case CODA_PHY2:
            break;
        case CODA_PRST:
        case CODA_GO:
        case CODA_END:
        default:
            continue;
        }

        evchan.Scan();
        // get block level
        int blvl = evchan.GetEvBuffer(ref.crate, ref.bank, ref.slot).size();

        const uint32_t *dbuf;
        size_t buflen;
        for (int ii = 0; ii < blvl; ++ii) {
            // parse module data
            for (auto &mod : modules) {
                // get data buffer
                try {
                    dbuf = evchan.GetEvBuffer(mod.crate, mod.bank, mod.slot, ii, buflen);
                } catch (std::exception &e) {
                    std::cout << "warning: " << e.what() << "\n";
                    continue;
                }
                // decode by module type
                switch (mod.type) {
                case kFADC250:
                    {
                        auto event = static_cast<fdec::Fadc250Event*>(mod.event);
                        fdecoder.DecodeEvent(*event, dbuf, buflen);
                        for (auto &ch : event->channels) {
                            analyzer.Analyze(ch);
                        }
                        times.push_back(event->time);
                    }
                    break;
                default:
                    std::cout << "Unsupported module type " << mod.type << std::endl;
                    break;
                }
            }
            tree->Fill();
            count ++;
        }

    }
    std::cout << "Processed events - " << count << std::endl;
    if (times.size()) {
        std::cout << "Time difference: " << (times.back() - times.front())*4*1e-9 << "s" << std::endl;
    }

    evchan.Close();
    hfile->Write();
    hfile->Close();
}


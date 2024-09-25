#pragma once

#include <vector>
#include <string>
#include <iostream>
#include "ConfigObject.h"
#include "Fadc250Decoder.h"
#include "SSPDecoder.h"
#include "nlohmann/json.hpp"


enum ModuleType
{
    kFADC250 = 0,
    kSSP = 1,
    kMaxModuleType,
};
ENUM_MAP(ModuleType, 0, "FADC250|SSP");


enum ChannelType
{
    kMaPMT = 0,
    kLAPPD,
    kScint,
    kCalo,
    kMaxChannelType,
};
ENUM_MAP(ChannelType, 0, "MaPMT|LAPPD|Scint|Calo");


struct Channel
{
    int id;
    std::string name;
    ChannelType type;
};


struct Module
{
    int crate, slot, bank;
    ModuleType type;
    std::vector<Channel> channels;
    // save for adding data
    void *event = nullptr;
};


std::vector<Module> read_modules(const std::string &path, bool verbose = false)
{
    // read json
    std::ifstream ifs(path);
    nlohmann::json jdata;
    ifs >> jdata;

    std::vector<Module> res;
    // loop over modules
    for (auto &mdata : jdata["modules"]) {
        Module m;
        m.crate = mdata["crate"].get<int>();
        m.slot = mdata["slot"].get<int>();
        m.bank = mdata["bank"].get<int>();
        m.type = str2ModuleType(mdata["type"].get<std::string>().c_str());
        if (mdata.find("channels") != mdata.end()) {
            for (auto &ch : mdata["channels"]) {
                Channel chan{ch["id"].get<int>(),
                             ch["name"].get<std::string>(),
                             str2ChannelType(ch["type"].get<std::string>().c_str())};
                m.channels.emplace_back(chan);
            }
        }
        res.emplace_back(m);
    }

    if (verbose) {
        // print out all channels
        std::cout << "Read-in " << res.size() << " modules from \"" << path << "\"." << std::endl;
        for (auto &mod : res) {
            std::cout << "Module: crate = " << mod.crate
                      << ", slot = " << mod.slot
                      << ", bank = " << mod.bank
                      << ", type = " << ModuleType2str(mod.type)
                      << std::endl;
            for (auto &ch : mod.channels) {
                std::cout << "\t Channel " << ch.id
                          << ": name =  " << ch.name
                          << ", type = " << ChannelType2str(ch.type)
                          << std::endl;
            }
        }
    }

    return res;
}


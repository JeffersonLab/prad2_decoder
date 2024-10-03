#include "EvChannel.h"
#include "evio.h"
#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <exception>
#include <algorithm>

using namespace evc;


// convert evio status to the enum
static inline status evio_status (unsigned int code)
{
    switch (code) {
    case S_SUCCESS:
        return status::success;
    case EOF:
    case S_EVFILE_UNXPTDEOF:
        return status::eof;
    case S_EVFILE_TRUNC:
        return status::incomplete;
    default:
        return status::failure;
    }
}

EvChannel::EvChannel(size_t buflen)
: fHandle(-1)
{
    buffer.resize(buflen);
}


status EvChannel::Open(const std::string &path)
{
    if (fHandle > 0) {
        Close();
    }
    char *cpath = strdup(path.c_str()), *copt = strdup("r");
    int status = evOpen(cpath, copt, &fHandle);
    free(cpath); free(copt);
    return evio_status(status);
}

void EvChannel::Close()
{
    evClose(fHandle);
    fHandle = -1;
}

status EvChannel::Read()
{
    return evio_status(evRead(fHandle, &buffer[0], buffer.size()));
}

std::string EvChannel::RawBufferAsString(bool annotate_header)
{
    std::stringstream ss;
    auto evh = BankHeader(&buffer[0]);
    uint32_t ibuf = 0;
    uint32_t event_length = evh.length + 1;
    ss << std::hex;
    for (size_t i = 0; i < event_length; ++i) {
        ss << "0x" << std::setw(8) << std::setfill('0') << buffer[i];
        if (annotate_header && (i == ibuf)) {
            evh = BankHeader(&buffer[i]);
            ss << "\t <- header word - "
               << "length: " << std::dec << evh.length
               << ", tag: " << std::dec << evh.tag << " (" << std::hex << "0x" << evh.tag << ")"
               << ", type: " << DataType2str(evh.type) << " (" << std::hex << "0x" << evh.type << ")"
               << ", num: " << std::dec << evh.num << " (" << std::hex << "0x" << evh.num << ")"
               << std::hex;
            ibuf += BankHeader::size();
            switch (evh.type) {
            case DATA_BANK:
            case DATA_ALSOBANK:
                break;
            default:
                ibuf += evh.length - 1;
                break;
            }
        }
        ss << "\n";
    }
    ss << std::dec;
    ss << "== End of This Event ==\n";
    return ss.str();
}

// scan and store all the banks
std::vector<BankHeader> EvChannel::ScanBanks(std::function<bool(const BankHeader&)> filter)
{
    std::vector<BankHeader> res;

    // get the event header and its length
    uint32_t ii = 0;
    auto evh = BankHeader(&buffer[0], ii);

    while (ii < evh.length + 1) {
        BankHeader blk(&buffer[ii], ii);
        // save the bank header
        if ( filter(blk) ) { res.push_back(blk); }

        ii += BankHeader::size();

        switch (blk.type) {
        case DATA_BANK:
        case DATA_ALSOBANK:
            // banks inside the bank (headers just below)
            break;
        default:
            ii += blk.length - 1;
            break;
        }
    }

    return res;
}


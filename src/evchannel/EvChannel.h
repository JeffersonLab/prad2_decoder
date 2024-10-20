//=============================================================================
// Class EvChannel                                                           ||
// Read event from CODA evio file, it can also scan data banks to locate     ||
// event buffers                                                             ||
//                                                                           ||
// Developer:                                                                ||
// Chao Peng                                                                 ||
// 09/07/2020                                                                ||
//=============================================================================
#pragma once

#include "EvStruct.h"
#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <functional>
#include <unordered_map>


namespace evc {

// status enum
enum class status : int
{
    failure = -1,
    success = 1,
    incomplete = 2,
    empty = 3,
    eof = 4,
};

class EvChannel
{
public:
    EvChannel(size_t buflen = 1024*2000);
    virtual ~EvChannel() { Close(); }

    EvChannel(const EvChannel &)  = delete;
    void operator =(const EvChannel &)  = delete;

    virtual status Open(const std::string &path);
    virtual void Close();
    virtual status Read();

    std::vector<BankHeader> ScanBanks(
        std::function<bool(const BankHeader&)> filter =
        [] (const BankHeader &header) {
            return true;
            }
        );

    uint32_t *GetRawBuffer() { return &buffer[0]; }
    const uint32_t *GetRawBuffer() const { return &buffer[0]; }
    std::string RawBufferAsString(bool annotate_header = true);

    std::vector<uint32_t> &GetRawBufferVec() { return buffer; }
    const std::vector<uint32_t> &GetRawBufferVec() const { return buffer; }

    BankHeader GetEvHeader() const { return BankHeader(&buffer[0]); }

protected:
    int fHandle;
    std::vector<uint32_t> buffer;
};

} // namespace evc


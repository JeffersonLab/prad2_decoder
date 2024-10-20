//=============================================================================
// EvStruct                                                                  ||
// Basic information about CODA evio file format                             ||
//                                                                           ||
// Developer:                                                                ||
// Chao Peng                                                                 ||
// 09/07/2020                                                                ||
//=============================================================================
#pragma once

#include <cstdint>
#include <cstddef>
#include <string>


namespace evc
{

// evio bank data type
enum DataType {
    DATA_UNKNOWN32    =  (0x0),
    DATA_UINT32       =  (0x1),
    DATA_FLOAT32      =  (0x2),
    DATA_CHARSTAR8    =  (0x3),
    DATA_SHORT16      =  (0x4),
    DATA_USHORT16     =  (0x5),
    DATA_CHAR8        =  (0x6),
    DATA_UCHAR8       =  (0x7),
    DATA_DOUBLE64     =  (0x8),
    DATA_LONG64       =  (0x9),
    DATA_ULONG64      =  (0xa),
    DATA_INT32        =  (0xb),
    DATA_TAGSEGMENT   =  (0xc),
    DATA_ALSOSEGMENT  =  (0xd),
    DATA_ALSOBANK     =  (0xe),
    DATA_COMPOSITE    =  (0xf),
    DATA_BANK         =  (0x10),
    DATA_SEGMENT      =  (0x20)
};

static std::string DataType2str(int dtype)
{
    switch (DataType(dtype)) {
    default:
    case DATA_UNKNOWN32:
        return "Unknown" + std::to_string(int(dtype));
    case DATA_UINT32:
        return "uint32";
    case DATA_FLOAT32:
        return "float32";
    case DATA_CHARSTAR8:
        return "char*8";
    case DATA_SHORT16:
        return "short16";
    case DATA_USHORT16:
        return "ushort16";
    case DATA_CHAR8:
        return "char8";
    case DATA_UCHAR8:
        return "uchar8";
    case DATA_DOUBLE64:
        return "double64";
    case DATA_LONG64:
        return "long64";
    case DATA_ULONG64:
        return "ulong64";
    case DATA_INT32:
        return "int32";
    case DATA_TAGSEGMENT:
        return "tagsegment";
    case DATA_ALSOSEGMENT:
        return "alsosegment";
    case DATA_ALSOBANK:
        return "alsobank";
    case DATA_COMPOSITE:
        return "composite";
    case DATA_BANK:
        return "bank";
    case DATA_SEGMENT:
        return "segment";
    }
}

/* 32 bit bank header structure
 * -------------------------------------
 * |          length:32                |
 * -------------------------------------
 * |   tag:16   |pad:2| type:6 | num:8 |
 * -------------------------------------
 */
struct BankHeader
{
    uint32_t buf_loc;
    uint32_t length, num, type, tag, padding;

    BankHeader() : buf_loc(0), length(0) {}
    BankHeader(const uint32_t *buf, uint32_t loc = 0)
    {
        buf_loc = loc;
        length = buf[0];
        uint32_t word = buf[1];
        tag = (word >> 16) & 0xFFFF;
        padding = (word >> 14) & 0x3;
        type = (word >> 8) & 0x3F;
        num = (word & 0xFF);
    }

    static size_t size() { return 2; }
};

struct SegmentHeader
{
    uint32_t buf_loc;
    uint32_t length, type, tag, padding;

    SegmentHeader() : buf_loc(0) {}
    SegmentHeader(const uint32_t *buf, uint32_t loc = 0)
    {
        buf_loc = loc;
        uint32_t word = buf[0];
        tag = (word >> 24) & 0xFF;
        type = (word >> 16) & 0x3F;
        padding = (word >> 22) & 0x3;
        length = (word & 0xFFFF);
    }

    static size_t size() { return 1; }
};

struct TagSegmentHeader
{
    uint32_t buf_loc;
    uint32_t length, type, tag;

    TagSegmentHeader() : buf_loc(0) {}
    TagSegmentHeader(const uint32_t *buf, uint32_t loc = 0)
    {
        buf_loc = loc;
        uint32_t word = buf[0];
        tag = (word >> 20) & 0xFFF;
        type = (word >> 16) & 0xF;
        length = (word & 0xFFFF);
    }

    static size_t size() { return 1; }
};

// data word definitions
enum WordDefinition {
    BLOCK_HEADER = 0,
    BLOCK_TRAILER = 1,
    EVENT_HEADER = 2,
};

struct BlockHeader
{
    bool valid;
    uint32_t nevents, number, mod, slot;

    BlockHeader() : valid(false) {}
    BlockHeader(const uint32_t *buf)
    {
        uint32_t word = buf[0];
        valid = (word & 0x80000000) && (((word >> 27) & 0xF) == BLOCK_HEADER);
        slot = (word >> 22) & 0x1F;
        mod = (word >> 18) & 0xF;
        number = (word >> 8) & 0x3FF;
        nevents = (word & 0xFF);
    }

    static size_t size() { return 1; }
};

struct BlockTrailer
{
    bool valid;
    uint32_t nwords, slot;

    BlockTrailer() : valid(false) {}
    BlockTrailer(const uint32_t *buf)
    {
        uint32_t word = buf[0];
        valid = (word & 0x80000000) && (((word >> 27) & 0xF) == BLOCK_TRAILER);
        slot = (word >> 22) & 0x1F;
        nwords = (word & 0x3FFFFF);
    }

    static size_t size() { return 1; }
};

struct EventHeader
{
    bool valid;
    uint32_t number, slot;

    EventHeader() : valid(false) {}
    EventHeader(const uint32_t *buf)
    {
        uint32_t word = buf[0];
        valid = (word & 0x80000000) && (((word >> 27) & 0xF) == EVENT_HEADER);
        slot = (word >> 22) & 0x1F;
        number = (word & 0x3FFFFF);
    }

    static size_t size() { return 1; }
};

} // namespace evio


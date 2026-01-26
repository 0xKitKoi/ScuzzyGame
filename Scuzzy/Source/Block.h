#pragma once
#include <vector>
#include <cstdint>

class Block {
public:
    explicit Block(size_t location);

    // decompression helper
    std::vector<uint8_t> decompress() const;

    uint8_t readInt16(); // returns one byte (mirrors JS Block.readInt16)
    uint32_t readInt32();
    int16_t readDoubleShort();

    size_t pointer() const { return m_pointer; }

private:
    size_t m_address;
    size_t m_pointer;
};

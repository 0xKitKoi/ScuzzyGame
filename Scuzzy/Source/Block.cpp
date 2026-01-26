#include "Block.h"
#include "Rom.h"
#include "Decompress.h"
#include <stdexcept>

Block::Block(size_t location) : m_address(location), m_pointer(location) {
    if (location >= ROM::size()) throw std::runtime_error("Block location out of ROM bounds");
}

uint8_t Block::readInt16() {
    if (m_pointer >= ROM::size()) throw std::runtime_error("Read past ROM end");
    return ROM::data()[m_pointer++];
}

uint32_t Block::readInt32() {
    uint32_t b0 = readInt16();
    uint32_t b1 = readInt16();
    uint32_t b2 = readInt16();
    uint32_t b3 = readInt16();
    return b0 + (b1 << 8) + (b2 << 16) + (b3 << 24);
}

int16_t Block::readDoubleShort() {
    // read two bytes little-endian into signed 16
    uint8_t lo = readInt16();
    uint8_t hi = readInt16();
    int16_t val = (int16_t)((hi << 8) | lo);
    return val;
}

std::vector<uint8_t> Block::decompress() const {
    size_t start = m_pointer;
    int size = getCompressedSize((int)start, ROM::data());
    if (size < 1) throw std::runtime_error("Invalid compressed data: getCompressedSize failed");
    std::vector<uint8_t> out((size_t)size);
    auto res = ::decompress((int)start, ROM::data(), out, 0);
    if (res.empty()) throw std::runtime_error("Decompression failed or returned empty");
    return res;
}

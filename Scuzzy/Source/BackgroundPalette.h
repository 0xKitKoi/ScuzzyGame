#pragma once
#include <vector>
#include <cstdint>

class Block;

class BackgroundPalette {
public:
    BackgroundPalette(int index, int bitsPerPixel);
    void read(int index);
    const std::vector<std::vector<uint32_t>>& getColorMatrix() const { return colors; }
    const std::vector<uint32_t>& getColors(size_t paletteIndex) const { return colors.at(paletteIndex); }
private:
    std::vector<std::vector<uint32_t>> colors;
    int bitsPerPixel;
    size_t address;
    void readPalette(Block& block, int bitsPerPixel, int count);
};

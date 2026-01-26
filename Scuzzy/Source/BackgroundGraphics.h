#pragma once
#include <vector>
#include <cstdint>
#include "ROMGraphics.h"

class BackgroundGraphics {
public:
    BackgroundGraphics(int index, int bitsPerPixel);
    void read(int index);
    void draw(std::vector<uint8_t>& bitmap, const std::vector<std::vector<uint32_t>>& palette);
private:
    std::vector<uint8_t> arrayROMGraphics;
    ROMGraphics romGraphics;
};

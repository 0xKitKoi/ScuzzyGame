#include "BackgroundPalette.h"
#include "Rom.h"
#include "Block.h"
#include <stdexcept>
#include <iostream>

BackgroundPalette::BackgroundPalette(int index, int bitsPerPixel): bitsPerPixel(bitsPerPixel) {
}

void BackgroundPalette::read(int index) {
    // pointer = readBlock(0xDAD9 + index * 4)
    size_t pointerLoc = 0xDAD9 + index * 4;
    Block pointerBlock = readBlock(pointerLoc);
    uint32_t address = snesToHex(pointerBlock.readInt32());
    Block dataBlock(address);
    
    // For 4bpp, we need 4 sub-palettes (16 colors each)
    // For 2bpp, we need 16 sub-palettes (4 colors each)
    // For 8bpp, we have 1 sub-palette (256 colors)
    int subPaletteCount = 1;
    if (this->bitsPerPixel == 4) {
        subPaletteCount = 4;
    } else if (this->bitsPerPixel == 2) {
        subPaletteCount = 16;
    }
    
    readPalette(dataBlock, this->bitsPerPixel, subPaletteCount);
}

void BackgroundPalette::readPalette(Block& block, int bitsPerPixel, int count) {
    if (bitsPerPixel != 2 && bitsPerPixel != 4 && bitsPerPixel != 8) {
        throw std::runtime_error("Palette error: Incorrect color depth specified.");
    }
    if (count < 1) throw std::runtime_error("Palette error: Must specify positive number of subpalettes.");
    colors.clear();
    int power = 1 << bitsPerPixel;
    for (int palette = 0; palette < count; ++palette) {
        colors.emplace_back();
        colors.back().resize(power);
        for (int i = 0; i < power; ++i) {
            int16_t clr16 = block.readDoubleShort();
            // SNES color format is BGR555: bits [14:10] = B, [9:5] = G, [4:0] = R
            uint32_t b = ((clr16 >> 10) & 31) * 8;
            uint32_t g = ((clr16 >> 5) & 31) * 8;
            uint32_t r = (clr16 & 31) * 8;
            // Store as RGB format: 0xFF RRRR GGGG BBBB
            colors.back()[i] = (0xFFu << 24) | (r << 16) | (g << 8) | b;
        }
    }
}

#include "ROMGraphics.h"
#include <stdexcept>
#include <cmath>

ROMGraphics::ROMGraphics(int bitsPerPixel): bitsPerPixel(bitsPerPixel) {}

void ROMGraphics::loadGraphics(const std::vector<uint8_t>& blockData) {
    gfxROMGraphics = blockData;
    buildTiles();
}

void ROMGraphics::buildTiles() {
    if (gfxROMGraphics.empty()) return;
    const int n = (int)gfxROMGraphics.size() / (8 * bitsPerPixel);
    tiles.clear();
    tiles.resize(n);
    for (int i = 0; i < n; ++i) {
        const int o = i * 8 * bitsPerPixel;
        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                int c = 0;
                for (int bp = 0; bp < bitsPerPixel; ++bp) {
                    int halfBp = bp / 2; // floor
                    int index = o + y * 2 + (halfBp * 16 + (bp & 1));
                    if (index < 0 || index >= (int)gfxROMGraphics.size()) throw std::runtime_error("gfx index OOB");
                    uint8_t gfx = gfxROMGraphics[index];
                    int bit = 1 << (7 - x);
                    c += ((gfx & bit) ? 1 : 0) << bp;
                }
                tiles[i][x][y] = (uint8_t)c;
            }
        }
    }
}

void ROMGraphics::draw(std::vector<uint8_t>& bmp, const std::vector<std::vector<uint32_t>>& palette, const std::vector<uint8_t>& arrayROMGraphics) {
    if (bmp.size() < 256 * 256 * 4) bmp.resize(256 * 256 * 4);
    const int stride = 1024;

    // Calculate sub-palette mask based on bits per pixel
    // For 2bpp: 4 colors/palette, so 4 palettes per block, mask = 0x3
    // For 4bpp: 16 colors/palette, so 2 palettes per block, mask = 0x1
    // For 8bpp: 256 colors/palette, so 1 palette per block, mask = 0x0
    int subPaletteMask;
    if (bitsPerPixel == 2) {
        subPaletteMask = 0xF;  // 16 sub-palettes: bits [13:10]
    } else if (bitsPerPixel == 4) {
        subPaletteMask = 0x3;  // 4 sub-palettes: bits [11:10]
    } else {
        subPaletteMask = 0x0;  // 1 sub-palette for 8bpp
    }

    for (int j = 0; j < 32; ++j) {
        for (int i = 0; i < 32; ++i) {
            int n = j * 32 + i;
            if ((size_t)(n * 2 + 1) >= arrayROMGraphics.size()) continue;
            int b1 = arrayROMGraphics[n * 2];
            int b2 = arrayROMGraphics[n * 2 + 1] << 8;
            int block = b1 + b2;
            int tile = block & 0x3FF;
            bool verticalFlip = (block & 0x8000) != 0;
            bool horizontalFlip = (block & 0x4000) != 0;
            int subPalette = (block >> 10) & subPaletteMask;
            
            // Safety check
            if (subPalette >= (int)palette.size()) {
                continue;  // Skip if palette index is out of bounds
            }
            
            const std::vector<uint32_t>& subPaletteArray = palette.at(subPalette);
            drawTile(bmp, stride, i * 8, j * 8, subPaletteArray, tile, verticalFlip, horizontalFlip);
        }
    }
}

void ROMGraphics::drawTile(std::vector<uint8_t>& pixels, int stride, int x, int y, const std::vector<uint32_t>& subPalette, int tileIndex, bool verticalFlip, bool horizontalFlip) {
    if (tileIndex < 0 || tileIndex >= (int)tiles.size()) return;
    for (int i = 0; i < 8; ++i) {
        int px = horizontalFlip ? (x + 7 - i) : (x + i);
        for (int j = 0; j < 8; ++j) {
            int py = verticalFlip ? (y + 7 - j) : (y + j);
            uint8_t colorIndex = tiles[tileIndex][i][j];
            uint32_t rgb = subPalette.at(colorIndex);
            int pos = 4 * px + stride * py;
            if (pos + 2 >= (int)pixels.size()) continue;
            pixels[pos + 0] = (rgb) & 0xFF;        // B
            pixels[pos + 1] = (rgb >> 8) & 0xFF;   // G
            pixels[pos + 2] = (rgb >> 16) & 0xFF;  // R
            pixels[pos + 3] = 255;                 // A
        }
    }
}

#pragma once
#include <vector>
#include <cstdint>
#include <array>

class ROMGraphics {
public:
    ROMGraphics(int bitsPerPixel);
    void loadGraphics(const std::vector<uint8_t>& blockData);
    // draw into a 256x256 RGBA buffer (4 bytes per pixel), stride is 1024
    void draw(std::vector<uint8_t>& bmp, const std::vector<std::vector<uint32_t>>& palette, const std::vector<uint8_t>& arrayROMGraphics);

private:
    int bitsPerPixel;
    std::vector<uint8_t> gfxROMGraphics; // decompressed graphics data
    // tiles[tileIndex][x][y]
    std::vector<std::array<std::array<uint8_t, 8>, 8>> tiles;
    void buildTiles();
    void drawTile(std::vector<uint8_t>& pixels, int stride, int x, int y, const std::vector<uint32_t>& subPalette, int tileIndex, bool verticalFlip, bool horizontalFlip);
};

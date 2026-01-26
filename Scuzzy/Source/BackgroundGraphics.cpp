#include "BackgroundGraphics.h"
#include "Rom.h"
#include "Block.h"
#include <stdexcept>
#include <iostream>
#include <ios>

BackgroundGraphics::BackgroundGraphics(int index, int bitsPerPixel): romGraphics(bitsPerPixel) {
    read(index);
}

void BackgroundGraphics::read(int index) {
    // Graphics pointer table entry
    Block graphicsPointerBlock = readBlock(0xD7A1 + index * 4);
    uint32_t graphicsPointer = graphicsPointerBlock.readInt32();
    size_t graphicsAddress = snesToHex(graphicsPointer);
    Block gfxBlock(graphicsAddress);
    romGraphics.loadGraphics(gfxBlock.decompress());

    // Arrangement pointer table entry
    Block arrayPointerBlock = readBlock(0xD93D + index * 4);
    uint32_t arrayPointer = arrayPointerBlock.readInt32();
    size_t arrayAddress = snesToHex(arrayPointer);
    Block arrayBlock(arrayAddress);
    arrayROMGraphics = arrayBlock.decompress();
}

void BackgroundGraphics::draw(std::vector<uint8_t>& bitmap, const std::vector<std::vector<uint32_t>>& palette) {
    romGraphics.draw(bitmap, palette, arrayROMGraphics);
}

#include "BattleBackground.h"
#include "Block.h"
#include <string>
#include <stdexcept>
#include <exception>

BattleBackground::BattleBackground(uint16_t index)
    : data(STRUCT_SIZE, 0) {
    read(index);
}

void BattleBackground::read(uint16_t index) {
    try {
        Block block(ROM_ADDRESS + index * STRUCT_SIZE);
        for (size_t i = 0; i < STRUCT_SIZE; ++i) {
            // Read a single byte and store in the int16_t (will be zero-extended)
            data[i] = block.readInt16();
        }
    }
    catch (const std::exception& e) {
        throw std::runtime_error(
            "Failed to read BattleBackground at index " +
            std::to_string(index) + ": " + e.what()
        );
    }
}

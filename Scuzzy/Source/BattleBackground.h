#pragma once
#include <cstdint>
#include <vector>

class BattleBackground {
public:
    explicit BattleBackground(uint16_t index = 0);

    // Getters for all 17 bytes
    uint8_t graphicsIndex() const { return data[0]; }
    uint8_t paletteIndex() const { return data[1]; }
    uint8_t bitsPerPixel() const { return data[2]; }
    uint8_t paletteCycleType() const { return data[3]; }
    uint8_t paletteCycle1Start() const { return data[4]; }
    uint8_t paletteCycle1End() const { return data[5]; }
    uint8_t paletteCycle2Start() const { return data[6]; }
    uint8_t paletteCycle2End() const { return data[7]; }
    uint8_t paletteCycleSpeed() const { return data[8]; }
    uint8_t horizontalMovement() const { return data[9]; }
    uint8_t verticalMovement() const { return data[10]; }
    uint8_t horizontalAcceleration() const { return data[11]; }
    uint8_t verticalAcceleration() const { return data[12]; }
    
    // Animation is bytes 13-16 in big-endian order
    uint32_t animation() const {
        return (static_cast<uint32_t>(data[13]) << 24) |
               (static_cast<uint32_t>(data[14]) << 16) |
               (static_cast<uint32_t>(data[15]) << 8) |
               (static_cast<uint32_t>(data[16]));
    }

private:
    std::vector<uint8_t> data;
    static constexpr size_t STRUCT_SIZE = 17;
    static constexpr size_t ROM_ADDRESS = 0xDCA1;

    void read(uint16_t index);
};

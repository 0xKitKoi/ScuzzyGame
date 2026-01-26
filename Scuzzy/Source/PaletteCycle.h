#pragma once
#include <vector>
#include <cstdint>
#include <memory>

class BackgroundPalette;

class PaletteCycle {
public:
    struct Config {
        uint8_t cycleType;
        uint8_t start1;
        uint8_t end1;
        uint8_t start2;
        uint8_t end2;
        uint8_t speed;
    };

    PaletteCycle(const Config& config, std::shared_ptr<BackgroundPalette> palette);

    // Advance the cycle by one frame, returns true if colors changed
    bool cycle();

    // Get the current colors for a sub-palette
    const std::vector<uint32_t>& getColors(size_t subPaletteIndex) const;

    // Get the full current color matrix
    const std::vector<std::vector<uint32_t>>& getNowColors() const { return nowColors; }

private:
    uint8_t type;
    uint8_t start1;
    uint8_t end1;
    uint8_t start2;
    uint8_t end2;
    uint8_t speed;
    
    int cycleCountdown;
    int cycleCount;
    
    std::shared_ptr<BackgroundPalette> palette;
    std::vector<std::vector<uint32_t>> originalColors;
    std::vector<std::vector<uint32_t>> nowColors;

    void cycleColors();
};

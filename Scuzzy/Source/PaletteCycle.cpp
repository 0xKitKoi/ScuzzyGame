#include "PaletteCycle.h"
#include "BackgroundPalette.h"
#include <algorithm>
#include <iostream>

PaletteCycle::PaletteCycle(const Config& config, std::shared_ptr<BackgroundPalette> pal)
    : type(config.cycleType),
      start1(config.start1),
      end1(config.end1),
      start2(config.start2),
      end2(config.end2),
      speed(config.speed / 2),  // Match JS: divide by 2
      cycleCountdown(static_cast<int>(config.speed / 2)),
      cycleCount(0),
      palette(pal) {
    
    // Copy original colors and set up duplicate range for cycling
    const auto& original = palette->getColorMatrix();
    for (size_t subPal = 0; subPal < original.size(); ++subPal) {
        // Make a copy of this sub-palette's colors
        std::vector<uint32_t> originalRow;
        
        // Copy existing colors
        for (const auto& color : original[subPal]) {
            originalRow.push_back(color);
        }
        
        // Ensure we have at least 32 colors
        if (originalRow.size() < 32) {
            originalRow.resize(32, 0);
        }
        
        // Duplicate indices 0-15 to indices 16-31 (for easier cycle math)
        for (int i = 16; i < 32; ++i) {
            originalRow[i] = originalRow[i - 16];
        }
        
        originalColors.push_back(originalRow);
        nowColors.push_back(originalRow);
    }
}

bool PaletteCycle::cycle() {
    if (speed == 0) {
        return false;
    }
    
    --cycleCountdown;
    if (cycleCountdown <= 0) {
        cycleColors();
        ++cycleCount;
        cycleCountdown = speed;
        return true;
    }
    return false;
}

void PaletteCycle::cycleColors() {
    if (type == 1 || type == 2) {
        if (end1 < start1) {
            return;  // Invalid range
        }
        int cycleLength = end1 - start1 + 1;
        int cycle1Position = cycleCount % cycleLength;
        
        for (size_t subPalette = 0; subPalette < originalColors.size(); ++subPalette) {
            for (int i = start1; i <= end1; ++i) {
                if (i >= static_cast<int>(originalColors[subPalette].size())) {
                    continue;  // Skip out-of-bounds
                }
                int newColor = i - cycle1Position;
                if (newColor < start1) {
                    newColor += cycleLength;
                }
                if (newColor >= 0 && newColor < static_cast<int>(originalColors[subPalette].size())) {
                    nowColors[subPalette][i] = originalColors[subPalette][newColor];
                }
            }
        }
    }
    
    if (type == 2) {
        if (end2 < start2) {
            return;  // Invalid range
        }
        int cycleLength = end2 - start2 + 1;
        int cycle2Position = cycleCount % cycleLength;
        
        for (size_t subPalette = 0; subPalette < originalColors.size(); ++subPalette) {
            for (int i = start2; i <= end2; ++i) {
                if (i >= static_cast<int>(originalColors[subPalette].size())) {
                    continue;  // Skip out-of-bounds
                }
                int newColor = i - cycle2Position;
                if (newColor < start2) {
                    newColor += cycleLength;
                }
                if (newColor >= 0 && newColor < static_cast<int>(originalColors[subPalette].size())) {
                    nowColors[subPalette][i] = originalColors[subPalette][newColor];
                }
            }
        }
    }
    
    if (type == 3) {
        if (end1 < start1) {
            return;  // Invalid range
        }
        int cycleLength = end1 - start1 + 1;
        int cycle1Position = cycleCount % (cycleLength * 2);
        
        for (size_t subPalette = 0; subPalette < originalColors.size(); ++subPalette) {
            for (int i = start1; i <= end1; ++i) {
                if (i >= static_cast<int>(originalColors[subPalette].size())) {
                    continue;  // Skip out-of-bounds
                }
                int newColor = i + cycle1Position;
                int difference = 0;
                
                if (newColor > end1) {
                    difference = newColor - end1 - 1;
                    newColor = end1 - difference;
                    if (newColor < start1) {
                        difference = start1 - newColor - 1;
                        newColor = start1 + difference;
                    }
                }
                if (newColor >= 0 && newColor < static_cast<int>(originalColors[subPalette].size())) {
                    nowColors[subPalette][i] = originalColors[subPalette][newColor];
                }
            }
        }
    }
}

const std::vector<uint32_t>& PaletteCycle::getColors(size_t subPaletteIndex) const {
    if (subPaletteIndex >= nowColors.size()) {
        // Return empty or first palette as fallback
        if (nowColors.empty()) {
            static std::vector<uint32_t> empty;
            return empty;
        }
        return nowColors[0];
    }
    return nowColors[subPaletteIndex];
}

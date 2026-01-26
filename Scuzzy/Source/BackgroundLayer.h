#pragma once
#include <memory>
#include <vector>
#include <cstdint>

class BackgroundGraphics;
class BackgroundPalette;
class PaletteCycle;
class Distorter;
class DistortionEffect;

class BackgroundLayer {
public:
    // Constructor takes a layer entry index and ROM data
    explicit BackgroundLayer(uint16_t entry);
    
    ~BackgroundLayer();

    // Render a frame into the destination bitmap
    void overlayFrame(
        uint8_t* bitmap,
        int width,
        int height,
        int letterbox,
        float ticks,
        float alpha,
        bool erase
    );

    uint16_t entry() const { return m_entry; }
    bool isValid() const { return m_valid; }

private:
    uint16_t m_entry;
    bool m_valid;

    std::shared_ptr<BackgroundGraphics> graphics;
    std::shared_ptr<BackgroundPalette> palette;
    std::shared_ptr<PaletteCycle> paletteCycle;
    std::shared_ptr<Distorter> distorter;
    std::shared_ptr<DistortionEffect> effect;
    std::vector<uint8_t> pixels;

    void loadEntry(uint16_t index);
    void loadGraphics(uint8_t index, uint8_t bitsPerPixel);
    void loadPalette(uint8_t paletteIndex, uint8_t bitsPerPixel);
    void loadEffect(uint8_t index);
};

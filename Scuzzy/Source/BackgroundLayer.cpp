#include "BackgroundLayer.h"
#include "BattleBackground.h"
#include "BackgroundGraphics.h"
#include "BackgroundPalette.h"
#include "PaletteCycle.h"
#include "Distorter.hpp"
#include "DistortionEffect.hpp"
#include <iostream>
#include <stdexcept>

constexpr int SNES_WIDTH = 256;
constexpr int SNES_HEIGHT = 224;

BackgroundLayer::BackgroundLayer(uint16_t entry)
    : m_entry(entry),
      m_valid(false),
      pixels(SNES_WIDTH * SNES_HEIGHT * 4, 0) {
    try {
        loadEntry(entry);
        m_valid = true;
    } catch (const std::exception& e) {
        std::cerr << "BackgroundLayer(" << entry << ") failed: " << e.what() << "\n";
        m_valid = false;
    }
}

BackgroundLayer::~BackgroundLayer() {
    // Shared pointers handle cleanup
}

void BackgroundLayer::loadEntry(uint16_t index) {
    BattleBackground background(index);
    
    // Extract animation effect index (bytes 13-16 as big-endian)
    uint32_t animation = background.animation();
    uint8_t e1 = (animation >> 24) & 0xFF;
    uint8_t e2 = (animation >> 16) & 0xFF;
    uint8_t effectIndex = e2 ? e2 : e1;
    
    // Store bits per pixel for later use
    uint8_t bitsPerPixel = background.bitsPerPixel();
    
    // Load components in order: palette first, then graphics, then effect
    loadPalette(background.paletteIndex(), bitsPerPixel);
    loadGraphics(background.graphicsIndex(), bitsPerPixel);
    loadEffect(effectIndex);
}

void BackgroundLayer::loadGraphics(uint8_t index, uint8_t bitsPerPixel) {
    // Skip rendering graphics for index 0 (empty/background layer)
    if (index == 0) {
        return;
    }
    graphics = std::make_shared<BackgroundGraphics>(index, bitsPerPixel);
    
    // Draw graphics into our pixel buffer using the loaded palette
    if (palette) {
        graphics->draw(pixels, palette->getColorMatrix());
    }
}

void BackgroundLayer::loadPalette(uint8_t paletteIndex, uint8_t bitsPerPixel) {
    palette = std::make_shared<BackgroundPalette>(paletteIndex, bitsPerPixel);
    palette->read(paletteIndex);
    
    // Set up palette cycling based on BattleBackground
    BattleBackground background(m_entry);
    
    PaletteCycle::Config cycleConfig{
        background.paletteCycleType(),
        background.paletteCycle1Start(),
        background.paletteCycle1End(),
        background.paletteCycle2Start(),
        background.paletteCycle2End(),
        background.paletteCycleSpeed()
    };
    
    paletteCycle = std::make_shared<PaletteCycle>(cycleConfig, palette);
}

void BackgroundLayer::loadEffect(uint8_t index) {
    effect = std::make_shared<DistortionEffect>(index);
    distorter = std::make_shared<Distorter>(pixels.data());
}

void BackgroundLayer::overlayFrame(
    uint8_t* bitmap,
    int width,
    int height,
    int letterbox,
    float ticks,
    float alpha,
    bool erase
) {
    if (!m_valid) {
        return;
    }

    // Update palette cycling and redraw only if palette actually changed
    if (paletteCycle && graphics) {
        if (paletteCycle->cycle()) {
            // Palette changed, redraw graphics with cycled palette
            graphics->draw(pixels, paletteCycle->getNowColors());
        }
    }

    // Apply distortion with letterboxing
    if (distorter && effect) {
        distorter->overlayFrame(bitmap, width, height, *effect, letterbox, ticks, alpha, erase);
    }
}

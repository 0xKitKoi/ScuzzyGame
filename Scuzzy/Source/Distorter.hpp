/*
#include <cstdint>
#include "DistortionEffect.hpp"

class Distorter {
public:
    Distorter(const uint8_t* sourceRGBA)
        : source(sourceRGBA) {}

    void apply(
        uint8_t* dest,
        int width,
        int height,
        const DistortionEffect& effect,
        uint32_t tick,
        float alpha,
        bool clear
    );

private:
    const uint8_t* source;
};
*/
#pragma once
#include <cstdint>
#include <cmath>
#include <cstring>
#include "DistortionEffect.hpp"

class Distorter {
    const uint8_t* source;
    
    // Constants from JS implementation
    const float C1 = 1.0f / 512.0f;  // amplitude scaling
    const float C2 = 8.0f * 3.141592653589793f / (1024.0f * 256.0f);  // frequency scaling
    const float C3 = 3.141592653589793f / 60.0f;  // speed scaling

public:
    explicit Distorter(const uint8_t* src) : source(src) {}

    // Compute the distortion offset for a given y-coordinate
    float computeOffset(
        int y,
        float tick,
        const DistortionEffect& e,
        float& amplitude,
        float& frequency,
        float& compression,
        float& speed
    ) const {
        // Constants for computing time-based modulation
        float t2 = tick * 2.0f;
        
        // Update the amplitude, frequency, compression, and speed based on time
        amplitude = C1 * (e.amplitude() + e.amplitudeAcceleration() * t2);
        frequency = C2 * (e.frequency() + e.frequencyAcceleration() * t2);
        compression = 1.0f + (e.compression() + e.compressionAcceleration() * t2) / 256.0f;
        speed = C3 * e.speed() * tick;

        /*
        ```
        Offset (y, t) = A sin ( F*y + S*t )
        ```

        where:

        - _y_ is the vertical coordinate being transformed
        - _t_ is time that's elapsed
        - _A_ is the amplitude
        - _F_ is the frequency
        - _S_ is the speed or frameskip of the transformation
        */

        
        // Calculate S(y) = amplitude * sin(frequency * y + speed)
        float s = amplitude * std::sin(frequency * y + speed);
        return std::round(s);
    }

    void apply(
        uint8_t* dest,
        int w,
        int h,
        const DistortionEffect& e,
        float tick,
        float alpha = 1.0f,
        bool clear = true,
        int letterbox = 0
    ) {
        if (clear) {
            memset(dest, 0, w * h * 4);
        }

        float amplitude, frequency, compression, speed;

        for (int y = 0; y < h; ++y) {
            // Apply letterbox (black bars)
            if (y < letterbox || y >= h - letterbox) {
                // Fill with black
                for (int x = 0; x < w; ++x) {
                    int dstIdx = (y * w + x) * 4;
                    dest[dstIdx + 0] = 0;    // B
                    dest[dstIdx + 1] = 0;    // G
                    dest[dstIdx + 2] = 0;    // R
                    dest[dstIdx + 3] = 255;  // A
                }
                continue;
            }

            // Compute offset for this scanline
            float offset = computeOffset(y, tick, e, amplitude, frequency, compression, speed);

            for (int x = 0; x < w; ++x) {
                int srcX = x;
                int srcY = y;

                switch (e.type()) {
                    case DistortionType::HORIZONTAL:
                        srcX = (x + static_cast<int>(offset) + w) % w;
                        break;
                    case DistortionType::HORIZONTAL_INTERLACED:
                        srcX = (x + static_cast<int>(y % 2 == 0 ? -offset : offset) + w) % w;
                        break;
                    case DistortionType::VERTICAL: {
                        // For vertical: srcY = mod(floor(offset + y * compression), 256)
                        int modVal = static_cast<int>(std::floor(offset + y * compression));
                        srcY = ((modVal % h) + h) % h;
                        break;
                    }
                    default:
                        break;
                }

                int srcIdx = (srcY * w + srcX) * 4;
                int dstIdx = (y * w + x) * 4;

                for (int c = 0; c < 3; ++c) {
                    float srcVal = static_cast<float>(source[srcIdx + c]);
                    float dstVal = static_cast<float>(dest[dstIdx + c]);
                    dest[dstIdx + c] = static_cast<uint8_t>(dstVal * (1.0f - alpha) + srcVal * alpha);
                }
                dest[dstIdx + 3] = 255;
            }
        }
    }

    // JS-compatible interface: overlayFrame
    // Renders distorted source onto destination bitmap with alpha blending
    void overlayFrame(
        uint8_t* dest,
        int w,
        int h,
        const DistortionEffect& e,
        int letterbox,
        float tick,
        float alpha,
        bool erase
    ) {
        apply(dest, w, h, e, tick, alpha, erase, letterbox);
    }
};

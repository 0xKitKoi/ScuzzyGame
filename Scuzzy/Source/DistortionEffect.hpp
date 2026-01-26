#pragma once
#include "Rom.h"
#include "Block.h"


enum class DistortionType {
    HORIZONTAL = 1,
    HORIZONTAL_INTERLACED = 2,
    VERTICAL = 3
};

struct DistortionEffect {
    uint8_t data[17];

    explicit DistortionEffect(int index) {
        read(index);
    }

    void read(int index) {
        Block block = Block(0xF708 + index * 17);
        for (int i = 0; i < 17; ++i) {
            data[i] = block.readInt16();
        }
    }

    int16_t frequency() const {
        return int16_t(data[3] | (data[4] << 8));
    }

    int16_t amplitude() const {
        return int16_t(data[5] | (data[6] << 8));
    }

    int16_t compression() const {
        return int16_t(data[8] | (data[9] << 8));
    }

    int16_t frequencyAcceleration() const {
        return int16_t(data[10] | (data[11] << 8));
    }

    int16_t amplitudeAcceleration() const {
        return int16_t(data[12] | (data[13] << 8));
    }

    int16_t speed() const {
        return int16_t(data[14]);
    }

    int16_t compressionAcceleration() const {
        return int16_t(data[15] | (data[16] << 8));
    }

    DistortionType type() const {
        uint8_t t = data[2];
        // Sanitize invalid types: if not HORIZONTAL or VERTICAL, use HORIZONTAL_INTERLACED
        if (t == 1 || t == 3) {
            return static_cast<DistortionType>(t);
        } else {
            return DistortionType::HORIZONTAL_INTERLACED;
        }
    }
};

/* #include "Distorter.hpp"
#include <cstring>
#include <cmath>

void Distorter::apply(
    uint8_t* dest,
    int w,
    int h,
    const DistortionEffect& e,
    uint32_t tick,
    float alpha,
    bool clear
) {
    if (clear) {
        memset(dest, 0, w * h * 4);
    }

    for (int y = 0; y < h; ++y) {
        int offset = int(
            e.amplitude() *
            sin((e.frequency() * (tick + y)) * 0.01)
        );

        for (int x = 0; x < w; ++x) {
            int sx = x;
            int sy = y;

            if (e.type() == DistortionType::HORIZONTAL) {
                sx = (x + offset + w) % w;
            } else if (e.type() == DistortionType::VERTICAL) {
                sy = (y + offset + h) % h;
            }

            int src = (sy * w + sx) * 4;
            int dst = (y * w + x) * 4;

            for (int c = 0; c < 3; ++c) {
                dest[dst + c] =
                    dest[dst + c] * (1.0f - alpha) +
                    source[src + c] * alpha;
            }
            dest[dst + 3] = 255;
        }
    }
}
    */

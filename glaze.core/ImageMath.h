#pragma once

#include <cstdint>

namespace glaze {
    namespace core {

        typedef struct RgbColor
        {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        } RgbColor;

        typedef struct HsvColor
        {
            uint8_t h;
            uint8_t s;
            uint8_t v;
        } HsvColor;

        float ToRadians(uint8_t degrees);

        uint8_t ToDegrees(float radians);

        RgbColor HsvToRgb(HsvColor hsv);

        HsvColor RgbToHsv(RgbColor rgb);

        uint8_t GetLuma(RgbColor rgb);

        void GetSourceCropping(
            unsigned int sourceWidth,
            unsigned int sourceHeight,
            unsigned int destinationWidth,
            unsigned int destinationHeight,
            unsigned int& cellSize,
            unsigned int& croppedSourceWidth,
            unsigned int& croppedSourceHeight);
    }
}
#include "pch.h"

#include "ImageMath.h"
#define _USE_MATH_DEFINES
#include <cmath>

namespace glaze {
    namespace core {

        uint8_t GetLuma(RgbColor rgb)
        {
            return (0.3 * rgb.r) + (0.59 * rgb.g) + (0.11 * rgb.b);
        }

        RgbColor HsvToRgb(HsvColor hsv)
        {
            RgbColor rgb;
            unsigned char region, remainder, p, q, t;

            if (hsv.s == 0)
            {
                rgb.r = hsv.v;
                rgb.g = hsv.v;
                rgb.b = hsv.v;
                return rgb;
            }

            region = hsv.h / 43;
            remainder = (hsv.h - (region * 43)) * 6;

            p = (hsv.v * (255 - hsv.s)) >> 8;
            q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
            t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

            switch (region)
            {
            case 0:
                rgb.r = hsv.v; rgb.g = t; rgb.b = p;
                break;
            case 1:
                rgb.r = q; rgb.g = hsv.v; rgb.b = p;
                break;
            case 2:
                rgb.r = p; rgb.g = hsv.v; rgb.b = t;
                break;
            case 3:
                rgb.r = p; rgb.g = q; rgb.b = hsv.v;
                break;
            case 4:
                rgb.r = t; rgb.g = p; rgb.b = hsv.v;
                break;
            default:
                rgb.r = hsv.v; rgb.g = p; rgb.b = q;
                break;
            }

            return rgb;
        }

        HsvColor RgbToHsv(RgbColor rgb)
        {
            HsvColor hsv;
            unsigned char rgbMin, rgbMax;

            rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
            rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

            hsv.v = rgbMax;
            if (hsv.v == 0)
            {
                hsv.h = 0;
                hsv.s = 0;
                return hsv;
            }

            hsv.s = 255 * long(rgbMax - rgbMin) / hsv.v;
            if (hsv.s == 0)
            {
                hsv.h = 0;
                return hsv;
            }

            if (rgbMax == rgb.r)
                hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
            else if (rgbMax == rgb.g)
                hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
            else
                hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

            return hsv;
        }

        void GetSourceCropping(
            unsigned int sourceWidth,
            unsigned int sourceHeight,
            unsigned int destinationWidth,
            unsigned int destinationHeight,
            unsigned int& cellSize,
            unsigned int& croppedSourceWidth,
            unsigned int& croppedSourceHeight)
        {
            float sourceRatio = ((float)sourceWidth) / sourceHeight;
            float destinationRatio = ((float)destinationWidth) / destinationHeight;

            if (sourceRatio > destinationRatio)
            {
                croppedSourceHeight = sourceHeight;
                cellSize = ceil((float)destinationHeight / sourceHeight);
                croppedSourceWidth = ceil((float)destinationWidth / cellSize);
            }
            else if (sourceRatio < destinationRatio)
            {
                croppedSourceWidth = sourceWidth;
                cellSize = ceil((float)destinationWidth / sourceWidth);
                croppedSourceHeight = ceil((float)destinationHeight / cellSize);
            }
            else
            {
                croppedSourceWidth = sourceWidth;
                croppedSourceHeight = sourceHeight;
                cellSize = ceil((float)destinationWidth / sourceWidth);
            }
        }

        float ToRadians(uint8_t degrees)
        {
            float degrees_360 = ((float)degrees / 255) * 360;
            return degrees_360 * (M_PI / 180.0);
        }

        uint8_t ToDegrees(float radians)
        {
            float degrees_360 = (float)(((int)(radians  * (180.0 / M_PI)) + 360) % 360);
            return (uint8_t)((degrees_360 / 360) * 255);
        }
    }
}
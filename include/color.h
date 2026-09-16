// https://github.com/yourpov/gradify
#pragma once

typedef struct {
    int r, g, b;
} RgbColor;

RgbColor hexToRgb(const char *hex);

void gradientStops(RgbColor *outStops, int count, const char **hexColors, int hexColorCount);

void formatForeground(char *outBuffer, int bufferSize, RgbColor color);

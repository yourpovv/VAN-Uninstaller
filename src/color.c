#include "color.h"
#include <stdio.h>
#include <string.h>

#define MAX_GRADIENT_COLORS 16

RgbColor hexToRgb(const char *hex) {
    RgbColor color = { 0, 0, 0 };
    size_t length = strlen(hex);

    if (length == 3) {
        unsigned int r = 0, g = 0, b = 0;
        sscanf(hex, "%1x%1x%1x", &r, &g, &b);
        color.r = (int)(r * 17);
        color.g = (int)(g * 17);
        color.b = (int)(b * 17);
    } else if (length == 6) {
        unsigned int r = 0, g = 0, b = 0;
        sscanf(hex, "%2x%2x%2x", &r, &g, &b);
        color.r = (int)r;
        color.g = (int)g;
        color.b = (int)b;
    }

    return color;
}

void gradientStops(RgbColor *outStops, int count, const char **hexColors, int hexColorCount) {
    if (count <= 0 || hexColorCount <= 0) return;
    if (hexColorCount > MAX_GRADIENT_COLORS) hexColorCount = MAX_GRADIENT_COLORS;

    RgbColor colors[MAX_GRADIENT_COLORS];
    for (int i = 0; i < hexColorCount; i++) {
        colors[i] = hexToRgb(hexColors[i]);
    }

    if (count == 1) {
        outStops[0] = colors[0];
        return;
    }

    int segmentCount = hexColorCount - 1;
    for (int i = 0; i < count; i++) {
        double pos = (double)i / (double)(count - 1) * segmentCount;
        int idx1 = (int)pos;
        int idx2 = idx1 + 1;
        if (idx2 >= hexColorCount) idx2 = hexColorCount - 1;
        double fraction = pos - idx1;

        outStops[i].r = (int)(colors[idx1].r * (1 - fraction) + colors[idx2].r * fraction);
        outStops[i].g = (int)(colors[idx1].g * (1 - fraction) + colors[idx2].g * fraction);
        outStops[i].b = (int)(colors[idx1].b * (1 - fraction) + colors[idx2].b * fraction);
    }
}

void formatForeground(char *outBuffer, int bufferSize, RgbColor color) {
    snprintf(outBuffer, (size_t)bufferSize, "\x1b[38;2;%d;%d;%dm", color.r, color.g, color.b);
}

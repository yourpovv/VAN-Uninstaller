#include "banner.h"
#include "color.h"
#include "console.h"
#include "theme.h"
#include <stdio.h>
#include <string.h>

#define DIVIDER "─"

static const char *SUBTITLE = "Love from yourpov.dev";

static const char *NECTAR[] = {
    "ffb347", "ffa857", "ff9d67", "ff9277", "ff8787", "ff7c97", "ff6f91"
};
#define NECTAR_COUNT ((int)(sizeof(NECTAR) / sizeof(NECTAR[0])))

static const char *BANNER[] = {
    "    ██████████████████",
    "    ██████████████████",
    "     ████████████████",
    "      ███████████████",
    "       █████████████",
    "       ████████████",
    " ██      █████████      █",
    " ███     ████████     ███",
    " ████     ██████     █████",
    "███████    ████    ███████",
    "████████    ██    █████████",
    "█████████        █████████",
    "  █████████    █████████",
    "    ████████  ████████",
    "      ██████████████",
    "        ██████████",
    "          ██████",
    "            ███",
};
#define BANNER_ROWS ((int)(sizeof(BANNER) / sizeof(BANNER[0])))

static int utf8Width(const char *text) {
    int width = 0;
    for (const unsigned char *p = (const unsigned char *)text; *p; p++) {
        if ((*p & 0xC0) != 0x80) width++;
    }
    return width;
}

static void addSpaces(int count) {
    for (int i = 0; i < count; i++) putchar(' ');
}

static void Centered(const char *text, int consoleWidth) {
    int margin = (consoleWidth - utf8Width(text)) / 2;
    if (margin < 0) margin = 0;
    addSpaces(margin);
    printf("%s%s%s\n", ANSI_WHITE, text, ANSI_RESET);
}

void Banner(void) {
    int consoleWidth = getWidth();
    if (consoleWidth <= 0) consoleWidth = 80;

    int contentWidth = 0;
    for (int i = 0; i < BANNER_ROWS; i++) {
        int rowWidth = utf8Width(BANNER[i]);
        if (rowWidth > contentWidth) contentWidth = rowWidth;
    }

    int margin = (consoleWidth - contentWidth) / 2;
    if (margin < 0) margin = 0;

    RgbColor rowColors[BANNER_ROWS];
    gradientStops(rowColors, BANNER_ROWS, NECTAR, NECTAR_COUNT);

    char ansi[32];
    printf("\n");
    for (int i = 0; i < BANNER_ROWS; i++) {
        addSpaces(margin);
        formatForeground(ansi, sizeof(ansi), rowColors[i]);
        printf("%s%s%s\n", ansi, BANNER[i], ANSI_RESET);
    }

    addSpaces(margin);
    printf("%s", ANSI_WHITE);
    for (int i = 0; i < contentWidth; i++) fputs(DIVIDER, stdout);
    printf("%s\n", ANSI_RESET);

    Centered(SUBTITLE, consoleWidth);
    printf("\n");
}

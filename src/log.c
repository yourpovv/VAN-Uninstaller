#include "log.h"
#include "theme.h"
#include <stdio.h>
#include <string.h>

static int isOk(const char *tag) {
    return _stricmp(tag, "OK") == 0;
}

void logLine(const char *tag, const char *message) {
    const char *color = isOk(tag) ? ANSI_GREEN : ANSI_RED;
    const char *pad = isOk(tag) ? "    " : " ";
    printf(ANSI_WHITE "[" ANSI_RESET "%s%s" ANSI_RESET ANSI_WHITE "]" ANSI_RESET "%s%s\n",
           color, tag, pad, message);
}

void logAction(const char *tag, const char *message) {
    const char *color = isOk(tag) ? ANSI_GREEN : ANSI_RED;
    printf(ANSI_WHITE "[" ANSI_RESET "%s%s" ANSI_RESET ANSI_WHITE "] " ANSI_RESET "%s\n",
           color, tag, message);
}

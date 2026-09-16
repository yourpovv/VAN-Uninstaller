#pragma once
#include <stdbool.h>

typedef struct {
    bool vgcService;
    bool vgkService;
    bool installFolder;
    bool clean;
} VanguardTraces;

VanguardTraces checkTraces(void);

bool Uninstall(void);

void restartRiotClient(void);

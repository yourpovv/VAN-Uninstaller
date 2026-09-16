#include "vanguard.h"
#include "log.h"
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VANGUARD_FOLDER_NAME "Riot Vanguard"

static void vanguardFolderPath(char *outPath, size_t outSize) {
    char programFiles[MAX_PATH] = { 0 };
    GetEnvironmentVariableA("ProgramFiles", programFiles, sizeof(programFiles));
    snprintf(outPath, outSize, "%s\\%s", programFiles, VANGUARD_FOLDER_NAME);
}

static bool folderExists(const char *path) {
    DWORD attrs = GetFileAttributesA(path);
    return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY);
}

static bool serviceInstalled(const char *serviceName) {
    char command[128];
    snprintf(command, sizeof(command), "sc query %s >nul 2>&1", serviceName);
    return system(command) == 0;
}

static bool readDefaultInstallPath(char *outPath, size_t outSize) {
    char programData[MAX_PATH] = { 0 };
    if (!GetEnvironmentVariableA("ProgramData", programData, sizeof(programData))) return false;

    char jsonPath[MAX_PATH];
    snprintf(jsonPath, sizeof(jsonPath), "%s\\Riot Games\\RiotClientInstalls.json", programData);

    FILE *file = fopen(jsonPath, "rb");
    if (!file) return false;

    char buffer[4096] = { 0 };
    size_t bytesRead = fread(buffer, 1, sizeof(buffer) - 1, file);
    fclose(file);
    buffer[bytesRead] = '\0';

    const char *key = strstr(buffer, "\"rc_default\"");
    if (!key) return false;
    const char *colon = strchr(key, ':');
    if (!colon) return false;
    const char *value = strchr(colon, '"');
    if (!value) return false;
    value++;

    size_t outIndex = 0;
    for (const char *p = value; *p && *p != '"' && outIndex < outSize - 1; p++) {
        if (*p == '\\' && *(p + 1)) p++;
        outPath[outIndex++] = *p;
    }
    outPath[outIndex] = '\0';
    return outIndex > 0;
}

static bool scanDrivesForRiotClient(char *outPath, size_t outSize) {
    static const char *KNOWN_PATHS[] = {
        "Riot Games\\Riot Client\\RiotClientServices.exe",
        "Program Files\\Riot Games\\Riot Client\\RiotClientServices.exe",
    };

    for (char driveLetter = 'C'; driveLetter <= 'Z'; driveLetter++) {
        for (size_t i = 0; i < sizeof(KNOWN_PATHS) / sizeof(KNOWN_PATHS[0]); i++) {
            snprintf(outPath, outSize, "%c:\\%s", driveLetter, KNOWN_PATHS[i]);
            if (GetFileAttributesA(outPath) != INVALID_FILE_ATTRIBUTES) return true;
        }
    }
    return false;
}

static bool findRiotClientPath(char *outPath, size_t outSize) {
    if (readDefaultInstallPath(outPath, outSize) && GetFileAttributesA(outPath) != INVALID_FILE_ATTRIBUTES) {
        return true;
    }
    return scanDrivesForRiotClient(outPath, outSize);
}

static void killProcess(const char *processName) {
    char message[160];
    snprintf(message, sizeof(message), "Closing %s...", processName);
    logAction("OK", message);

    char command[128];
    snprintf(command, sizeof(command), "taskkill /f /im \"%s\" >nul 2>&1", processName);
    system(command);
}

static void stopService(const char *serviceName) {
    char message[96];
    snprintf(message, sizeof(message), "Stopping service: %s", serviceName);
    logAction("OK", message);

    char command[64];
    snprintf(command, sizeof(command), "sc stop %s >nul 2>&1", serviceName);
    system(command);
}

static void deleteService(const char *serviceName) {
    char message[96];
    snprintf(message, sizeof(message), "Deleting service: %s", serviceName);
    logAction("OK", message);

    char command[64];
    snprintf(command, sizeof(command), "sc delete %s >nul 2>&1", serviceName);
    system(command);
}

VanguardTraces checkTraces(void) {
    VanguardTraces traces = { 0 };
    char folder[MAX_PATH];
    vanguardFolderPath(folder, sizeof(folder));

    traces.vgcService = serviceInstalled("vgc");
    traces.vgkService = serviceInstalled("vgk");
    traces.installFolder = folderExists(folder);
    traces.clean = !traces.vgcService && !traces.vgkService && !traces.installFolder;
    return traces;
}

bool Uninstall(void) {
    killProcess("vgtray.exe");
    killProcess("vgc.exe");
    killProcess("vgk.exe");
    printf("\n");

    stopService("vgc");
    stopService("vgk");
    printf("\n");

    deleteService("vgc");
    deleteService("vgk");
    printf("\n");

    char folder[MAX_PATH];
    vanguardFolderPath(folder, sizeof(folder));

    if (!folderExists(folder)) {
        char message[MAX_PATH + 16];
        snprintf(message, sizeof(message), "%s not found", folder);
        logAction("OK", message);
        return true;
    }

    char deletingMessage[MAX_PATH + 16];
    snprintf(deletingMessage, sizeof(deletingMessage), "Deleting %s\\", folder);
    logAction("OK", deletingMessage);

    char command[MAX_PATH + 32];
    snprintf(command, sizeof(command), "rd /s /q \"%s\" >nul 2>&1", folder);
    system(command);

    if (folderExists(folder)) {
        logAction("FOUND", "Some files could not be removed. restart pc and rerun this.");
        return false;
    }

    char message[MAX_PATH + 16];
    snprintf(message, sizeof(message), "Removed %s", folder);
    logAction("OK", message);
    return true;
}

void restartRiotClient(void) {
    killProcess("VALORANT-Win64-Shipping.exe");
    killProcess("RiotClientUx.exe");
    killProcess("RiotClientServices.exe");
    Sleep(3000);

    char riotClientPath[MAX_PATH];
    if (findRiotClientPath(riotClientPath, sizeof(riotClientPath))) {
        logAction("OK", "Reopening Riot Client....");
        ShellExecuteA(NULL, "open", riotClientPath, NULL, NULL, SW_SHOWNORMAL);
    } else {
        logAction("FOUND", "Could not find Riot Client. Open it manually");
    }
}

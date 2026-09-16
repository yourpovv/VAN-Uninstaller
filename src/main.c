#include <windows.h>
#include <stdio.h>
#include "banner.h"
#include "console.h"
#include "log.h"
#include "theme.h"
#include "vanguard.h"

static char getChoice(void) {
    printf(ANSI_WHITE "Select an option: " ANSI_RESET);
    char line[16];
    if (!fgets(line, sizeof(line), stdin)) return '\0';
    return line[0];
}

static int getAnswer(const char *prompt) {
    for (;;) {
        printf(ANSI_WHITE "%s" ANSI_RESET, prompt);
        char line[16];
        if (!fgets(line, sizeof(line), stdin)) return 0;
        if (line[0] == 'y' || line[0] == 'Y') return 1;
        if (line[0] == 'n' || line[0] == 'N') return 0;
        printf("Enter Y or N.\n");
    }
}

static void pressEnter(const char *prompt) {
    printf(ANSI_WHITE "%s" ANSI_RESET, prompt);
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { }
}

static void showMenu(void) {
    system("cls");
    Banner();
    printf(ANSI_WHITE "[1]" ANSI_RESET ANSI_GREEN " Check for traces" ANSI_RESET "\n");
    printf(ANSI_WHITE "[2]" ANSI_RESET ANSI_RED " Uninstall Vanguard" ANSI_RESET "\n");
    printf(ANSI_WHITE "[0] Exit" ANSI_RESET "\n");
    printf("\n");
}

static void doCheck(void) {
    system("cls");
    Banner();
    printf(ANSI_WHITE "Checking for Vanguard traces..." ANSI_RESET "\n\n");

    VanguardTraces traces = checkTraces();

    logLine(traces.vgcService ? "FOUND" : "OK",
            traces.vgcService ? "vgc service is installed" : "vgc service not found");
    logLine(traces.vgkService ? "FOUND" : "OK",
            traces.vgkService ? "vgk service is installed" : "vgk service not found");
    logLine(traces.installFolder ? "FOUND" : "OK",
            traces.installFolder ? "Install folder exists" : "Vanguard folder not found");

    printf("\n");
    logLine(traces.clean ? "OK" : "FOUND",
            traces.clean ? "No Vanguard traces found." : "Vanguard traces.");

    pressEnter("\nPress Enter to go back...");
}

static void doUninstall(void) {
    system("cls");
    Banner();
    printf(ANSI_WHITE "Uninstalling Vanguard..." ANSI_RESET "\n\n");

    VanguardTraces traces = checkTraces();

    if (traces.clean) {
        logLine("OK", "No Vanguard traces found.");
        printf("\n");
    } else if (Uninstall()) {
        printf("\n" ANSI_RED "Vanguard has been uninstalled." ANSI_RESET "\n\n");
    } else {
        printf("\n");
        logLine("FOUND", "Vanguard was only partially removed. some files are still in use.");
        printf("\n");
    }

    if (getAnswer("Restart Riot Client? (Y/N): ")) {
        printf("\n");
        restartRiotClient();
    }

    pressEnter("\nPress Enter to go back...");
}

int main(void) {
    SetConsoleOutputCP(CP_UTF8);
    ansiSupport();
    SetConsoleTitleA("Vanguard Uninstaller");

    for (;;) {
        showMenu();
        char choice = getChoice();
        if (choice == '1') doCheck();
        else if (choice == '2') doUninstall();
        else if (choice == '0') break;
    }

    return 0;
}

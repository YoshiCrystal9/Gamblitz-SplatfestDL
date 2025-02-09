#include <curl/curl.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h> // chdir
#include <dirent.h> // mkdir
#include <switch.h>

#include "download.h"
#include "unzip.h"


#define ROOT                    "/"
#define APP_PATH                "/switch/gamblitz-splatfestdl/"
#define APP_OUTPUT              "/switch/gamblitz-splatfestdl/gamblitz-splatfestdl.nro"
#define OLD_APP_PATH            "/switch/gamblitz-splatfestdl.nro"

#define APP_VERSION             "1.1.0"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static const char *OPTION_LIST[] =
{
    "= Download latest Splatfest BCAT (All Regions)",
    //"= Download latest Project Gamblitz",
    "= Update this app"
};

static void refreshScreen(int cursor)
{
    consoleClear();

    printf("\x1B[36mGamblitz Splatfest-DL: v%s.\x1B[37m\n\n\n", APP_VERSION);
    printf("Press (A) to select option\n\n");
    printf("Press (+) to exit\n\n\n");

    for (int i = 0; i < ARRAY_SIZE(OPTION_LIST); i++)
    {
        printf("[%c] %s\n\n", cursor == i ? 'X' : ' ', OPTION_LIST[i]);
    }

    consoleUpdate(NULL);
}

static void printDisplay(const char *text, ...)
{
    va_list v;
    va_start(v, text);
    vfprintf(stdout, text, v);
    va_end(v);
    consoleUpdate(NULL);
}

// update the cursor so that it wraps around
static void update_cursor(int* cur, int new_value, int max)
{
    if (new_value >= max) new_value = 0;
    else if (new_value < 0) new_value = max - 1;

    *cur = new_value;
    refreshScreen(new_value);
}

// this is called before main
void userAppInit(void)
{
    appletLockExit();
    consoleInit(NULL);
    socketInitializeDefault();
}

// this is called after main exits
void userAppExit(void)
{
    socketExit();
    consoleExit(NULL);
    appletUnlockExit();
}

// where the app starts
int main(int argc, char **argv)
{
    curl_global_init(CURL_GLOBAL_ALL);

    // init stuff
    mkdir(APP_PATH, 0777);

    // change directory to root (defaults to /switch/)
    chdir(ROOT);

    // set the cursor position to 0
    int cursor = 0;

    PadState pad = {0};
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);

    // main menu
    refreshScreen(cursor);

    // muh loooooop
    while(appletMainLoop())
    {
        padUpdate(&pad);
        const u64 kDown = padGetButtonsDown(&pad);

        // move cursor down...
        if (kDown & HidNpadButton_AnyDown)
        {
            update_cursor(&cursor, cursor - 1, ARRAY_SIZE(OPTION_LIST));
        }

        // move cursor up...
        if (kDown & HidNpadButton_AnyUp)
        {
            update_cursor(&cursor, cursor + 1, ARRAY_SIZE(OPTION_LIST));
        }

        if (kDown & HidNpadButton_A)
        {
            switch (cursor)
            {
                case UP_SIGS:
                    if (downloadFile(AMS_SIG_URL, TEMP_FILE, OFF))
                    {
                        unzip(TEMP_FILE);
                    }
                    else
                    {
                        printDisplay("Failed to download files!\n");
                    }
                    break;

                case UP_APP:
                    if (downloadFile(APP_URL, TEMP_FILE, OFF))
                    {
                        remove(APP_OUTPUT);
                        rename(TEMP_FILE, APP_OUTPUT);
                        remove(OLD_APP_PATH);
                    }
                    else
                    {
                        printDisplay("Failed to download app update!\n");
                    }
                    break;
            }
        }

        // exit...
        if (kDown & HidNpadButton_Plus) break;

        // 1e+9 = 1 second
        svcSleepThread(1e+9 / 60);
    }

    curl_global_cleanup();
    return 0;
}

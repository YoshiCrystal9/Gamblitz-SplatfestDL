#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#define AMS_SIG_URL     "https://github.com/YoshiCrystal9/Gamblitz-Splatfest-BCAT/releases/download/release/atmosphere.zip"
#define APP_URL         "https://github.com/YoshiCrystal9/Gamblitz-SplatfestDL/releases/download/release/switch.zip"
#define TEMP_FILE       "/switch/gamblitz-splatfestdl/temp"

#define ON              1
#define OFF             0


#include <stdbool.h>

//
bool downloadFile(const char *url, const char *output, int api);

#endif

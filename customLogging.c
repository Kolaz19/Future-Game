#include "include/customLogging.h"
#include "include/raylib/raylib.h"
#include "include/slog.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// If true, output is only made to console
// Full logging is enabled
#define DEBUG false

static void customLog(int msgType, const char *text, va_list args) {
    char buffer[256];
    vsnprintf(buffer, 255, text, args);
    switch (msgType) {
    case LOG_INFO:
        slog_info(buffer);
        break;
    case LOG_ERROR:
        slog_error(buffer);
        break;
    case LOG_WARNING:
        slog_warn(buffer);
        break;
    case LOG_DEBUG:
        slog_debug(buffer);
        break;
    default:
        break;
    }
}

void initLogger(int argc, char *argv[]) {
    uint16_t enabledLevels = SLOG_FLAGS_ALL;
    slog_init("log.txt", enabledLevels, 0);
    slog_config_t slog_conf;
    /* Setup configuration parameters */
    strcpy(slog_conf.sFileName, "log.txt");
    strcpy(slog_conf.sFilePath, "./");
    strcpy(slog_conf.sSeparator, " ");
    slog_conf.logCallback = NULL;
    slog_conf.pCallbackCtx = NULL;
    slog_conf.eColorFormat = SLOG_COLORING_DISABLE;
    slog_conf.eDateControl = SLOG_TIME_ONLY;
    slog_conf.nTraceTid = 0;
    slog_conf.nUseHeap = 0;
    slog_conf.nIndent = 1;
    slog_conf.nRotate = 0;
    slog_conf.nFlush = 1;
    slog_conf.nToFile = 1;
    slog_conf.nToScreen = 0;

    // Let user have control over output in file
    if (argc == 3 && strcmp(argv[1], "-log") == 0) {
        if (strcmp(argv[2], "trace") == 0) {
            slog_conf.nFlags = SLOG_FLAGS_ALL;
            slog_conf.nKeepOpen = 1;
        } else if (strcmp(argv[2], "debug") == 0) {
            slog_conf.nFlags = SLOG_DEBUG | SLOG_INFO | SLOG_WARN |
                               SLOG_ERROR | SLOG_FATAL;
            slog_conf.nKeepOpen = 1;
        } else if (strcmp(argv[2], "standard") == 0) {
            slog_conf.nFlags = SLOG_INFO | SLOG_WARN |
                               SLOG_ERROR | SLOG_FATAL;
            slog_conf.nKeepOpen = 1;
        } else {
            slog_conf.nFlags = SLOG_ERROR | SLOG_FATAL;
            slog_conf.nKeepOpen = 0;
        }
    } else {
        slog_conf.nFlags = SLOG_ERROR | SLOG_FATAL;
        slog_conf.nKeepOpen = 0;
    }

    if (DEBUG) {
        slog_conf.nToFile = 0;
        slog_conf.nToScreen = 1;
        slog_conf.nFlags =  SLOG_INFO | SLOG_WARN |
                           SLOG_ERROR | SLOG_FATAL;
        slog_conf.eColorFormat = SLOG_COLORING_TAG;
    }
    slog_config_set(&slog_conf);
    SetTraceLogCallback(customLog);
}

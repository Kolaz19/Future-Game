#include "include/customLogging.h"
#include "include/raylib/raylib.h"
#include "include/slog.h"
#include <stdio.h>
#include <string.h>
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

void initLogger(void) {
    uint16_t enabledLevels = SLOG_FLAGS_ALL;
    slog_init("log.txt", enabledLevels, 0);
    slog_config_t slog_conf;
    /* Setup configuration parameters */
    strcpy(slog_conf.sFileName, "log.txt");
    strcpy(slog_conf.sFilePath, "./");
    strcpy(slog_conf.sSeparator, " ");
    slog_conf.logCallback = NULL;
    slog_conf.pCallbackCtx = NULL;
    slog_conf.eColorFormat = SLOG_COLORING_TAG;
    slog_conf.eDateControl = SLOG_TIME_ONLY;
    slog_conf.nKeepOpen = 1;
    slog_conf.nTraceTid = 0; //CHECK
    slog_conf.nToScreen = 1;
    slog_conf.nUseHeap = 0;

    slog_conf.nToFile = 0;

	slog_conf.nIndent = 1;
	slog_conf.nRotate = 0;
    slog_conf.nFlush = 1;
    slog_conf.nFlags = SLOG_FLAGS_ALL;

    /* Tread safe call to update slog configuration */
    slog_config_set(&slog_conf);
    SetTraceLogCallback(customLog);
}

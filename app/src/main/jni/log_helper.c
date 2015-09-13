#include "log_helper.h"


void LogE(char* tag, char* error) {
    __android_log_write(ANDROID_LOG_ERROR, tag, error);
}
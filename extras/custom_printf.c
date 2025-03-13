// custom_printf.c
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define CAPTURE_BUFFER_SIZE 8192

static char captureBuffer[CAPTURE_BUFFER_SIZE];
static int captureOffset = 0;

// Return pointer to the entire captured text
char* get_output() {
    return captureBuffer;
}

// Helper function that appends to the buffer.
static int writeToCaptureBuffer(const char* format, va_list args) {
    int spaceLeft = CAPTURE_BUFFER_SIZE - captureOffset;
    if (spaceLeft <= 0) {
        return 0; // no space left
    }

    // vsnprintf writes up to 'spaceLeft' bytes (including '\0')
    int written = vsnprintf(captureBuffer + captureOffset, spaceLeft, format, args);
    if (written < 0) {
        return 0;
    }

    // If vsnprintf truncated, we only advance up to the last valid char
    if (written >= spaceLeft) {
        captureOffset += spaceLeft - 1; // skip the null terminator
        captureBuffer[captureOffset] = '\0';
    } else {
        captureOffset += written;
    }

    return written;
}

// Override printf(...)
int printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = writeToCaptureBuffer(format, args);
    va_end(args);
    return ret;
}

// Override fprintf(...)
int fprintf(FILE *stream, const char* format, ...) {
    va_list args;
    va_start(args, format);

    int ret = 0;
    ret = writeToCaptureBuffer(format, args);

    va_end(args);
    return ret;
}

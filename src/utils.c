#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "utils.h"

int file_exists(const char* filename)
{
    struct stat buffer;
    return stat(filename, &buffer) == 0;
}

long file_size(const char* filename)
{
    struct stat buffer;
    if (stat(filename, &buffer) == 0) {
        return buffer.st_size;
    }
    return -1;
}

int file_read_all(const char* filename, char* buffer, size_t buffer_size)
{
    FILE* f;
    size_t bytes_read;

    f = fopen(filename, "rb");
    if (!f) return 1;

    bytes_read = fread(buffer, 1, buffer_size - 1, f);
    buffer[bytes_read] = '\0';

    fclose(f);
    return 0;
}

int file_write_all(const char* filename, const char* buffer, size_t size)
{
    FILE* f;
    size_t bytes_written;

    f = fopen(filename, "wb");
    if (!f) return 1;

    bytes_written = fwrite(buffer, 1, size, f);
    fclose(f);

    return bytes_written != size;
}

char* file_extension(const char* filename)
{
    char* ext = strrchr((char*)filename, '.');
    return ext ? ext : NULL;
}

char* string_escape(const char* input, size_t* output_size)
{
    size_t input_len = strlen(input);
    char* output = malloc(input_len * 2 + 1);
    size_t out_idx = 0;
    size_t i;

    if (!output) return NULL;

    for (i = 0; i < input_len; i++) {
        unsigned char c = input[i];

        switch (c) {
            case '\n':
                output[out_idx++] = '\\';
                output[out_idx++] = 'n';
                break;
            case '\r':
                output[out_idx++] = '\\';
                output[out_idx++] = 'r';
                break;
            case '\t':
                output[out_idx++] = '\\';
                output[out_idx++] = 't';
                break;
            case '\\':
                output[out_idx++] = '\\';
                output[out_idx++] = '\\';
                break;
            case '"':
                output[out_idx++] = '\\';
                output[out_idx++] = '"';
                break;
            default:
                if (c < 32 || c > 126) {
                    out_idx += snprintf(&output[out_idx], 5, "\\x%02x", c);
                } else {
                    output[out_idx++] = c;
                }
                break;
        }
    }

    output[out_idx] = '\0';
    *output_size = out_idx;

    return output;
}

void error_print(const char* format, ...)
{
    va_list args;
    fprintf(stderr, "[ERROR] ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

void info_print(const char* format, ...)
{
    va_list args;
    printf("[INFO] ");
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void debug_print(const char* format, ...)
{
    va_list args;
    fprintf(stderr, "[DEBUG] ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

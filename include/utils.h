#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

/**
 * Check if file exists
 */
int file_exists(const char* filename);

/**
 * Get file size
 */
long file_size(const char* filename);

/**
 * Read entire file into buffer
 */
int file_read_all(const char* filename, char* buffer, size_t buffer_size);

/**
 * Write buffer to file
 */
int file_write_all(const char* filename, const char* buffer, size_t size);

/**
 * Get file extension
 */
char* file_extension(const char* filename);

/**
 * Escape string for C code
 */
char* string_escape(const char* input, size_t* output_size);

/**
 * Print error message
 */
void error_print(const char* format, ...);

/**
 * Print info message
 */
void info_print(const char* format, ...);

/**
 * Print debug message
 */
void debug_print(const char* format, ...);

#endif // UTILS_H

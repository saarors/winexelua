#ifndef EMBEDDER_H
#define EMBEDDER_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char lua_code[65536];
    char output_file[256];
    char icon_file[256];
    char version[32];
    int console_mode;
} EmbedderOptions;

/**
 * Create a new embedder
 */
EmbedderOptions* embedder_options_new(void);

/**
 * Free embedder options
 */
void embedder_options_free(EmbedderOptions* opts);

/**
 * Read Lua source file
 */
int embedder_read_lua_file(const char* filename, char* buffer, size_t buffer_size);

/**
 * Generate C source with embedded Lua
 */
int embedder_generate_source(EmbedderOptions* opts, const char* output_source);

/**
 * Compile generated source to executable
 */
int embedder_compile_source(const char* source_file, const char* output_file,
                           const char* icon_file, int console_mode);

#endif // EMBEDDER_H

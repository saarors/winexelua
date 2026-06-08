#ifndef EMBEDDER_H
#define EMBEDDER_H

#include <stddef.h>

typedef struct {
    char lua_code[1024 * 1024];
    char output_file[256];
    char icon_file[256];
    char version[64];
    int console_mode;

    char lua_include[256];
    char lua_lib[256];

} EmbedderOptions;

EmbedderOptions* embedder_options_new(void);
void embedder_options_free(EmbedderOptions* opts);

int embedder_read_lua_file(const char* filename, char* buffer, size_t buffer_size);

int embedder_generate_source(EmbedderOptions* opts, const char* output_source);

int embedder_compile_source(const char* source_file,
                            const char* output_file,
                            const char* icon_file,
                            int console_mode);

#endif

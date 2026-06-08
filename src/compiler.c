#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "compiler.h"
#include "embedder.h"
#include "utils.h"

CompilerOptions* compiler_options_new(void)
{
    CompilerOptions* opts = malloc(sizeof(CompilerOptions));
    if (!opts) return NULL;

    memset(opts->input_file, 0, sizeof(opts->input_file));
    memset(opts->output_file, 0, sizeof(opts->output_file));
    memset(opts->icon_file, 0, sizeof(opts->icon_file));
    memset(opts->version, 0, sizeof(opts->version));
    opts->console_mode = 0;
    opts->verbose = 0;

    return opts;
}

void compiler_options_free(CompilerOptions* opts)
{
    if (opts) free(opts);
}

int compiler_parse_args(int argc, char* argv[], CompilerOptions* opts)
{
    int i;
    int input_set = 0;

    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
                if (i + 1 < argc) {
                    strncpy(opts->output_file, argv[i + 1], sizeof(opts->output_file) - 1);
                    i++;
                } else return 1;
            } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--icon") == 0) {
                if (i + 1 < argc) {
                    strncpy(opts->icon_file, argv[i + 1], sizeof(opts->icon_file) - 1);
                    i++;
                } else return 1;
            } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
                if (i + 1 < argc) {
                    strncpy(opts->version, argv[i + 1], sizeof(opts->version) - 1);
                    i++;
                } else return 1;
            } else if (strcmp(argv[i], "-c") == 0) {
                opts->console_mode = 1;
            } else if (strcmp(argv[i], "--verbose") == 0) {
                opts->verbose = 1;
            } else if (strcmp(argv[i], "-h") == 0) {
                compiler_show_help(argv[0]);
                return 1;
            } else {
                error_print("Unknown option: %s\n", argv[i]);
                return 1;
            }
        } else {
            if (!input_set) {
                strncpy(opts->input_file, argv[i], sizeof(opts->input_file) - 1);
                input_set = 1;
            } else {
                return 1;
            }
        }
    }

    if (!input_set) return 1;

    if (!file_exists(opts->input_file)) {
        error_print("Input not found: %s\n", opts->input_file);
        return 1;
    }

    if (opts->output_file[0] == '\0') {
        strncpy(opts->output_file, opts->input_file, sizeof(opts->output_file) - 1);
        char* ext = file_extension(opts->output_file);
        if (ext) strcpy(ext, ".exe");
        else strncat(opts->output_file, ".exe",
            sizeof(opts->output_file) - strlen(opts->output_file) - 1);
    }

    return 0;
}

int compiler_compile(CompilerOptions* opts)
{
    EmbedderOptions* embed_opts;
    char temp_source[256];
    char debug_file[256];

    embed_opts = embedder_options_new();
    if (!embed_opts) return 1;

    if (embedder_read_lua_file(opts->input_file,
                               embed_opts->lua_code,
                               sizeof(embed_opts->lua_code)) != 0)
        return 1;

    strncpy(embed_opts->output_file, opts->output_file, sizeof(embed_opts->output_file) - 1);
    strncpy(embed_opts->icon_file, opts->icon_file, sizeof(embed_opts->icon_file) - 1);
    strncpy(embed_opts->version, opts->version, sizeof(embed_opts->version) - 1);
    embed_opts->console_mode = opts->console_mode;

    snprintf(temp_source, sizeof(temp_source), "_temp_%lu.c", (unsigned long)getpid());
    snprintf(debug_file, sizeof(debug_file), "debug_%lu.c", (unsigned long)getpid());

    if (embedder_generate_source(embed_opts, temp_source) != 0)
        return 1;

    /* debug copy */
    FILE* src = fopen(temp_source, "rb");
    if (src) {
        FILE* dst = fopen(debug_file, "wb");
        if (dst) {
            char buf[4096];
            size_t bytes;
            while ((bytes = fread(buf, 1, sizeof(buf), src)) > 0)
                fwrite(buf, 1, bytes, dst);
            fclose(dst);
        }
        fclose(src);
    }

    if (opts->verbose) {
        info_print("DEBUG: input file = %s\n", opts->input_file);
        info_print("DEBUG: output file = %s\n", opts->output_file);
    }

    int result = embedder_compile_source(temp_source,
                                         opts->output_file,
                                         opts->icon_file,
                                         opts->console_mode);

    FILE* check = fopen(opts->output_file, "rb");
    if (!check) {
        error_print("FAILED: EXE was not created\n");
        return 1;
    }
    fclose(check);

    embedder_options_free(embed_opts);
    return result;
}

void compiler_show_help(const char* program_name)
{
    printf("Usage: %s <input.lua> [options]\n", program_name);
}

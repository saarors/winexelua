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
                } else {
                    error_print("Error: -o requires an argument\n");
                    return 1;
                }
            } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--icon") == 0) {
                if (i + 1 < argc) {
                    strncpy(opts->icon_file, argv[i + 1], sizeof(opts->icon_file) - 1);
                    i++;
                } else {
                    error_print("Error: -i requires an argument\n");
                    return 1;
                }
            } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
                if (i + 1 < argc) {
                    strncpy(opts->version, argv[i + 1], sizeof(opts->version) - 1);
                    i++;
                } else {
                    error_print("Error: -v requires an argument\n");
                    return 1;
                }
            } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--console") == 0) {
                opts->console_mode = 1;
            } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                compiler_show_help(argv[0]);
                return 1;
            } else if (strcmp(argv[i], "--verbose") == 0) {
                opts->verbose = 1;
            } else {
                error_print("Error: Unknown option '%s'\n", argv[i]);
                return 1;
            }
        } else {
            if (!input_set) {
                strncpy(opts->input_file, argv[i], sizeof(opts->input_file) - 1);
                input_set = 1;
            } else {
                error_print("Error: Multiple input files not supported\n");
                return 1;
            }
        }
    }

    if (!input_set) {
        error_print("Error: No input file specified\n");
        return 1;
    }

    if (!file_exists(opts->input_file)) {
        error_print("Error: Input file '%s' not found\n", opts->input_file);
        return 1;
    }

    if (opts->output_file[0] == '\0') {
        strncpy(opts->output_file, opts->input_file, sizeof(opts->output_file) - 1);
        char* ext = file_extension(opts->output_file);
        if (ext) {
            strcpy(ext, ".exe");
        } else {
            strncat(opts->output_file, ".exe", sizeof(opts->output_file) - strlen(opts->output_file) - 1);
        }
    }

    return 0;
}

int compiler_compile(CompilerOptions* opts)
{
    EmbedderOptions* embed_opts;
    char temp_source[256];
    int result;

    embed_opts = embedder_options_new();
    if (!embed_opts) {
        error_print("Failed to allocate memory for embedder options\n");
        return 1;
    }

    if (embedder_read_lua_file(opts->input_file, embed_opts->lua_code, sizeof(embed_opts->lua_code)) != 0) {
        error_print("Failed to read Lua file\n");
        embedder_options_free(embed_opts);
        return 1;
    }

    strncpy(embed_opts->output_file, opts->output_file, sizeof(embed_opts->output_file) - 1);
    strncpy(embed_opts->icon_file, opts->icon_file, sizeof(embed_opts->icon_file) - 1);
    strncpy(embed_opts->version, opts->version, sizeof(embed_opts->version) - 1);
    embed_opts->console_mode = opts->console_mode;

    snprintf(temp_source, sizeof(temp_source), "_temp_%lu.c", (unsigned long)getpid());

    if (embedder_generate_source(embed_opts, temp_source) != 0) {
        error_print("Failed to generate C source\n");
        embedder_options_free(embed_opts);
        return 1;
    }

    if (opts->verbose) {
        info_print("Lua code size: %zu bytes\n", strlen(embed_opts->lua_code));
        info_print("First 100 chars: %.100s\n", embed_opts->lua_code);
    }

    result = embedder_compile_source(temp_source, opts->output_file, opts->icon_file, opts->console_mode);

    /* Keep temp file for debugging - do NOT delete it */
    info_print("Debug: Keeping temp source at: %s\n", temp_source);

    embedder_options_free(embed_opts);

    return result;
}

void compiler_show_help(const char* program_name)
{
    printf("\nUsage: %s <input.lua> [options]\n\n", program_name);
    printf("Options:\n");
    printf("  -o, --output <file>      Output executable filename (default: input.exe)\n");
    printf("  -i, --icon <file>        Custom application icon (ICO format)\n");
    printf("  -v, --version <ver>      Application version string\n");
    printf("  -c, --console            Enable console window\n");
    printf("  --verbose                Enable verbose output\n");
    printf("  -h, --help               Display this help message\n\n");
    printf("Examples:\n");
    printf("  %s hello.lua\n", program_name);
    printf("  %s hello.lua -o hello.exe -c\n", program_name);
    printf("  %s game.lua -o game.exe -i icon.ico\n\n", program_name);
}

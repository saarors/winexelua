#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char input_file[256];
    char output_file[256];
    char icon_file[256];
    char version[32];
    int console_mode;
    int verbose;
} CompilerOptions;

/**
 * Initialize compiler options with defaults
 */
CompilerOptions* compiler_options_new(void);

/**
 * Free compiler options
 */
void compiler_options_free(CompilerOptions* opts);

/**
 * Parse command line arguments
 */
int compiler_parse_args(int argc, char* argv[], CompilerOptions* opts);

/**
 * Compile Lua file to EXE
 */
int compiler_compile(CompilerOptions* opts);

/**
 * Display usage/help information
 */
void compiler_show_help(const char* program_name);

#endif // COMPILER_H

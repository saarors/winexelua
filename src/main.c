#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"
#include "utils.h"

int main(int argc, char* argv[])
{
    CompilerOptions* opts;
    int result;

    if (argc < 2) {
        printf("WinExeLua - Lua to EXE Compiler\n");
        printf("Usage: %s <input.lua> [options]\n\n", argv[0]);
        compiler_show_help(argv[0]);
        return 1;
    }

    opts = compiler_options_new();
    if (!opts) {
        error_print("Failed to allocate memory for compiler options\n");
        return 1;
    }

    result = compiler_parse_args(argc, argv, opts);
    if (result != 0) {
        compiler_options_free(opts);
        return result;
    }

    info_print("Compiling Lua to EXE...\n");
    info_print("Input: %s\n", opts->input_file);
    info_print("Output: %s\n", opts->output_file);

    result = compiler_compile(opts);

    if (result == 0) {
        info_print("Compilation successful!\n");
    } else {
        error_print("Compilation failed!\n");
    }

    compiler_options_free(opts);
    return result;
}

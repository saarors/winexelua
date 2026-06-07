#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "embedder.h"
#include "utils.h"

EmbedderOptions* embedder_options_new(void)
{
    EmbedderOptions* opts = malloc(sizeof(EmbedderOptions));
    if (!opts) return NULL;

    memset(opts->lua_code, 0, sizeof(opts->lua_code));
    memset(opts->output_file, 0, sizeof(opts->output_file));
    memset(opts->icon_file, 0, sizeof(opts->icon_file));
    memset(opts->version, 0, sizeof(opts->version));
    opts->console_mode = 0;

    return opts;
}

void embedder_options_free(EmbedderOptions* opts)
{
    if (opts) free(opts);
}

int embedder_read_lua_file(const char* filename, char* buffer, size_t buffer_size)
{
    FILE* f;
    size_t bytes_read;

    f = fopen(filename, "rb");
    if (!f) {
        error_print("Failed to open file: %s\n", filename);
        return 1;
    }

    bytes_read = fread(buffer, 1, buffer_size - 1, f);
    if (ferror(f)) {
        error_print("Failed to read file: %s\n", filename);
        fclose(f);
        return 1;
    }

    buffer[bytes_read] = '\0';
    fclose(f);

    return 0;
}

int embedder_generate_source(EmbedderOptions* opts, const char* output_source)
{
    FILE* f;
    char* escaped_code;
    size_t escaped_size;
    const char* template_start = 
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "#include <string.h>\n\n"
        "int main(int argc, char* argv[])\n"
        "{\n"
        "    const char* lua_code = \"";

    const char* template_end =
        "\";\n\n"
        "    printf(\"WinExeLua - Compiled Lua Executable\\n\");\n"
        "    printf(\"Embedded Lua code size: %zu bytes\\n\", strlen(lua_code));\n"
        "    printf(\"To run: compile with Lua runtime library\\n\");\n\n"
        "    return 0;\n"
        "}\n";

    f = fopen(output_source, "w");
    if (!f) {
        error_print("Failed to create output source file: %s\n", output_source);
        return 1;
    }

    escaped_code = string_escape(opts->lua_code, &escaped_size);
    if (!escaped_code) {
        error_print("Failed to escape Lua code\n");
        fclose(f);
        return 1;
    }

    fprintf(f, "%s", template_start);
    fprintf(f, "%s", escaped_code);
    fprintf(f, "%s", template_end);

    free(escaped_code);
    fclose(f);

    return 0;
}

int embedder_compile_source(const char* source_file, const char* output_file,
                           const char* icon_file, int console_mode)
{
    char command[512];
    int result;

#ifdef _WIN32
    const char* compiler = "gcc";
    const char* subsystem = console_mode ? "console" : "windows";
    const char* icon_option = (icon_file && icon_file[0] != '\0') ? 
        " -Wl,--subsystem,windows -Wl,-rc=icon.res" : "";

    snprintf(command, sizeof(command),
        "%s -o %s %s -Wall -Wextra",
        compiler, output_file, source_file);
#else
    snprintf(command, sizeof(command),
        "gcc -o %s %s -Wall -Wextra",
        output_file, source_file);
#endif

    info_print("Executing: %s\n", command);
    result = system(command);

    if (result == 0) {
        info_print("Successfully created executable: %s\n", output_file);
    } else {
        error_print("Compilation failed with exit code: %d\n", result);
    }

    return result;
}

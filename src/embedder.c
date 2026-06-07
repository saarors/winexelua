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

    /* Write C source with Lua interpreter integration */
    fprintf(f,
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "#include <string.h>\n"
        "#include \"lua.h\"\n"
        "#include \"lualib.h\"\n"
        "#include \"lauxlib.h\"\n\n"
        "int main(int argc, char* argv[])\n"
        "{\n"
        "    lua_State* L;\n"
        "    int result;\n"
        "    const char* lua_code = \"");
    
    fprintf(f, "%s", escaped_code);
    
    fprintf(f,
        "\";\n\n"
        "    /* Initialize Lua state */\n"
        "    L = luaL_newstate();\n"
        "    if (!L) {\n"
        "        printf(\"Error: Failed to create Lua state\\n\");\n"
        "        return 1;\n"
        "    }\n\n"
        "    /* Load Lua standard libraries */\n"
        "    luaL_openlibs(L);\n\n"
        "    /* Execute embedded Lua code */\n"
        "    result = luaL_dostring(L, lua_code);\n"
        "    if (result != 0) {\n"
        "        printf(\"Lua Error: %%s\\n\", lua_tostring(L, -1));\n"
        "        lua_close(L);\n"
        "        return 1;\n"
        "    }\n\n"
        "    /* Cleanup */\n"
        "    lua_close(L);\n"
        "    return 0;\n"
        "}\n");

    free(escaped_code);
    fclose(f);

    return 0;
}

int embedder_compile_source(const char* source_file, const char* output_file,
                           const char* icon_file, int console_mode)
{
    char command[2048];
    int result;

#ifdef _WIN32
    const char* compiler = "gcc";
    const char* lua_include = "C:/Users/Saar/scoop/apps/lua/current/include";
    const char* lua_lib = "C:/Users/Saar/scoop/apps/lua/current/lib";
    
    snprintf(command, sizeof(command),
        "%s -o %s %s -I%s -L%s -llua -lm -Wall -Wextra",
        compiler, output_file, source_file, lua_include, lua_lib);
#else
    snprintf(command, sizeof(command),
        "gcc -o %s %s -llua -lm -Wall -Wextra",
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

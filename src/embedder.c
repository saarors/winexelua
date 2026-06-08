#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "embedder.h"
#include "utils.h"

EmbedderOptions* embedder_options_new(void)
{
    EmbedderOptions* opts = malloc(sizeof(EmbedderOptions));
    if (!opts) return NULL;

    memset(opts, 0, sizeof(EmbedderOptions));
    opts->console_mode = 1;

    return opts;
}

void embedder_options_free(EmbedderOptions* opts)
{
    free(opts);
}

int embedder_read_lua_file(const char* filename, char* buffer, size_t buffer_size)
{
    FILE* f = fopen(filename, "rb");
    if (!f) return 1;

    size_t bytes_read = fread(buffer + sizeof(size_t),
                              1,
                              buffer_size - sizeof(size_t),
                              f);

    fclose(f);

    if (bytes_read == 0) return 1;

    *((size_t*)buffer) = bytes_read;
    return 0;
}

int embedder_generate_source(EmbedderOptions* opts, const char* output_source)
{
    FILE* f = fopen(output_source, "w");
    if (!f) return 1;

    const unsigned char* bytecode =
        (const unsigned char*)opts->lua_code + sizeof(size_t);

    size_t bytecode_size =
        *((size_t*)opts->lua_code);

    fprintf(f,
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "#include \"lua.h\"\n"
        "#include \"lauxlib.h\"\n"
        "#include \"lualib.h\"\n"
        "static const unsigned char lua_bytecode[] = {\n");

    for (size_t i = 0; i < bytecode_size; i++) {
        fprintf(f, "0x%02X,", bytecode[i]);
        if (i % 12 == 11) fprintf(f, "\n");
    }

    fprintf(f,
        "};\n"
        "int main(){\n"
        "lua_State* L = luaL_newstate();\n"
        "luaL_openlibs(L);\n"
        "luaL_loadbuffer(L,(const char*)lua_bytecode,sizeof(lua_bytecode),\"embedded\");\n"
        "lua_pcall(L,0,0,0);\n"
        "lua_close(L);\n"
        "return 0;\n"
        "}\n");

    fclose(f);
    return 0;
}

int embedder_compile_source(const char* source_file,
                            const char* output_file,
                            const char* icon_file,
                            int console_mode)
{
    char command[2048];

    snprintf(command, sizeof(command),
        "gcc -o \"%s\" \"%s\" -llua -lm -Wall -Wextra",
        output_file,
        source_file);

    printf("COMMAND: %s\n", command);

    int result = system(command);

    printf("GCC EXIT CODE: %d\n", result);

    FILE* f = fopen(output_file, "rb");
    if (!f) return 1;
    fclose(f);

    return result;
}

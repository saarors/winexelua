cmake_minimum_required(VERSION 3.10)
project(WinExeLua C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find Lua package
find_package(Lua REQUIRED)

# Add source files
set(SOURCES
    src/main.c
    src/compiler.c
    src/embedder.c
    src/utils.c
)

set(HEADERS
    include/compiler.h
    include/embedder.h
    include/utils.h
)

# Create executable
add_executable(winexelua ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(winexelua PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(winexelua PRIVATE ${LUA_INCLUDE_DIR})

# Link Lua library
target_link_libraries(winexelua PRIVATE ${LUA_LIBRARIES})

# Platform-specific settings
if(WIN32)
    target_compile_definitions(winexelua PRIVATE _CRT_SECURE_NO_WARNINGS)
    if(MSVC)
        target_compile_options(winexelua PRIVATE /W4)
    endif(MSVC)
else()
    target_compile_options(winexelua PRIVATE -Wall -Wextra -pedantic)
endif()

# Output directory
set_target_properties(winexelua PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

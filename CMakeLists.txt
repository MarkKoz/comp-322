cmake_minimum_required(VERSION 3.10)
project(cs_322_01 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Needed by Clang tooling (clang-tidy).

add_executable(${PROJECT_NAME}
    src/main.c)
target_include_directories(${PROJECT_NAME} PUBLIC
    include)

# GCC-specific flags cannot be used here.
# clang-tidy reads these flags and will emit warnings if Clang encounters a
# flag that it does not understand/support (e.g. Wlogical-op).
list(APPEND FLAGS_WARN
    -Wall
    -Wcast-align
    -Wconversion
    -Wextra
    -Wfloat-equal
    # -Wlogical-op
    -Wmissing-declarations
    -Wpedantic
    -Wredundant-decls
    -Wshadow
    -Wundef)
list(APPEND FLAGS_DEBUG
    ${FLAGS_WARN}
    -g3
    -ggdb)
list(APPEND FLAGS_RELEASE
    ${FLAGS_WARN}
    -DNDEBUG
    -o2)

target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:${FLAGS_DEBUG}>")
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELEASE>:${FLAGS_RELEASE}>")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME output)

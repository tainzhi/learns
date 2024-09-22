cmake_minimum_required(VERSION 3.14)
project(jpeg_encoder LANGUAGES C)
set(CMAKE_C_STANDARD 99)
# for clangd to export compile_commands.json 
# https://clangd.llvm.org/installation#project-setup
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

aux_source_directory(include INCLUDE)
include_directories(INCLUDE)

add_executable(jpeg_encoder main.c ${INCLUDE})
target_include_directories(jpeg_encoder PRIVATE include)
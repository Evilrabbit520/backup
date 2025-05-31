cmake_minimum_required(VERSION 3.30)

project(backup)

# 设置头文件搜索路径为 include 目录及其子目录
include_directories(include)

# aux_source_directory(. MAIN_DIR)
aux_source_directory(src SRCDIR)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

message("----------- It's OK CppVersion23 -----------")

add_executable(backup ${SRCDIR})

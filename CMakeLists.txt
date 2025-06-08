cmake_minimum_required(VERSION 3.30)

project(backup)

# 查找 OpenSSL 库
find_package(OpenSSL REQUIRED)

# 检查是否找到 OpenSSL
if (OpenSSL_FOUND)
    message(STATUS "Found OpenSSL: ${OPENSSL_VERSION}")
else()
    message(FATAL_ERROR "OpenSSL not found")
endif()


# 设置头文件搜索路径为 include 目录及其子目录
include_directories(include)

# aux_source_directory(. MAIN_DIR)
aux_source_directory(src SRCDIR)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

message("----------- It's OK CppVersion23 -----------")

add_executable(backup ${SRCDIR})
# 包含 OpenSSL 头文件路径
target_include_directories(backup PRIVATE ${OPENSSL_INCLUDE_DIR})

# 链接 OpenSSL 库
target_link_libraries(backup PRIVATE OpenSSL::SSL OpenSSL::Crypto)





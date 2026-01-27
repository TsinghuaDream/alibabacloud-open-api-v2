
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(alibabacloud_open_api_v2 VERSION "1.0.0")

# <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 选项与全局设置 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> #

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# C++ 标准设置：兼容 C++11 及以上，支持外部 -DCMAKE_CXX_STANDARD=20
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 静态库必须开启，否则无法被动态库链接
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 目标提前定义 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> #

set(SOURCE_FILES
    src/Client.cpp
    src/Utils.cpp
    src/POP.cpp
)

# 创建 Target
add_library(${PROJECT_NAME} ${SOURCE_FILES})

# <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 依赖搜寻 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> #

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

# 此时 Target 已存在，进入子目录不会报错
add_subdirectory(external)

# <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 路径与链接设置 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> #

# 自动搜集头文件用于安装
file(GLOB_RECURSE PUBLIC_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        CURL::libcurl
        OpenSSL::SSL
        OpenSSL::Crypto
        darabonba_core
        alibabacloud_credential
)

# Windows 兼容性
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC Crypt32 Rpcrt4 Ws2_32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# 设置 Target 属性
set_target_properties(${PROJECT_NAME} PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION}
    OUTPUT_NAME "${PROJECT_NAME}"
    DEBUG_POSTFIX "_d"
    PUBLIC_HEADER "${PUBLIC_HEADERS}"
    MACOSX_RPATH ON
)

# <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 安装与 Config 生成 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> #

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# 安装路径定义
set(INSTALL_CMAKEDIR ${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME})

# 1. 安装库文件和头文件
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud
)

# 2. 生成并安装导出文件 (Targets.cmake)
install(EXPORT ${PROJECT_NAME}Targets
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${INSTALL_CMAKEDIR}
)

# 3. 生成 ConfigVersion 文件
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# 4. 生成主 Config 文件 (使用你指定的 .in 文件)
# 请确保你的文件确实在 cmake/ 目录下，如果是在根目录，请去掉前面的 cmake/
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${INSTALL_CMAKEDIR}
)

# 5. 安装生成的配置文件
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION ${INSTALL_CMAKEDIR}
)

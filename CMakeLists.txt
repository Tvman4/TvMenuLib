cmake_minimum_required(VERSION 3.18)
project(TvMenu LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT ANDROID)
    message(FATAL_ERROR "This library is meant to be built with the Android NDK toolchain.")
endif()

set(BNM_DIR ${CMAKE_SOURCE_DIR}/third_party/BNM-Android)
set(DOBBY_DIR ${CMAKE_SOURCE_DIR}/third_party/Dobby)

# Overlay our Unity 2021.3 + Dobby settings on top of BNM
configure_file(
    ${CMAKE_SOURCE_DIR}/include/BNM/UserSettings/GlobalSettings.hpp
    ${BNM_DIR}/include/BNM/UserSettings/GlobalSettings.hpp
    COPYONLY
)

set(BNM_LINK_LOG ON)
add_subdirectory(${BNM_DIR} BNM)

add_library(dobby STATIC IMPORTED)
set_target_properties(dobby PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/third_party/libdobby.a
    INTERFACE_INCLUDE_DIRECTORIES ${DOBBY_DIR}/include
)

add_library(TvMenu SHARED
    src/main.cpp
    src/Unity.cpp
    src/Input.cpp
    src/Menu.cpp
    src/Mods.cpp
)

target_include_directories(TvMenu PRIVATE
    src
    include
    ${BNM_DIR}/include
    ${BNM_DIR}/external/include
    ${DOBBY_DIR}/include
)

target_compile_options(TvMenu PRIVATE
    -fvisibility=hidden
    -O2
    -ffunction-sections -fdata-sections
)

target_link_options(TvMenu PRIVATE
    -Wl,--gc-sections
    -Wl,--exclude-libs,ALL
)

target_link_libraries(TvMenu
    BNM
    dobby
    log
    android
    EGL
    GLESv3
)

set_target_properties(TvMenu PROPERTIES
    OUTPUT_NAME "TvMenu"
    PREFIX "lib"
    SUFFIX ".so"
)

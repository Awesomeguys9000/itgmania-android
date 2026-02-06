# CMakeLists.txt for ITGMania Android Native Build
# This integrates with the main ITGMania CMake build system

cmake_minimum_required(VERSION 3.22)
project(itgmania-android)

# Android NDK builds use this file as the entry point
# It delegates to the main ITGMania CMake configuration

# Paths relative to this CMakeLists.txt location (android/app/src/main/cpp)
get_filename_component(ITGMANIA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE)
set(SDL2_ROOT "${ITGMANIA_ROOT}/extern/SDL")


# Set Android-specific options
set(ANDROID TRUE)
set(WITH_SDL2 ON CACHE BOOL "Use SDL2 for Android")
set(WITH_GTK3 OFF CACHE BOOL "Disable GTK3 on Android")
set(WITH_X11 OFF CACHE BOOL "Disable X11 on Android")
set(WITH_PULSEAUDIO OFF CACHE BOOL "Disable PulseAudio on Android")
set(WITH_ALSA OFF CACHE BOOL "Disable ALSA on Android")
set(WITH_JACK OFF CACHE BOOL "Disable JACK on Android")
set(WITH_OSS OFF CACHE BOOL "Disable OSS on Android")

# Build SDL2 first
add_subdirectory(${SDL2_ROOT} SDL2_build)

# Get all ITGMania source files
# For now, we'll create a minimal stub that links against SDL2
# Full integration will be added incrementally

# Include paths
include_directories(
    ${SDL2_ROOT}/include
    ${ITGMANIA_ROOT}/src
)

# Minimal Android entry point
add_library(itgmania SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/android_main.cpp
)

# Link against SDL2
target_link_libraries(itgmania
    SDL2
    SDL2main
    android
    log
    GLESv2
    EGL
)

# Set output name
set_target_properties(itgmania PROPERTIES
    OUTPUT_NAME "itgmania"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)

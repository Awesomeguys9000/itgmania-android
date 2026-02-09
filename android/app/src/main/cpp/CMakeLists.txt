# CMakeLists.txt for ITGMania Android Native Build

cmake_minimum_required(VERSION 3.22)
project(itgmania-android)

get_filename_component(ITGMANIA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE)
set(SDL2_ROOT "${ITGMANIA_ROOT}/extern/SDL")

# Set Android options
set(ANDROID TRUE)
set(WITH_SDL2 ON CACHE BOOL "Use SDL2 for Android")
set(WITH_GTK3 OFF CACHE BOOL "Disable GTK3 on Android")
set(WITH_X11 OFF CACHE BOOL "Disable X11 on Android")
set(WITH_PULSEAUDIO OFF CACHE BOOL "Disable PulseAudio on Android")
set(WITH_ALSA OFF CACHE BOOL "Disable ALSA on Android")
set(WITH_JACK OFF CACHE BOOL "Disable JACK on Android")
set(WITH_OSS OFF CACHE BOOL "Disable OSS on Android")
set(WITH_MINIMAID OFF CACHE BOOL "Disable Minimaid on Android")
set(WITH_FFMPEG OFF CACHE BOOL "Disable FFmpeg on Android")

# Build SDL2
add_subdirectory(${SDL2_ROOT} SDL2_build)

# Include StepmaniaCore
set(SM_ROOT_DIR ${ITGMANIA_ROOT})
include(${ITGMANIA_ROOT}/StepmaniaCore.cmake)

# Debug: Print the generated source dir
message(STATUS "SM_GENERATED_SRC_DIR: ${SM_GENERATED_SRC_DIR}")

# Build externals
add_subdirectory(${ITGMANIA_ROOT}/extern extern_build)

# Define sources
set(CMAKE_MODULE_PATH ${ITGMANIA_ROOT}/src)
include(${ITGMANIA_ROOT}/src/CMakeData-arch.cmake)
include(${ITGMANIA_ROOT}/src/CMakeData-rage.cmake)
include(${ITGMANIA_ROOT}/src/CMakeData-os.cmake)
include(${ITGMANIA_ROOT}/src/CMakeData-actor.cmake)
include(${ITGMANIA_ROOT}/src/CMakeData-screen.cmake)
include(${ITGMANIA_ROOT}/src/CMakeData-data.cmake)
include(${ITGMANIA_ROOT}/src/CMakeData-file-types.cmake)
include(${ITGMANIA_ROOT}/src/CMakeData-globals.cmake)
include(${ITGMANIA_ROOT}/src/CMakeData-singletons.cmake)

list(APPEND SMDATA_ALL_FILES_SRC
            ${SMDATA_GLOBAL_FILES_SRC}
            ${SMDATA_GLOBAL_SINGLETON_SRC}
            ${SMDATA_ALL_ACTORS_SRC}
            ${SMDATA_ALL_ARCH_SRC}
            ${SMDATA_ALL_DATA_SRC}
            ${SMDATA_ALL_RAGE_SRC}
            ${SMDATA_ALL_SCREENS_SRC}
            ${SMDATA_OS_SRC}
            ${SMDATA_FILE_TYPES_SRC})

# Prepend source directory to all files
set(SM_SRC_DIR "${ITGMANIA_ROOT}/src")
set(SMDATA_ALL_FILES_SRC_ABS "")
foreach(FILE ${SMDATA_ALL_FILES_SRC})
    if(IS_ABSOLUTE ${FILE})
        list(APPEND SMDATA_ALL_FILES_SRC_ABS ${FILE})
    else()
        list(APPEND SMDATA_ALL_FILES_SRC_ABS "${SM_SRC_DIR}/${FILE}")
    endif()
endforeach()

# Add Android entry point
list(APPEND SMDATA_ALL_FILES_SRC_ABS "${CMAKE_CURRENT_SOURCE_DIR}/android_main.cpp")

# Create library
add_library(itgmania SHARED ${SMDATA_ALL_FILES_SRC_ABS})

# Set C++17 Standard
set_target_properties(itgmania PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

# Use PRIVATE to ensure it's on the include path for this target's sources
target_include_directories(itgmania PRIVATE
    ${SDL2_ROOT}/include
    ${ITGMANIA_ROOT}/src
    ${ITGMANIA_ROOT}/extern/ffmpeg/include
    ${SM_GENERATED_SRC_DIR}
)

# Link libraries
target_link_libraries(itgmania
    SDL2
    SDL2main
    android
    log
    GLESv2
    EGL
    # External libs
    lua-5.1
    miniz
    mad
    pcre
    tomcrypt
    tommath
    libjpeg-turbo
    zlib
    png
    jsoncpp
    vorbisfile
    vorbis
    ogg
    ixwebsocket
)

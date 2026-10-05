cmake_minimum_required(VERSION 3.20)
project(PSPRecomp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(PSPRECOMP_BUILD_TESTS "Build reusable PSPRecomp tests" ON)
option(PSPRECOMP_BUILD_PROFILE_TESTS "Build tests supplied by the selected profile" ON)
set(PSPRECOMP_PROFILE "" CACHE STRING "Profile to build from profiles/<name>; empty builds only the framework")

set(PSPRECOMP_GENERATED_OPT_LEVEL "2" CACHE STRING
    "Optimization level for generated AOT units (0, 1, 2 or 3)")
set_property(CACHE PSPRECOMP_GENERATED_OPT_LEVEL PROPERTY STRINGS 0 1 2 3)
if(NOT PSPRECOMP_GENERATED_OPT_LEVEL MATCHES "^[0-3]$")
    message(FATAL_ERROR "PSPRECOMP_GENERATED_OPT_LEVEL must be 0, 1, 2 or 3")
endif()

option(PSPRECOMP_LTO "Enable link-time/whole-program optimization" OFF)
option(PSPRECOMP_NATIVE_AVX2 "Use AVX2 code generation for optimized MSVC builds" OFF)
option(PSPRECOMP_HOST_AVX "Use AVX for host-side scalar floor/ceil hot paths on MSVC" ON)
option(PSPRECOMP_AOT_ASSUME_NO_WRITE_WATCH
    "Remove per-store write-watch branch from production AOT fast memory" OFF)
option(PSPRECOMP_AOT_PRODUCTION_FASTPATHS
    "Compile out AOT chain diagnostic/counter branches for production builds" OFF)
set(PSPRECOMP_MSVC_CGTHREADS "0" CACHE STRING
    "MSVC LTCG code-generation threads (0 = compiler default)")
if(NOT PSPRECOMP_MSVC_CGTHREADS MATCHES "^[0-9]+$")
    message(FATAL_ERROR "PSPRECOMP_MSVC_CGTHREADS must be a non-negative integer")
endif()

# Bare /MP lets cl.exe spawn one front-end per logical core, and MSBuild /m
# multiplies that by the number of projects it builds in parallel.  On the VCS
# profile each generated AOT unit costs well over a gigabyte at /Ox /Ob3, so the
# unbounded product exhausts physical memory.  Cap the per-cl.exe degree here and
# let the build scripts pick a memory-aware value.
set(PSPRECOMP_MSVC_MP_JOBS "0" CACHE STRING
    "Max cl.exe front-ends (0 = logical-core default, 1 = disable /MP)")
if(NOT PSPRECOMP_MSVC_MP_JOBS MATCHES "^[0-9]+$")
    message(FATAL_ERROR "PSPRECOMP_MSVC_MP_JOBS must be a non-negative integer")
endif()
if(PSPRECOMP_MSVC_MP_JOBS STREQUAL "0")
    set(PSPRECOMP_MSVC_MP_FLAG "/MP")
elseif(PSPRECOMP_MSVC_MP_JOBS STREQUAL "1")
    # Omitting /MP is materially different from /MP1: the latter still creates
    # and communicates with a child compiler process and can fail with D8040.
    set(PSPRECOMP_MSVC_MP_FLAG "")
else()
    set(PSPRECOMP_MSVC_MP_FLAG "/MP${PSPRECOMP_MSVC_MP_JOBS}")
endif()

if(PSPRECOMP_AOT_ASSUME_NO_WRITE_WATCH)
    add_compile_definitions(PSPRECOMP_AOT_ASSUME_NO_WRITE_WATCH=1)
endif()
if(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    add_compile_definitions(PSPRECOMP_AOT_PRODUCTION_FASTPATHS=1)
endif()
if(MSVC AND PSPRECOMP_NATIVE_AVX2)
    add_compile_options($<$<CONFIG:Release>:/arch:AVX2>
                        $<$<CONFIG:RelWithDebInfo>:/arch:AVX2>)
endif()

if(PSPRECOMP_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT PSPRECOMP_LTO_SUPPORTED OUTPUT PSPRECOMP_LTO_REASON)
    if(PSPRECOMP_LTO_SUPPORTED)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL ON)
    else()
        message(WARNING "Whole-program optimization unavailable: ${PSPRECOMP_LTO_REASON}")
    endif()
endif()

add_library(psprecomp_core STATIC
    src/decoder.cpp
    src/deflate.cpp
    src/elf32.cpp
    src/guest_memory.cpp
    src/nid_registry.cpp
    src/program_analysis.cpp
    src/runtime.cpp
    src/sha256.cpp
)
target_include_directories(psprecomp_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(psprecomp_core PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
if(MSVC)
    target_compile_options(psprecomp_core PRIVATE /W4 /permissive- /EHsc ${PSPRECOMP_MSVC_MP_FLAG}
        $<$<CONFIG:Release>:/Ob3> $<$<CONFIG:RelWithDebInfo>:/Ob3>)
else()
    target_compile_options(psprecomp_core PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

# Generic PSP HLE shared by title profiles (kernel: threads, scheduler, sync
# objects, partitions, file I/O, UMD, rtc, display, GE, audio, controller). Contains no game addresses.
add_library(psprecomp_hle STATIC
    src/hle/audio.cpp
    src/hle/ctrl.cpp
    src/hle/display.cpp
    src/hle/ge.cpp
    src/hle/ge_renderer.cpp
    src/hle/io.cpp
    src/hle/kernel.cpp
    src/hle/system.cpp
    src/hle/utility.cpp
)
target_link_libraries(psprecomp_hle PUBLIC psprecomp_core)
target_compile_definitions(psprecomp_hle PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
if(MSVC)
    target_compile_options(psprecomp_hle PRIVATE /W4 /permissive- /EHsc ${PSPRECOMP_MSVC_MP_FLAG})
endif()

add_executable(psp_analyze tools/analyzer_main.cpp)
target_link_libraries(psp_analyze PRIVATE psprecomp_core)

add_executable(psp_recomp tools/codegen_main.cpp)
target_link_libraries(psp_recomp PRIVATE psprecomp_core)

add_executable(dump_function tools/dump_function.cpp)
target_link_libraries(dump_function PRIVATE psprecomp_core)

function(psprecomp_enable_host_avx target)
    # /arch:AVX appears after the directory-wide /arch:AVX2 option in MSVC's
    # command line and therefore silently downgrades the whole target. AVX2
    # already includes the AVX scalar instructions this compatibility switch
    # was introduced for, so do not emit the weaker override in an AVX2 build.
    if(PSPRECOMP_HOST_AVX AND MSVC AND NOT PSPRECOMP_NATIVE_AVX2)
        target_compile_options(${target} PRIVATE /arch:AVX)
    endif()
endfunction()

# The GE software rasterizer previously built inside the VCS host target with
# its AVX and /Ob3 settings; keep the same code generation for it here.
psprecomp_enable_host_avx(psprecomp_hle)
if(MSVC)
    target_compile_options(psprecomp_hle PRIVATE $<$<CONFIG:Release>:/Ob3> $<$<CONFIG:RelWithDebInfo>:/Ob3>)
endif()

# GPU-backend entry points for profiles without a GPU backend: the software
# rasterizer stays authoritative for every draw.
add_library(psprecomp_hle_gpu_null STATIC src/hle/ge_gpu_null.cpp)
target_link_libraries(psprecomp_hle_gpu_null PUBLIC psprecomp_hle)

# Vulkan GE backend: replaces psprecomp_hle_gpu_null for profiles that render
# on the GPU (src/hle/ge_gpu_vulkan.cpp). Optional: skipped without Vulkan.
find_package(Vulkan QUIET)
if(Vulkan_FOUND)
    add_library(psprecomp_hle_gpu_vulkan STATIC src/hle/ge_gpu_vulkan.cpp)
    target_link_libraries(psprecomp_hle_gpu_vulkan PUBLIC psprecomp_hle Vulkan::Vulkan)
    psprecomp_enable_host_avx(psprecomp_hle_gpu_vulkan)
endif()

# sceAtrac3plus. Its sources compile inside the consuming target, which supplies
# the FFmpeg headers and libraries (VCS vendors them; CTW uses the system ones).
add_library(psprecomp_hle_atrac INTERFACE)
target_sources(psprecomp_hle_atrac INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hle/atrac.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hle/audio_stream_decoder.cpp")
target_link_libraries(psprecomp_hle_atrac INTERFACE psprecomp_hle)

if(PSPRECOMP_BUILD_TESTS)
    enable_testing()
    add_executable(psprecomp_tests tests/test_main.cpp)
    target_link_libraries(psprecomp_tests PRIVATE psprecomp_core)
    add_dependencies(psprecomp_tests psp_recomp)
    target_compile_definitions(psprecomp_tests PRIVATE
        PSPRECOMP_CODEGEN_PATH="$<TARGET_FILE:psp_recomp>")
    add_test(NAME psprecomp_tests COMMAND psprecomp_tests)
endif()

if(NOT PSPRECOMP_PROFILE STREQUAL "")
    set(PSPRECOMP_PROFILE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/profiles/${PSPRECOMP_PROFILE}")
    if(NOT EXISTS "${PSPRECOMP_PROFILE_DIR}/CMakeLists.txt")
        message(FATAL_ERROR "Unknown PSPRecomp profile: ${PSPRECOMP_PROFILE}")
    endif()
    add_subdirectory("${PSPRECOMP_PROFILE_DIR}" "profiles/${PSPRECOMP_PROFILE}")
endif()

set(CTW_PROFILE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Generated AOT corpus produced by the root psp_recomp tool (see README.md).
file(GLOB CTW_GENERATED CONFIGURE_DEPENDS "${CTW_PROFILE_DIR}/generated/*.cpp")
if(NOT CTW_GENERATED)
    message(FATAL_ERROR "CTW generated corpus is missing; run psp_recomp --auto first (see profiles/ctw/README.md)")
endif()

if(MSVC)
    if(PSPRECOMP_GENERATED_OPT_LEVEL STREQUAL "0")
        set(CTW_GENERATED_MSVC_OPT "/Od")
    elseif(PSPRECOMP_GENERATED_OPT_LEVEL STREQUAL "3")
        set(CTW_GENERATED_MSVC_OPT "/Ox")
    else()
        set(CTW_GENERATED_MSVC_OPT "/O${PSPRECOMP_GENERATED_OPT_LEVEL}")
    endif()
    set_source_files_properties(${CTW_GENERATED} PROPERTIES COMPILE_OPTIONS
        "${CTW_GENERATED_MSVC_OPT};/Ob0;/bigobj;${PSPRECOMP_MSVC_MP_FLAG}")
else()
    set_source_files_properties(${CTW_GENERATED} PROPERTIES COMPILE_OPTIONS
        "-O${PSPRECOMP_GENERATED_OPT_LEVEL};-g0")
endif()

add_library(ctw_generated STATIC ${CTW_GENERATED})
target_include_directories(ctw_generated PUBLIC "${CTW_PROFILE_DIR}/generated")
target_link_libraries(ctw_generated PUBLIC psprecomp_core)

# Loads the decrypted ELF and registers the full generated corpus. This is the
# link/registration smoke test, independent of the host.
add_executable(ctw_generated_check tools/ctw_generated_check.cpp)
target_link_libraries(ctw_generated_check PRIVATE ctw_generated)
set_target_properties(ctw_generated_check PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

# Recompiled code overlays (tools/build_overlays.py). Each overlay's units live
# in their own directory and namespace; the table maps fingerprints to them.
file(GLOB CTW_OVERLAY_SOURCES CONFIGURE_DEPENDS "${CTW_PROFILE_DIR}/generated_overlays/*/*.cpp")
set(CTW_OVERLAY_TABLE "${CTW_PROFILE_DIR}/generated_overlays/overlay_table.cpp")
if(NOT EXISTS "${CTW_OVERLAY_TABLE}")
    message(FATAL_ERROR "CTW overlays are not generated; run profiles/ctw/tools/build_overlays.py")
endif()
if(MSVC)
    set_source_files_properties(${CTW_OVERLAY_SOURCES} PROPERTIES COMPILE_OPTIONS
        "${CTW_GENERATED_MSVC_OPT};/Ob0;/bigobj;${PSPRECOMP_MSVC_MP_FLAG}")
else()
    set_source_files_properties(${CTW_OVERLAY_SOURCES} PROPERTIES COMPILE_OPTIONS
        "-O${PSPRECOMP_GENERATED_OPT_LEVEL};-g0")
endif()
add_library(ctw_overlays STATIC ${CTW_OVERLAY_SOURCES} ${CTW_OVERLAY_TABLE} host/ctw_overlays.cpp)
target_include_directories(ctw_overlays PUBLIC "${CTW_PROFILE_DIR}/host")
target_link_libraries(ctw_overlays PUBLIC psprecomp_core)

# The game: the generated corpus on the shared HLE library with the CTW host
# (SDL3 frontend, overlay manager, renderer selection).
add_executable(ctw_boot host/main.cpp host/ctw_frontend.cpp)
# SDL3 window, gamepad and audio frontend; without it ctw_boot runs headless.
find_package(SDL3 CONFIG QUIET)
if(SDL3_FOUND)
    target_compile_definitions(ctw_boot PRIVATE CTW_HAVE_SDL3=1)
    target_link_libraries(ctw_boot PRIVATE SDL3::SDL3)
    message(STATUS "CTW: SDL3 ${SDL3_VERSION} frontend enabled")
else()
    message(STATUS "CTW: SDL3 not found; ctw_boot is headless only")
endif()
# FFmpeg decodes the ATRAC3+ music (psprecomp_hle_atrac). SteamOS ships it.
find_package(PkgConfig REQUIRED)
pkg_check_modules(CTW_FFMPEG REQUIRED IMPORTED_TARGET libavcodec libavformat libavutil libswresample)
target_include_directories(ctw_boot PRIVATE "${CTW_PROFILE_DIR}/host")
target_link_libraries(ctw_boot PRIVATE ctw_generated ctw_overlays psprecomp_hle_atrac psprecomp_hle
    PkgConfig::CTW_FFMPEG)
# Vulkan GE backend (default renderer, CTW_RENDERER=software opts out);
# without the Vulkan SDK the software rasterizer is the only renderer.
if(TARGET psprecomp_hle_gpu_vulkan)
    target_compile_definitions(ctw_boot PRIVATE CTW_HAVE_VULKAN=1)
    target_link_libraries(ctw_boot PRIVATE psprecomp_hle_gpu_vulkan)
    message(STATUS "CTW: Vulkan renderer enabled")
else()
    target_link_libraries(ctw_boot PRIVATE psprecomp_hle_gpu_null)
    message(STATUS "CTW: Vulkan not found; software renderer only")
endif()
set_target_properties(ctw_boot PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

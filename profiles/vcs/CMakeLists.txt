set(VCS_PROFILE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Keep the enormous generated AOT corpus out of MSVC whole-program/LTCG IR by
# default.  Stage 45.8/45.9 cross-unit state makes whole-program analysis of all
# 234 generated units extremely memory hungry on Windows.  The AOT units are
# still fully optimized per translation unit; host/runtime code may still use
# PSPRECOMP_LTO.
option(PSPRECOMP_VCS_AOT_LTO
    "Include generated VCS AOT units in MSVC whole-program optimization" OFF)

option(PSPRECOMP_PROFILE_GUIDED_AOT
    "Optimize the measured VCS hot AOT units above the base generated level" ON)
set(PSPRECOMP_HOT_GENERATED_OPT_LEVEL "2" CACHE STRING
    "Optimization level for profile-guided VCS AOT units (0, 1, 2 or 3)")
set_property(CACHE PSPRECOMP_HOT_GENERATED_OPT_LEVEL PROPERTY STRINGS 0 1 2 3)
if(NOT PSPRECOMP_HOT_GENERATED_OPT_LEVEL MATCHES "^[0-3]$")
    message(FATAL_ERROR "PSPRECOMP_HOT_GENERATED_OPT_LEVEL must be 0, 1, 2 or 3")
endif()

# Full /Ob3 is valuable at runtime but several large, cold units still make
# MSVC spend minutes and close to a gigabyte per compiler process. Keep those
# units buildable, then override this level only for the measured hot corpus.
set(PSPRECOMP_GENERATED_INLINE_LEVEL "0" CACHE STRING
    "MSVC /Ob level for generated VCS AOT units (0, 1, 2 or 3)")
set_property(CACHE PSPRECOMP_GENERATED_INLINE_LEVEL PROPERTY STRINGS 0 1 2 3)
if(NOT PSPRECOMP_GENERATED_INLINE_LEVEL MATCHES "^[0-3]$")
    message(FATAL_ERROR "PSPRECOMP_GENERATED_INLINE_LEVEL must be 0, 1, 2 or 3")
endif()
set(PSPRECOMP_HOT_GENERATED_INLINE_LEVEL "3" CACHE STRING
    "MSVC /Ob level for measured hot VCS AOT units (0, 1, 2 or 3)")
set_property(CACHE PSPRECOMP_HOT_GENERATED_INLINE_LEVEL PROPERTY STRINGS 0 1 2 3)
if(NOT PSPRECOMP_HOT_GENERATED_INLINE_LEVEL MATCHES "^[0-3]$")
    message(FATAL_ERROR "PSPRECOMP_HOT_GENERATED_INLINE_LEVEL must be 0, 1, 2 or 3")
endif()

file(GLOB VCS_GENERATED CONFIGURE_DEPENDS "${VCS_PROFILE_DIR}/generated/*.cpp")
if(MSVC)
    if(PSPRECOMP_GENERATED_OPT_LEVEL STREQUAL "0")
        set(VCS_GENERATED_MSVC_OPT "/Od")
    elseif(PSPRECOMP_GENERATED_OPT_LEVEL STREQUAL "3")
        set(VCS_GENERATED_MSVC_OPT "/Ox")
    else()
        set(VCS_GENERATED_MSVC_OPT "/O${PSPRECOMP_GENERATED_OPT_LEVEL}")
    endif()
    set(VCS_GENERATED_MSVC_OPTIONS
        "${VCS_GENERATED_MSVC_OPT};/Ob${PSPRECOMP_GENERATED_INLINE_LEVEL};/bigobj;${PSPRECOMP_MSVC_MP_FLAG}")
    if(PSPRECOMP_LTO AND NOT PSPRECOMP_VCS_AOT_LTO)
        # CMAKE_INTERPROCEDURAL_OPTIMIZATION adds /GL at target scope.  /GL- on
        # these sources overrides it so the linker receives native .obj code
        # instead of 234 giant LTCG IR modules.
        string(APPEND VCS_GENERATED_MSVC_OPTIONS ";/GL-")
    endif()
    set_source_files_properties(${VCS_GENERATED} PROPERTIES COMPILE_OPTIONS
        "${VCS_GENERATED_MSVC_OPTIONS}")
else()
    set_source_files_properties(${VCS_GENERATED} PROPERTIES COMPILE_OPTIONS
        "-O${PSPRECOMP_GENERATED_OPT_LEVEL};-g0")
endif()

if(PSPRECOMP_PROFILE_GUIDED_AOT)
    # This unit is hot in the current capture and finishes /Ob3 without
    # triggering MSVC's pathological optimizer growth. It also contains the
    # validated native collision leaf at guest PC 0x088B1554. Other measured
    # units remain /Ob0 until their oversized helpers are split/noinline.
    set(VCS_HOT_UNIT_IDS
        0043)
    set(VCS_HOT_SOURCES)
    foreach(VCS_UNIT_ID IN LISTS VCS_HOT_UNIT_IDS)
        set(VCS_UNIT_SOURCE "${VCS_PROFILE_DIR}/generated/generated_unit_${VCS_UNIT_ID}.cpp")
        if(NOT EXISTS "${VCS_UNIT_SOURCE}")
            message(FATAL_ERROR "VCS profile-guided AOT unit is missing: ${VCS_UNIT_SOURCE}")
        endif()
        list(APPEND VCS_HOT_SOURCES "${VCS_UNIT_SOURCE}")
    endforeach()
    if(MSVC)
        if(PSPRECOMP_HOT_GENERATED_OPT_LEVEL STREQUAL "0")
            set(VCS_HOT_MSVC_OPT "/Od")
        elseif(PSPRECOMP_HOT_GENERATED_OPT_LEVEL STREQUAL "3")
            set(VCS_HOT_MSVC_OPT "/Ox")
        else()
            set(VCS_HOT_MSVC_OPT "/O${PSPRECOMP_HOT_GENERATED_OPT_LEVEL}")
        endif()
        set(VCS_HOT_MSVC_OPTIONS
            "${VCS_HOT_MSVC_OPT};/Ob${PSPRECOMP_HOT_GENERATED_INLINE_LEVEL};/bigobj;${PSPRECOMP_MSVC_MP_FLAG}")
        if(PSPRECOMP_LTO AND NOT PSPRECOMP_VCS_AOT_LTO)
            string(APPEND VCS_HOT_MSVC_OPTIONS ";/GL-")
        endif()
        set_source_files_properties(${VCS_HOT_SOURCES} PROPERTIES COMPILE_OPTIONS
            "${VCS_HOT_MSVC_OPTIONS}")
    else()
        set_source_files_properties(${VCS_HOT_SOURCES} PROPERTIES COMPILE_OPTIONS
            "-O${PSPRECOMP_HOT_GENERATED_OPT_LEVEL};-g0")
    endif()
endif()

if(MSVC AND PSPRECOMP_NATIVE_AVX2)
    # MSVC 19.44's AVX2 optimizer crashes (C1001/c2.dll) on this translation
    # unit, while the identical /Ox /Ob0 compile completes in ~3 s with AVX.
    # Keep AVX2 for the host and the other AOT units; append the weaker ISA only
    # to the known compiler-bug trigger (the last /arch switch wins in cl.exe).
    set(PSPRECOMP_VCS_MSVC_AVX_FALLBACK_UNITS "0018;0022;0035;0089;0091;0117;0164" CACHE STRING
        "Generated VCS units forced to AVX because MSVC 19.44 crashes with AVX2")
    set(VCS_MSVC_AVX_FALLBACK_SOURCES)
    foreach(VCS_UNIT_ID IN LISTS PSPRECOMP_VCS_MSVC_AVX_FALLBACK_UNITS)
        set(VCS_UNIT_SOURCE "${VCS_PROFILE_DIR}/generated/generated_unit_${VCS_UNIT_ID}.cpp")
        if(NOT EXISTS "${VCS_UNIT_SOURCE}")
            message(FATAL_ERROR "VCS AVX fallback unit is missing: ${VCS_UNIT_SOURCE}")
        endif()
        list(APPEND VCS_MSVC_AVX_FALLBACK_SOURCES "${VCS_UNIT_SOURCE}")
    endforeach()
    set_property(SOURCE ${VCS_MSVC_AVX_FALLBACK_SOURCES} APPEND PROPERTY
        COMPILE_OPTIONS "/arch:AVX")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set_source_files_properties("${VCS_PROFILE_DIR}/host/vcs_profile.cpp" PROPERTIES COMPILE_OPTIONS "-O2")
endif()

set(VCS_GPU_BACKEND_SOURCE host/ge_gpu_backend_dx12.cpp)
set(VCS_HDR_POST_SOURCE host/vcs_hdr_post_dx12_stub.cpp)
set(VCS_HOST_SOURCES
    host/vcs_bootstrap_paths.cpp
    host/vcs_profile.cpp
    host/vcs_native_fast_paths.cpp
    host/framebuffer_capture.cpp
    host/display_window.cpp
    host/audio_output.cpp
    host/vcs_config.cpp
    host/vcs_camera_input.cpp
    host/vcs_vehicle_input.cpp
    host/vcs_fps_overlay.cpp
    host/vcs_media_decoder.cpp
    host/vcs_project2dfx.cpp
    host/vcs_project2dfx_lights.cpp
    host/vcs_draw_distance_patch.cpp
    host/vcs_runtime_log.cpp
    ${VCS_HDR_POST_SOURCE}
    host/vcs_ge_integration.cpp
    ${VCS_GPU_BACKEND_SOURCE}
    host/dx12_presenter.cpp
)

function(vcs_target_common target)
    target_include_directories(${target} PRIVATE
        "${VCS_PROFILE_DIR}/host"
        "${VCS_PROFILE_DIR}/generated"
        "${VCS_PROFILE_DIR}/third_party/ffmpeg/include")
    target_link_libraries(${target} PRIVATE psprecomp_hle_atrac psprecomp_hle psprecomp_core ${CMAKE_DL_LIBS})
    if(WIN32)
        foreach(VCS_FFMPEG_LIB avcodec avformat avutil swresample swscale)
            target_link_libraries(${target} PRIVATE
                "${VCS_PROFILE_DIR}/third_party/ffmpeg/lib/${VCS_FFMPEG_LIB}.lib")
        endforeach()
        target_link_libraries(${target} PRIVATE winmm d3d12 dxgi d3dcompiler dxguid)
    else()
        # The profile vendors FFmpeg 7 headers. Some validation hosts install only
        # the versioned runtime sonames (without development symlinks), so accept
        # the matching FFmpeg 7 ABI names as fallbacks.
        set(VCS_FFMPEG_avcodec_NAMES avcodec libavcodec.so.61)
        set(VCS_FFMPEG_avformat_NAMES avformat libavformat.so.61)
        set(VCS_FFMPEG_avutil_NAMES avutil libavutil.so.59)
        set(VCS_FFMPEG_swresample_NAMES swresample libswresample.so.5)
        set(VCS_FFMPEG_swscale_NAMES swscale libswscale.so.8)
        foreach(VCS_FFMPEG_LIB avcodec avformat avutil swresample swscale)
            find_library(VCS_SYSTEM_${VCS_FFMPEG_LIB}
                NAMES ${VCS_FFMPEG_${VCS_FFMPEG_LIB}_NAMES}
                REQUIRED)
            target_link_libraries(${target} PRIVATE ${VCS_SYSTEM_${VCS_FFMPEG_LIB}})
        endforeach()
    endif()
    psprecomp_enable_host_avx(${target})
endfunction()


function(vcs_set_runtime_output target)
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
endfunction()

set(VCS_APP_RESOURCES)
if(WIN32)
    list(APPEND VCS_APP_RESOURCES resources/VCSNative.rc)
endif()

add_executable(VCSNative
    ${VCS_APP_RESOURCES}
    host/main.cpp
    ${VCS_HOST_SOURCES}
    ${VCS_GENERATED}
)
vcs_target_common(VCSNative)
vcs_set_runtime_output(VCSNative)
if(MSVC)
    # /Ob3 must stay off the target: target_compile_options lands in the vcxproj
    # AdditionalOptions, which cl.exe sees *after* the per-source
    # InlineFunctionExpansion, so a target-wide /Ob3 silently overrides the
    # /Ob${PSPRECOMP_GENERATED_INLINE_LEVEL} set on the generated corpus above
    # (MSVC reports this as "D9025: overriding '/Ob0' with '/Ob3'").  Apply it to
    # the host sources only.
    target_compile_options(VCSNative PRIVATE ${PSPRECOMP_MSVC_MP_FLAG})
    set_source_files_properties(host/main.cpp ${VCS_HOST_SOURCES}
        DIRECTORY "${VCS_PROFILE_DIR}" PROPERTIES COMPILE_OPTIONS
        "$<$<CONFIG:Release>:/Ob3>;$<$<CONFIG:RelWithDebInfo>:/Ob3>")
    target_link_options(VCSNative PRIVATE /STACK:67108864)
    if(PSPRECOMP_LTO)
        # Make the otherwise silent LTCG phase visible in the console.
        target_link_options(VCSNative PRIVATE /LTCG:STATUS /INCREMENTAL:NO)
        if(NOT PSPRECOMP_MSVC_CGTHREADS STREQUAL "0")
            target_link_options(VCSNative PRIVATE "/CGTHREADS:${PSPRECOMP_MSVC_CGTHREADS}")
        endif()
    endif()
endif()

if(WIN32)
    add_custom_command(TARGET VCSNative POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${VCS_PROFILE_DIR}/third_party/ffmpeg/bin" "$<TARGET_FILE_DIR:VCSNative>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${VCS_PROFILE_DIR}/third_party/ffmpeg/COPYING.LGPLv2.1"
            "$<TARGET_FILE_DIR:VCSNative>/COPYING.LGPLv2.1")
endif()
add_custom_command(TARGET VCSNative POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCS_PROFILE_DIR}/config/VCSNative.ini" "$<TARGET_FILE_DIR:VCSNative>/VCSNative.ini"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCS_PROFILE_DIR}/config/ProperShaders.ini" "$<TARGET_FILE_DIR:VCSNative>/ProperShaders.ini"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCS_PROFILE_DIR}/data/VCSProject2DFX_Lights.bin"
        "$<TARGET_FILE_DIR:VCSNative>/VCSProject2DFX_Lights.bin"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCS_PROFILE_DIR}/third_party/project2dfx/ATTRIBUTION.md"
        "$<TARGET_FILE_DIR:VCSNative>/PROJECT2DFX_ATTRIBUTION.md"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCS_PROFILE_DIR}/third_party/project2dfx/LICENSE.txt"
        "$<TARGET_FILE_DIR:VCSNative>/PROJECT2DFX_LICENSE.txt"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCS_PROFILE_DIR}/third_party/cloudworks/ATTRIBUTION.md"
        "$<TARGET_FILE_DIR:VCSNative>/CLOUDWORKS_ATTRIBUTION.md")

# Profile-specific generator. The root psp_recomp target stays game-neutral.
add_executable(vcs_recomp tools/vcs_codegen_main.cpp)
target_link_libraries(vcs_recomp PRIVATE psprecomp_core)
vcs_set_runtime_output(vcs_recomp)

if(PSPRECOMP_BUILD_PROFILE_TESTS)
    if(NOT PSPRECOMP_BUILD_TESTS)
        enable_testing()
    endif()
    add_executable(vcs_profile_tests tests/vcs_profile_tests.cpp ${VCS_HOST_SOURCES})
    vcs_target_common(vcs_profile_tests)
    vcs_set_runtime_output(vcs_profile_tests)
    add_test(NAME vcs_profile_tests COMMAND vcs_profile_tests)

    add_executable(vcs_config_tests
        tests/vcs_config_tests.cpp
        host/vcs_config.cpp
        host/vcs_camera_input.cpp
        host/vcs_vehicle_input.cpp)
    target_include_directories(vcs_config_tests PRIVATE host)
    add_test(NAME vcs_config_tests COMMAND vcs_config_tests)

    add_executable(audio_resampler_tests tests/audio_resampler_tests.cpp)
    target_include_directories(audio_resampler_tests PRIVATE host)
    add_test(NAME audio_resampler_tests COMMAND audio_resampler_tests)

    add_executable(vcs_bootstrap_paths_tests
        tests/vcs_bootstrap_paths_tests.cpp host/vcs_bootstrap_paths.cpp)
    target_include_directories(vcs_bootstrap_paths_tests PRIVATE host)
    target_link_libraries(vcs_bootstrap_paths_tests PRIVATE psprecomp_core)
    add_test(NAME vcs_bootstrap_paths_tests COMMAND vcs_bootstrap_paths_tests)
endif()

if(WIN32)
    add_executable(vcs_dx12_probe
        tools/dx12_probe_main.cpp
        host/dx12_presenter.cpp
        host/vcs_runtime_log.cpp
        host/vcs_config.cpp
        host/vcs_camera_input.cpp
        host/vcs_vehicle_input.cpp)
    target_include_directories(vcs_dx12_probe PRIVATE host "${PROJECT_SOURCE_DIR}/include")
    target_link_libraries(vcs_dx12_probe PRIVATE d3d12 dxgi d3dcompiler dxguid)
    vcs_set_runtime_output(vcs_dx12_probe)

    add_executable(vcs_dx12_ge_probe
        tools/dx12_ge_probe_main.cpp
        host/ge_gpu_backend_dx12.cpp
        host/vcs_runtime_log.cpp
        host/vcs_config.cpp
        host/vcs_camera_input.cpp
        host/vcs_vehicle_input.cpp)
    target_include_directories(vcs_dx12_ge_probe PRIVATE host "${PROJECT_SOURCE_DIR}/include")
    target_link_libraries(vcs_dx12_ge_probe PRIVATE d3d12 dxgi d3dcompiler dxguid)
    vcs_set_runtime_output(vcs_dx12_ge_probe)
endif()

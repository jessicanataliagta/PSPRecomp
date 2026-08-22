set(VCS_PROFILE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Keep the enormous generated AOT corpus out of MSVC whole-program/LTCG IR by
# default.  Stage 45.8/45.9 cross-unit state makes whole-program analysis of all
# 234 generated units extremely memory hungry on Windows.  The AOT units are
# still fully optimized per translation unit; host/runtime code may still use
# PSPRECOMP_LTO.
option(PSPRECOMP_VCS_AOT_LTO
    "Include generated VCS AOT units in MSVC whole-program optimization" OFF)

option(PSPRECOMP_VCS_TIER2_DEEP_TELEMETRY
    "Compile expensive per-entry Tier-2 coverage/timing counters" OFF)

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
# Keep this at 0, and do not "fix" it without measuring.
#
# /Ob0 does drop __forceinline, not merely auto-inlining: compiling a stand-in
# accessor at each level and reading the disassembly shows /Ob0 emitting a call
# to the accessor itself while /Ob1+ emit a call only to its cold fallback. Since
# every guest load and store goes through a __forceinline fast path, /Ob0 looks
# like it must be turning the most frequent operation in the program into an
# out-of-line call, and /Ob1 looks like a free win.
#
# It measures 13.6% slower. On a 3800-frame route, median frame 11.84 ms at /Ob0
# against 13.69 ms at /Ob1 (median FPS 84.5 against 73.0), guest cpu_us 9.66 ms
# against 11.54 ms. Inlining the fast path at every call site grows the binary
# from 103 MB to 232 MB, and this corpus is already hostile to the instruction
# cache: 234 units, ~50k cross-unit chains, over a million architectural register
# references. The call overhead removed is smaller than the I-cache and BTB
# pressure added.
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
    # The units that carry 80% of the guest's cross-unit calls, measured with
    # PSPRECOMP_UNIT_PROFILE=1 over a 7900-frame route: 141 million calls across
    # 201 live units, roughly 17,900 unit transitions per frame.
    #
    # Only 0043 used to be here, and the census puts it eighth at 3.3% -- the
    # aggressive settings were reaching 3% of the work. 0023 and 0024 alone are
    # 16.6%.
    #
    # Why a list and not the whole corpus: /Ob1 applied to all 234 units measured
    # 13.6% *slower* (median frame 13.69 ms against 11.84 ms) because the binary
    # grew from 103 MB to 232 MB and this corpus is already hostile to the
    # instruction cache. Confining the aggressive settings to the hot 16% keeps
    # the code growth bounded where it cannot pay for itself.
    set(VCS_HOT_UNIT_IDS
        0023 0024 0097 0030 0011 0091 0084 0043 0067 0163
        0087 0142 0066 0152 0109 0179 0085 0215 0064 0149
        0095 0199 0092 0206 0022 0190 0200 0080 0164 0198
        0088 0155 0213 0154 0169 0035 0089)
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

    # Feed the measured hot units to the linker contiguously and in profile rank
    # order. This is deliberately limited to the same 37-unit hot set: globally
    # inflating/reordering the 234-unit corpus previously hurt I-cache/BTB behavior.
    # Linker PGO can refine this further after a physical gameplay training run.
    list(REMOVE_ITEM VCS_GENERATED ${VCS_HOT_SOURCES})
    list(PREPEND VCS_GENERATED ${VCS_HOT_SOURCES})
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

# Tier-2 V4 keeps each profile-guided cluster in its own translation unit.
# This is the actual second AOT layer: hot functions/loops are cold-split from
# the giant generated units and selected cross-unit edges are fused locally.
# /GL- on these sources avoids re-inflating them into a giant LTCG partition;
# their hot paths are already visible to the normal /O2+/Ob3 optimizer.
set(VCS_TIER2_CLUSTER_SOURCES
    host/vcs_tier2_cluster_entity.cpp
    host/vcs_tier2_cluster_geometry.cpp
    host/vcs_tier2_cluster_boundary.cpp
    host/vcs_tier2_cluster_matrix.cpp
    host/vcs_tier2_cluster_physics.cpp
    host/vcs_tier2_cluster_world.cpp
    host/vcs_tier2_cluster_edge43.cpp
)
if(MSVC)
    set_source_files_properties(
        "${VCS_PROFILE_DIR}/host/vcs_tier2_superblocks.cpp"
        ${VCS_TIER2_CLUSTER_SOURCES}
        PROPERTIES COMPILE_OPTIONS "/O2;/Ob3;/bigobj;/GL-")
    # Geometry is the largest V4 hot TU (~7k generated lines) and /Ob3 can
    # make MSVC spend many minutes in pathological inlining/optimization.
    # Keep full /O2 codegen and normal aggressive /Ob2 inlining here; this
    # preserves GPR shadow/SIMD/dataflow while avoiding the compile-time cliff.
    set_source_files_properties(
        "${VCS_PROFILE_DIR}/host/vcs_tier2_cluster_geometry.cpp"
        PROPERTIES COMPILE_OPTIONS "/O2;/Ob2;/bigobj;/GL-")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set_source_files_properties(
        "${VCS_PROFILE_DIR}/host/vcs_tier2_superblocks.cpp"
        ${VCS_TIER2_CLUSTER_SOURCES}
        PROPERTIES COMPILE_OPTIONS "-O2;-g0")
endif()

set(VCS_GPU_BACKEND_SOURCE host/ge_gpu_backend_dx12.cpp)
set(VCS_HDR_POST_SOURCE host/vcs_hdr_post_dx12_stub.cpp)

# ProperShaders is an optional private extension layer. The public repository can
# omit host/propershaders/private entirely and still build a stock VCS renderer.
set(VCS_PROPER_SHADERS_SOURCES
    host/propershaders/ProperShadersConfig.cpp
    host/propershaders/ProperShadersBridge.cpp
)
set(VCS_PRIVATE_SHADER_DIR "${VCS_PROFILE_DIR}/host/propershaders/private")
set(VCS_PRIVATE_SHADER_REQUIRED
    "${VCS_PRIVATE_SHADER_DIR}/PipeSources.hpp"
    "${VCS_PRIVATE_SHADER_DIR}/PrivateShaders.cpp"
    "${VCS_PRIVATE_SHADER_DIR}/BuildingPipe.cpp"
    "${VCS_PRIVATE_SHADER_DIR}/SkinPipe.cpp"
    "${VCS_PRIVATE_SHADER_DIR}/VehiclePipe.cpp"
    "${VCS_PRIVATE_SHADER_DIR}/RealtimeShadows.cpp"
    "${VCS_PRIVATE_SHADER_DIR}/VolumetricClouds.cpp")
set(VCS_PRIVATE_SHADER_COMPLETE TRUE)
foreach(VCS_PRIVATE_SHADER_FILE IN LISTS VCS_PRIVATE_SHADER_REQUIRED)
    if(NOT EXISTS "${VCS_PRIVATE_SHADER_FILE}")
        set(VCS_PRIVATE_SHADER_COMPLETE FALSE)
    endif()
endforeach()
if(VCS_PRIVATE_SHADER_COMPLETE)
    message(STATUS "VCS ProperShaders: private shader pack present")
    list(APPEND VCS_PROPER_SHADERS_SOURCES
        host/propershaders/private/PrivateShaders.cpp
        host/propershaders/private/BuildingPipe.cpp
        host/propershaders/private/SkinPipe.cpp
        host/propershaders/private/VehiclePipe.cpp
        host/propershaders/private/RealtimeShadows.cpp
        host/propershaders/private/VolumetricClouds.cpp)
else()
    message(STATUS "VCS ProperShaders: private shader pack absent; building stock-renderer stubs")
    list(APPEND VCS_PROPER_SHADERS_SOURCES host/propershaders/PrivateShaderStubs.cpp)
endif()
set(VCS_HOST_SOURCES
    host/vcs_tier2_superblocks.cpp
    host/vcs_bootstrap_paths.cpp
    host/vcs_profile.cpp
    host/vcs_native_fast_paths.cpp
    host/vcs_compact_leaves.cpp
    host/vcs_resident_regions.cpp
    host/framebuffer_capture.cpp
    host/display_window.cpp
    host/savedata_utility_ui.cpp
    host/vcs_texture_replacement.cpp
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
    ${VCS_PROPER_SHADERS_SOURCES}
    ${VCS_HDR_POST_SOURCE}
    host/ge_renderer.cpp
    ${VCS_GPU_BACKEND_SOURCE}
    host/dx12_presenter.cpp
)

function(vcs_target_common target)
    target_include_directories(${target} PRIVATE
        "${VCS_PROFILE_DIR}/host"
        "${VCS_PROFILE_DIR}/generated"
        "${VCS_PROFILE_DIR}/third_party/ffmpeg/include")
    target_link_libraries(${target} PRIVATE psprecomp_core ${CMAKE_DL_LIBS})
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
    ${VCS_TIER2_CLUSTER_SOURCES}
    ${VCS_HOST_SOURCES}
    ${VCS_GENERATED}
)
vcs_target_common(VCSNative)
vcs_set_runtime_output(VCSNative)
if(PSPRECOMP_VCS_TIER2_DEEP_TELEMETRY)
    target_compile_definitions(VCSNative PRIVATE PSPRECOMP_TIER2_DEEP_TELEMETRY=1)
endif()
if(MSVC)
    # /Ob3 must stay off the target: target_compile_options lands in the vcxproj
    # AdditionalOptions, which cl.exe sees *after* the per-source
    # InlineFunctionExpansion, so a target-wide /Ob3 silently overrides the
    # /Ob${PSPRECOMP_GENERATED_INLINE_LEVEL} set on the generated corpus above
    # (MSVC reports this as "D9025: overriding '/Ob0' with '/Ob3'").  Apply it to
    # the host sources only.
    target_compile_options(VCSNative PRIVATE ${PSPRECOMP_MSVC_MP_FLAG})
    # APPEND is required here: Tier-2 cluster sources already carry /O2,
    # /bigobj and /GL-. Replacing COMPILE_OPTIONS would silently drop those
    # flags on MSVC and make the Windows build differ from the validated one.
    set_property(SOURCE host/main.cpp ${VCS_TIER2_CLUSTER_SOURCES} ${VCS_HOST_SOURCES}
        DIRECTORY "${VCS_PROFILE_DIR}" APPEND PROPERTY COMPILE_OPTIONS
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
        host/propershaders/ProperShadersConfig.cpp
        host/vcs_camera_input.cpp
        host/vcs_vehicle_input.cpp)
    target_include_directories(vcs_config_tests PRIVATE host)
    add_test(NAME vcs_config_tests COMMAND vcs_config_tests)

    add_executable(audio_resampler_tests tests/audio_resampler_tests.cpp)
    target_include_directories(audio_resampler_tests PRIVATE host)
    add_test(NAME audio_resampler_tests COMMAND audio_resampler_tests)

    add_executable(vfpu_tier2_tests tests/vfpu_tier2_tests.cpp)
    target_include_directories(vfpu_tier2_tests PRIVATE "${PROJECT_SOURCE_DIR}/include")
    # This differential test instantiates a dense set of header-only VFPU Tier-2
    # templates.  MSVC 19.44 can ICE in the LTCG back-end while linking this
    # otherwise-valid test (C1001 in allegrex_context.hpp).  Keep host/game LTCG
    # enabled, but compile this diagnostic target as a normal translation unit.
    set_property(TARGET vfpu_tier2_tests PROPERTY INTERPROCEDURAL_OPTIMIZATION FALSE)
    set_property(TARGET vfpu_tier2_tests PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE FALSE)
    set_property(TARGET vfpu_tier2_tests PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO FALSE)
    set_property(TARGET vfpu_tier2_tests PROPERTY INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL FALSE)
    if(MSVC)
        target_compile_options(vfpu_tier2_tests PRIVATE /Od /Ob0 /GL-)
    endif()
    add_test(NAME vfpu_tier2_tests COMMAND vfpu_tier2_tests)

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
        host/vcs_tier2_superblocks.cpp
        host/vcs_config.cpp
        host/propershaders/ProperShadersConfig.cpp
        host/vcs_camera_input.cpp
        host/vcs_vehicle_input.cpp)
    target_include_directories(vcs_dx12_probe PRIVATE host "${PROJECT_SOURCE_DIR}/include")
    target_link_libraries(vcs_dx12_probe PRIVATE d3d12 dxgi d3dcompiler dxguid)
    vcs_set_runtime_output(vcs_dx12_probe)

    add_executable(vcs_dx12_ge_probe
        tools/dx12_ge_probe_main.cpp
        host/ge_gpu_backend_dx12.cpp
        host/vcs_runtime_log.cpp
        host/vcs_tier2_superblocks.cpp
        host/vcs_config.cpp
        host/vcs_draw_distance_patch.cpp
        ${VCS_PROPER_SHADERS_SOURCES}
        host/vcs_camera_input.cpp
        host/vcs_vehicle_input.cpp)
    target_include_directories(vcs_dx12_ge_probe PRIVATE host "${PROJECT_SOURCE_DIR}/include")
    target_link_libraries(vcs_dx12_ge_probe PRIVATE psprecomp_core d3d12 dxgi d3dcompiler dxguid)
    vcs_set_runtime_output(vcs_dx12_ge_probe)
endif()

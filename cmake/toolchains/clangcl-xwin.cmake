# Cross-compile Windows x64 from Linux with clang-cl + lld-link against an
# xwin-splatted MSVC CRT / Windows SDK (default: ~/.xwin).
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

set(XWIN_DIR "$ENV{HOME}/.xwin" CACHE PATH "xwin splat output")

set(CMAKE_C_COMPILER clang-cl)
set(CMAKE_CXX_COMPILER clang-cl)
set(CMAKE_LINKER lld-link)
set(CMAKE_RC_COMPILER llvm-rc)
set(CMAKE_MT llvm-mt)
set(CMAKE_AR llvm-lib)

set(_xwin_inc
  "/imsvc${XWIN_DIR}/crt/include"
  "/imsvc${XWIN_DIR}/sdk/include/ucrt"
  "/imsvc${XWIN_DIR}/sdk/include/um"
  "/imsvc${XWIN_DIR}/sdk/include/shared"
  "/imsvc${XWIN_DIR}/sdk/include/winrt"
  "/imsvc${XWIN_DIR}/sdk/include/cppwinrt")
string(JOIN " " _xwin_inc_str ${_xwin_inc})
set(CMAKE_C_FLAGS_INIT "--target=x86_64-pc-windows-msvc -fuse-ld=lld-link ${_xwin_inc_str}")
set(CMAKE_CXX_FLAGS_INIT "--target=x86_64-pc-windows-msvc -fuse-ld=lld-link ${_xwin_inc_str}")

set(_xwin_lib
  "/libpath:${XWIN_DIR}/crt/lib/x86_64"
  "/libpath:${XWIN_DIR}/sdk/lib/um/x86_64"
  "/libpath:${XWIN_DIR}/sdk/lib/ucrt/x86_64")
string(JOIN " " _xwin_lib_str ${_xwin_lib})
set(CMAKE_EXE_LINKER_FLAGS_INIT "${_xwin_lib_str}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${_xwin_lib_str}")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${_xwin_lib_str}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# xwin ships no debug CRT (msvcrtd.lib); always use the release DLL runtime.
set(CMAKE_TRY_COMPILE_CONFIGURATION Release)
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDLL)

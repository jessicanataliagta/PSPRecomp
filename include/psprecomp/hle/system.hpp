#pragma once

// Small generic PSP system imports shared by title profiles: scePower,
// sceUmdUser status calls, LoadExecForUser exit callback and sceRtc
// (wall-clock ticks and ScePspDateTime conversion).

#include "psprecomp/runtime.hpp"

namespace psprecomp::hle {

void install_system_hle(Runtime &runtime);

} // namespace psprecomp::hle

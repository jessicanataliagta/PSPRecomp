#pragma once

// sceAtrac3plus HLE shared by title profiles. Decodes the AT3 file the guest is
// streaming with FFmpeg, so a profile using it links FFmpeg and adds the
// psprecomp_hle_atrac sources (see CMakeLists.txt).

#include "psprecomp/runtime.hpp"

namespace psprecomp::hle {

// Closes every decoder and frees all ATRAC IDs.
void reset_atrac();
void install_atrac_hle(Runtime &runtime);

} // namespace psprecomp::hle

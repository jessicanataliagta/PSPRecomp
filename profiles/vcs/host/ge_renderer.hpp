#pragma once

#include "psprecomp/guest_memory.hpp"
#include "psprecomp/hle/ge.hpp"
#include "psprecomp/hle/ge_renderer.hpp"

#include <array>
#include <cstdint>
#include <string>

namespace vcs {

// GE matrix state, BBOX results and render statistics are part of the shared
// GE interpreter (psprecomp/hle/ge.hpp); the renderer below consumes them.
using psprecomp::hle::GeBoundingBoxResult;
using psprecomp::hle::GeRenderStats;
using psprecomp::hle::GeTransformState;
using psprecomp::hle::reset_ge_transform_state;
using psprecomp::hle::update_ge_transform_state;


// Rasterizer API, shared with other profiles (psprecomp/hle/ge_renderer.hpp).
using psprecomp::hle::test_ge_bounding_box;
using psprecomp::hle::render_ge_primitive;
using psprecomp::hle::GePhaseTotals;
using psprecomp::hle::ge_phase_totals;
using psprecomp::hle::reset_ge_phase_totals;

} // namespace vcs

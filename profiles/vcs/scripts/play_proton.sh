#!/usr/bin/env bash
# Linux counterpart of play.bat: runs the Windows VCSNative.exe through Proton.
# Usage: play_proton.sh [GAME_ROOT]
# Env overrides: PROTON (path to a Proton dir), VCS_BIN (path to VCSNative.exe).
set -euo pipefail

PROFILE="$(cd "$(dirname "$0")/.." && pwd)"
REPO="$(cd "$PROFILE/../.." && pwd)"
STEAM_ROOT="${STEAM_ROOT:-$HOME/.local/share/Steam}"
PROTON="${PROTON:-$STEAM_ROOT/steamapps/common/Proton 10.0}"

BIN="${VCS_BIN:-}"
for candidate in "$REPO/out/vcs-win/bin/Release/VCSNative.exe" \
                 "$REPO/out/vcs-release/bin/Release/VCSNative.exe"; do
  [[ -z "$BIN" && -f "$candidate" ]] && BIN="$candidate"
done
[[ -n "$BIN" ]] || { echo "VCSNative.exe not found. Build it first." >&2; exit 3; }

GAME="${1:-$PROFILE/game}"
ELF="$GAME/PSP_GAME/SYSDIR/EBOOT_DECRYPTED.ELF"
[[ -f "$ELF" ]] || { echo "Missing $ELF" >&2; exit 5; }
for movie in LOGO.PMF TITLES.PMF; do
  [[ -f "$GAME/PSP_GAME/USRDIR/RUNDATA/PSP/MOVIES/$movie" ]] ||
    { echo "The VCS game root is incomplete: $movie is missing." >&2; exit 6; }
done

# Proton exposes / as drive Z:.
winpath() { printf 'Z:%s' "${1//\//\\}"; }

export STEAM_COMPAT_DATA_PATH="${STEAM_COMPAT_DATA_PATH:-$REPO/out/proton-prefix}"
export STEAM_COMPAT_CLIENT_INSTALL_PATH="$STEAM_ROOT"
mkdir -p "$STEAM_COMPAT_DATA_PATH"

# Same runtime switches as play.bat.
export PSPRECOMP_CONFIG="$(winpath "$PROFILE/config/VCSNative.ini")"
export PSPRECOMP_GE_BACKEND="${PSPRECOMP_GE_BACKEND:-directx12}"
export PSPRECOMP_GE_GPU_TELEMETRY="${PSPRECOMP_GE_GPU_TELEMETRY:-0}"
export PSPRECOMP_GE_GPU_REPORT="${PSPRECOMP_GE_GPU_REPORT:-0}"
export PSPRECOMP_GE_GPU_HW_CULL="${PSPRECOMP_GE_GPU_HW_CULL:-1}"
export PSPRECOMP_DX12_DEBUG="${PSPRECOMP_DX12_DEBUG:-0}"
export PSPRECOMP_DX12_GE_READBACK="${PSPRECOMP_DX12_GE_READBACK:-0}"
export PSPRECOMP_DX12_GE_STRICT="${PSPRECOMP_DX12_GE_STRICT:-0}"
export PSPRECOMP_GE_PARALLEL_VERTEX_DECODE="${PSPRECOMP_GE_PARALLEL_VERTEX_DECODE:-0}"
export PSPRECOMP_RASTER_THREADS="${PSPRECOMP_RASTER_THREADS:-4}"
export PSPRECOMP_GE_DIRECT_NONINDEXED_DRAW="${PSPRECOMP_GE_DIRECT_NONINDEXED_DRAW:-1}"
# Packed 0x0115 GPU decode explodes geometry under vkd3d-proton; keep it off on Linux.
export PSPRECOMP_DX12_PACKED_0115="${PSPRECOMP_DX12_PACKED_0115:-0}"
export PSPRECOMP_DX12_NATIVE_INDEXED_DRAW="${PSPRECOMP_DX12_NATIVE_INDEXED_DRAW:-1}"
export PSPRECOMP_DX12_BATCH_MERGE="${PSPRECOMP_DX12_BATCH_MERGE:-1}"
export PSPRECOMP_ENABLE_FAST_088B1554="${PSPRECOMP_ENABLE_FAST_088B1554:-1}"
export PSPRECOMP_GE_GPU_DUAL_FRAME="${PSPRECOMP_GE_GPU_DUAL_FRAME:-0}"
unset PSPRECOMP_GE_ASYNC PSPRECOMP_GE_GPU_SKIP_SOFTWARE_RASTER \
      PSPRECOMP_CHAIN_DEPTH PSPRECOMP_TIME_TICK_DISPATCHES

cd "$(dirname "$BIN")"
exec "$PROTON/proton" run "$BIN" "$(winpath "$ELF")" "$(winpath "$GAME")"

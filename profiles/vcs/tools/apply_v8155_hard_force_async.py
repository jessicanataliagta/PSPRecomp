#!/usr/bin/env python3
from __future__ import annotations

import argparse
from pathlib import Path

STAGE_8151 = 'stage=perf-v8.15.1-async-ge-overlap-force-activation-2026-08-27'
STAGE_8155 = 'stage=perf-v8.15.5-hard-force-async-ge-2026-08-27'
MAIN_MARK = 'PSPRECOMP_V8155_HARD_FORCE_ASYNC'
ASYNC_OPT_IN_MARK = 'PSPRECOMP_V8155_ENABLE_ASYNC'
LOG_MARK = 'PSPRECOMP_V8155_RUNTIME_LOG'
CONFIG_MARK = 'PSPRECOMP_V8155_PRESERVE_EXPLICIT_GE_FLAGS'


def read(p: Path) -> str:
    return p.read_text(encoding='utf-8', errors='strict')


def write(p: Path, text: str) -> None:
    tmp = p.with_suffix(p.suffix + '.v8155tmp')
    tmp.write_text(text, encoding='utf-8', newline='\n')
    tmp.replace(p)


def patch_main(p: Path) -> bool:
    old = read(p)
    if MAIN_MARK in old:
        return False
    text = old
    old_block = '''    // PSPRECOMP_V815_PRODUCTION_DEFAULTS
    // PSPRECOMP_V8151_FORCE_PRODUCTION_ON: stale shell/user variables must not
    // silently turn the performance architecture off. A new explicit rollback
    // switch is available for compatibility bisects.
    const char *v8151_force_sync_text = std::getenv("PSPRECOMP_V8151_FORCE_SYNC");
    const bool v8151_force_sync = v8151_force_sync_text != nullptr &&
        *v8151_force_sync_text != '\\0' && *v8151_force_sync_text != '0';
#ifdef _WIN32
    _putenv_s("PSPRECOMP_GE_ASYNC", v8151_force_sync ? "0" : "1");
    _putenv_s("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE", v8151_force_sync ? "0" : "1");
#else
    setenv("PSPRECOMP_GE_ASYNC", v8151_force_sync ? "0" : "1", 1);
    setenv("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE", v8151_force_sync ? "0" : "1", 1);
#endif
'''
    if old_block not in text:
        raise RuntimeError(f'{p}: V8.15.1 activation block missing; refusing blind patch')
    new_block = '''    // PSPRECOMP_V815_PRODUCTION_DEFAULTS
    // PSPRECOMP_V8151_FORCE_PRODUCTION_ON
    // PSPRECOMP_V8155_HARD_FORCE_ASYNC: production VCSNative always runs the
    // CPU/GE overlap architecture. Older shell variables and the temporary
    // V8.15.1 rollback variable are deliberately cleared here so an inherited
    // environment cannot silently turn the fast path off again.
#ifdef _WIN32
    _putenv_s("PSPRECOMP_V8151_FORCE_SYNC", "");
    _putenv_s("PSPRECOMP_GE_ASYNC", "1");
    _putenv_s("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE", "1");
#else
    unsetenv("PSPRECOMP_V8151_FORCE_SYNC");
    setenv("PSPRECOMP_GE_ASYNC", "1", 1);
    setenv("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE", "1", 1);
#endif
'''
    text = text.replace(old_block, new_block, 1)

    # Keep the existing V8.15.1 activation line, but add an unmistakable marker
    # after logging is initialized. This is source + runtime proof that this
    # stronger state reached the binary and process environment.
    anchor = '        vcs::runtime_log_initialize(configuration);\n'
    if text.count(anchor) != 1:
        raise RuntimeError(f'{p}: runtime_log_initialize anchor count={text.count(anchor)}')
    injected = anchor + (
        '        vcs::runtime_log_line("v8155 hard_force_async=1 parallel_vertex=1");\n'
    )
    text = text.replace(anchor, injected, 1)
    write(p, text)
    return True


def patch_log(p: Path) -> bool:
    old = read(p)
    text = old
    if STAGE_8155 not in text:
        if STAGE_8151 in text:
            text = text.replace(STAGE_8151, STAGE_8155, 1)
        else:
            raise RuntimeError(f'{p}: expected V8.15.1 stage before V8.15.5')
    if 'ge_async_hard_force=1' not in text:
        needle = '           << " ge_async_quarantined=0 ge_async_present_safepoint=1 parallel_vertex_decode_quarantined=0 legacy_perf_env_ignored=1"\n'
        repl = '           << " ge_async_quarantined=0 ge_async_present_safepoint=1 ge_async_hard_force=1 parallel_vertex_decode_quarantined=0 legacy_perf_env_ignored=1"\n'
        if needle not in text:
            raise RuntimeError(f'{p}: async metadata anchor missing')
        text = text.replace(needle, repl, 1)
    if LOG_MARK not in text:
        stage_line = f'    s.file << "{STAGE_8155}\\n";\n'
        if stage_line in text:
            text = text.replace(stage_line, stage_line.rstrip('\n') + f' // {LOG_MARK}\n', 1)
        else:
            # V8.15.1 may already carry a comment on the line. The stage string
            # itself is sufficient; add a nearby marker without changing output.
            anchor = '    s.file << "config=" << configuration.source_path.string() << \'\\n\';\n'
            if anchor not in text:
                raise RuntimeError(f'{p}: config log anchor missing')
            text = text.replace(anchor, f'    // {LOG_MARK}\n' + anchor, 1)
    if text == old:
        return False
    write(p, text)
    return True


def patch_config(p: Path) -> bool:
    """Keep the DX12 safety defaults without undoing the launcher's opt-in."""
    old = read(p)
    if CONFIG_MARK in old:
        return False
    old_block = '''    // V5 STABLE RECOVERY2: return every unproven scheduler/CPU-renderer
    // experiment to the last gameplay-stable V4 AMD/UMA baseline.  Both GE
    // async and parallel vertex decode are quarantined in production, and old
    // inherited environment flags are deliberately ignored.
    if (config.rendering.backend == RenderingBackend::DirectX12) {
        set_environment_value("PSPRECOMP_GE_ASYNC", "0");
        set_environment_value("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE", "0");
    }
'''
    if old_block not in old:
        raise RuntimeError(f'{p}: DX12 async quarantine block missing; refusing blind patch')
    new_block = '''    // PSPRECOMP_V8155_PRESERVE_EXPLICIT_GE_FLAGS
    // DX12 keeps conservative defaults for an otherwise unconfigured process,
    // while preserving an explicit launcher/benchmark override.  The VCS
    // launcher enables overlap before loading this configuration; overwriting
    // it here would silently serialize the guest CPU with the GE worker.
    if (config.rendering.backend == RenderingBackend::DirectX12) {
        if (std::getenv("PSPRECOMP_GE_ASYNC") == nullptr)
            set_environment_value("PSPRECOMP_GE_ASYNC", "0");
        if (std::getenv("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE") == nullptr)
            set_environment_value("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE", "0");
    }
'''
    write(p, old.replace(old_block, new_block, 1))
    return True


def check(root: Path) -> None:
    main = read(root / 'profiles/vcs/host/main.cpp')
    log = read(root / 'profiles/vcs/host/vcs_runtime_log.cpp')
    config = read(root / 'profiles/vcs/host/vcs_config.cpp')
    required_main = [
        MAIN_MARK,
        ASYNC_OPT_IN_MARK,
        '_putenv_s("PSPRECOMP_V8151_FORCE_SYNC", "")',
        '_putenv_s("PSPRECOMP_GE_ASYNC", async_opt_in ? "1" : "0")',
        '_putenv_s("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE", parallel_decode_enabled ? "1" : "0")',
        'v8155 hard_force_async=',
    ]
    for token in required_main:
        if token not in main:
            raise RuntimeError(f'V8.15.5 check: main missing {token}')
    forbidden = [
        '_putenv_s("PSPRECOMP_GE_ASYNC", v8151_force_sync ? "0" : "1")',
        '_putenv_s("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE", v8151_force_sync ? "0" : "1")',
    ]
    for token in forbidden:
        if token in main:
            raise RuntimeError(f'V8.15.5 check: conditional rollback still present: {token}')
    for token in [STAGE_8155, 'ge_async_default=0', 'parallel_vertex_decode_default=0',
                  'ge_async_opt_in=PSPRECOMP_V8155_ENABLE_ASYNC',
                  'ge_async_present_safepoint=1', 'ge_async_hard_force=0', LOG_MARK]:
        if token not in log:
            raise RuntimeError(f'V8.15.5 check: runtime log missing {token}')
    for token in [CONFIG_MARK,
                  'if (std::getenv("PSPRECOMP_GE_ASYNC") == nullptr)',
                  'if (std::getenv("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE") == nullptr)']:
        if token not in config:
            raise RuntimeError(f'V8.15.5 check: config missing {token}')


def main_entry() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument('root', nargs='?', default=None)
    ap.add_argument('--check', action='store_true')
    ns = ap.parse_args()
    root = Path(ns.root).resolve() if ns.root else Path(__file__).resolve().parents[3]
    if ns.check:
        check(root)
        print('V8.15.5 CHECK OK: async + parallel vertex require explicit opt-in')
        return 0
    changed = []
    if patch_main(root / 'profiles/vcs/host/main.cpp'): changed.append('main.cpp')
    if patch_log(root / 'profiles/vcs/host/vcs_runtime_log.cpp'): changed.append('vcs_runtime_log.cpp')
    if patch_config(root / 'profiles/vcs/host/vcs_config.cpp'): changed.append('vcs_config.cpp')
    check(root)
    print('V8.15.5 hard-force async applied: ' + (', '.join(changed) if changed else 'already applied'))
    return 0


if __name__ == '__main__':
    raise SystemExit(main_entry())

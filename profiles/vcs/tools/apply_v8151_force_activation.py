#!/usr/bin/env python3
from __future__ import annotations

import argparse
from pathlib import Path

STAGE_OLD = 'stage=perf-v8.10-extreme-cpu-resident-regions-2026-08-18'
STAGE_815 = 'stage=perf-v8.15-async-ge-overlap-2026-08-27'
STAGE_8151 = 'stage=perf-v8.15.1-async-ge-overlap-force-activation-2026-08-27'
STAGE_8155 = 'stage=perf-v8.15.5-hard-force-async-ge-2026-08-27'
MAIN_MARK = 'PSPRECOMP_V8151_ACTIVATION_LOG'
PROFILE_MARK = 'PSPRECOMP_V8151_WORKER_LOG'
SAFE_ASYNC_MARK = 'PSPRECOMP_V8155_ENABLE_ASYNC'


def read(p: Path) -> str:
    return p.read_text(encoding='utf-8', errors='strict')


def write(p: Path, s: str) -> None:
    tmp = p.with_suffix(p.suffix + '.v8151tmp')
    tmp.write_text(s, encoding='utf-8', newline='\n')
    tmp.replace(p)


def patch_main(p: Path) -> bool:
    old = read(p)
    text = old
    force_mark = 'PSPRECOMP_V8151_FORCE_PRODUCTION_ON'
    if force_mark not in text:
        old_block = '''    // PSPRECOMP_V815_PRODUCTION_DEFAULTS: VCS production defaults. Tests do not execute this main(),
    // so their historical synchronous behavior stays isolated. User environment
    // variables always win and provide immediate compatibility rollback.
#ifdef _WIN32
    if (std::getenv("PSPRECOMP_GE_ASYNC") == nullptr)
        _putenv_s("PSPRECOMP_GE_ASYNC", "1");
    if (std::getenv("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE") == nullptr)
        _putenv_s("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE", "1");
#else
    if (std::getenv("PSPRECOMP_GE_ASYNC") == nullptr)
        setenv("PSPRECOMP_GE_ASYNC", "1", 0);
    if (std::getenv("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE") == nullptr)
        setenv("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE", "1", 0);
#endif
'''
        if old_block not in text:
            raise RuntimeError(f'{p}: V8.15 production-default block missing')
        new_block = '''    // PSPRECOMP_V815_PRODUCTION_DEFAULTS
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
        text = text.replace(old_block, new_block, 1)
    if MAIN_MARK not in text:
        anchor = '        vcs::runtime_log_initialize(configuration);\n'
        if text.count(anchor) != 1:
            raise RuntimeError(f'{p}: runtime_log_initialize anchor count={text.count(anchor)}')
        ins = anchor + (
            f'        // {MAIN_MARK}: prove the production defaults reached this executable.\n'
            '        {\n'
            '            const char *async_env = std::getenv("PSPRECOMP_GE_ASYNC");\n'
            '            const char *decode_env = std::getenv("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE");\n'
            '            vcs::runtime_log_line(std::string("v8151 activation ge_async_env=") +\n'
            '                (async_env != nullptr ? async_env : "<unset>") +\n'
            '                " parallel_vertex_env=" +\n'
            '                (decode_env != nullptr ? decode_env : "<unset>"));\n'
            '        }\n'
        )
        text = text.replace(anchor, ins, 1)
    if text == old:
        return False
    write(p, text)
    return True


def patch_profile(p: Path) -> bool:
    old = read(p)
    if PROFILE_MARK in old:
        return False
    anchor = '    ge_async.thread = std::thread(&ge_async_worker_main);\n'
    if old.count(anchor) != 1:
        raise RuntimeError(f'{p}: worker start anchor count={old.count(anchor)}')
    repl = anchor + (
        f'    // {PROFILE_MARK}: runtime proof that the asynchronous consumer actually started.\n'
        '    vcs::runtime_log_line("v8151 ge_async_worker_started=1");\n'
    )
    write(p, old.replace(anchor, repl, 1))
    return True


def patch_log(p: Path) -> bool:
    old = read(p)
    text = old
    changed = False
    # V8.15.5 deliberately supersedes the old hard-on policy with a safe,
    # explicit opt-in.  Do not rewrite its metadata back to the quarantined
    # V8.15.1 defaults when the normal build script reapplies this prerequisite.
    if STAGE_8155 in text and SAFE_ASYNC_MARK in text:
        return False
    if STAGE_8151 not in text and STAGE_8155 not in text:
        if STAGE_815 in text:
            text = text.replace(STAGE_815, STAGE_8151, 1)
            changed = True
        elif STAGE_OLD in text:
            text = text.replace(STAGE_OLD, STAGE_8151, 1)
            changed = True
        else:
            raise RuntimeError(f'{p}: no known stage string to update')
    if 'ge_async_default=1 parallel_vertex_decode_default=1' not in text:
        text = text.replace('ge_async_default=0 parallel_vertex_decode_default=0',
                            'ge_async_default=1 parallel_vertex_decode_default=1', 1)
        changed = text != old or changed
    if 'ge_async_present_safepoint=1' not in text:
        text = text.replace('ge_async_quarantined=1 parallel_vertex_decode_quarantined=1 legacy_perf_env_ignored=1',
                            'ge_async_quarantined=0 ge_async_present_safepoint=1 parallel_vertex_decode_quarantined=0 legacy_perf_env_ignored=1', 1)
        changed = text != old or changed
    if changed:
        write(p, text)
    return changed


def check(root: Path) -> None:
    main = read(root / 'profiles/vcs/host/main.cpp')
    profile = read(root / 'profiles/vcs/host/vcs_profile.cpp')
    log = read(root / 'profiles/vcs/host/vcs_runtime_log.cpp')
    required_main = [
        'PSPRECOMP_V815_PRODUCTION_DEFAULTS',
        'PSPRECOMP_V8151_FORCE_PRODUCTION_ON',
        MAIN_MARK,
        'v8151 activation ge_async_env=',
    ]
    for tok in required_main:
        if tok not in main:
            raise RuntimeError(f'V8.15.1 check: main missing {tok}')
    # Monotonic prerequisite: V8.15.5 intentionally replaces the V8.15.1
    # conditional rollback with unconditional production-on. Accept either.
    v8151_conditional = (
        '_putenv_s("PSPRECOMP_GE_ASYNC", v8151_force_sync ? "0" : "1")' in main and
        '_putenv_s("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE", v8151_force_sync ? "0" : "1")' in main
    )
    v8155_hard = (
        'PSPRECOMP_V8155_HARD_FORCE_ASYNC' in main and
        '_putenv_s("PSPRECOMP_GE_ASYNC", "1")' in main and
        '_putenv_s("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE", "1")' in main
    )
    v8155_safe = (
        'PSPRECOMP_V8155_HARD_FORCE_ASYNC' in main and
        SAFE_ASYNC_MARK in main and
        '_putenv_s("PSPRECOMP_GE_ASYNC", async_opt_in ? "1" : "0")' in main and
        ('_putenv_s("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE", async_opt_in ? "1" : "0")' in main or
         '_putenv_s("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE", parallel_decode_enabled ? "1" : "0")' in main)
    )
    if not (v8151_conditional or v8155_hard or v8155_safe):
        raise RuntimeError('V8.15.1 check: neither V8.15.1 conditional nor V8.15.5 hard activation is present')
    required_profile = [
        'PSPRECOMP_V815_ASYNC_GE_OVERLAP',
        'PSPRECOMP_V815_PRESENT_SAFEPOINT',
        'presentation_requested',
        'worker_busy',
        'ge_async_begin_presentation',
        'ge_async_end_presentation',
        PROFILE_MARK,
        'v8151 ge_async_worker_started=1',
    ]
    for tok in required_profile:
        if tok not in profile:
            raise RuntimeError(f'V8.15.1 check: profile missing {tok}')
    # V8.15.3: scope this check to the sceDisplay wait_vblank lambda only.
    # ge_async_wait_idle(rt) remains required in true GE synchronization HLEs
    # such as DrawSync/GetCmd/context save+restore and must not fail this check.
    vblank_start = profile.find("auto wait_vblank =")
    if vblank_start < 0:
        raise RuntimeError('V8.15.1 check: wait_vblank lambda missing')
    vblank_prefix = profile[vblank_start:vblank_start + 1800]
    if 'PSPRECOMP_V815_PRESENT_SAFEPOINT' not in vblank_prefix:
        raise RuntimeError('V8.15.1 check: VBlank presentation safe-point marker missing')
    if 'if (!ge_async_wait_idle(rt)) return;' in vblank_prefix:
        raise RuntimeError('V8.15.1 check: old VBlank GE idle barrier still present')
    if STAGE_8151 not in log and STAGE_8155 not in log:
        raise RuntimeError('V8.15.1 check: runtime log missing V8.15.1/V8.15.5 stage')
    if SAFE_ASYNC_MARK in main:
        for tok in ['ge_async_default=0', 'parallel_vertex_decode_default=0',
                    'ge_async_opt_in=PSPRECOMP_V8155_ENABLE_ASYNC',
                    'ge_async_present_safepoint=1']:
            if tok not in log:
                raise RuntimeError(f'V8.15.1 check: runtime log missing {tok}')
    else:
        for tok in ['ge_async_default=1', 'parallel_vertex_decode_default=1',
                    'ge_async_present_safepoint=1']:
            if tok not in log:
                raise RuntimeError(f'V8.15.1 check: runtime log missing {tok}')


def main_entry() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument('root', nargs='?', default=None)
    ap.add_argument('--check', action='store_true')
    ns = ap.parse_args()
    root = Path(ns.root).resolve() if ns.root else Path(__file__).resolve().parents[3]
    if ns.check:
        check(root)
        print('V8.15.1 CHECK OK: source activation + async worker proof + stage marker')
        return 0
    changed=[]
    if patch_main(root / 'profiles/vcs/host/main.cpp'): changed.append('main.cpp')
    if patch_profile(root / 'profiles/vcs/host/vcs_profile.cpp'): changed.append('vcs_profile.cpp')
    if patch_log(root / 'profiles/vcs/host/vcs_runtime_log.cpp'): changed.append('vcs_runtime_log.cpp')
    check(root)
    print('V8.15.1 force activation applied: ' + (', '.join(changed) if changed else 'already applied'))
    return 0

if __name__ == '__main__':
    raise SystemExit(main_entry())

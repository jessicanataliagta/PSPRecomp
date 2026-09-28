#!/usr/bin/env python3
from pathlib import Path
import sys
root = Path(__file__).resolve().parents[3]
profile = root / 'profiles' / 'vcs'
host = profile / 'host'
source = (host / 'vcs_profile.cpp').read_text(encoding='utf-8')
log = (host / 'vcs_runtime_log.cpp').read_text(encoding='utf-8')

def need(cond, msg):
    if not cond:
        print('FAIL:', msg)
        raise SystemExit(1)
    print('PASS:', msg)

need('stage=correctness-v8.2.2-recovery-save-audio-news-2026-08-17' in log, 'V8.2.2 runtime stage')
need('recovery_from_821=1' in log and 'save_lifecycle_v82=1' in log, 'V8.2 lifecycle recovery metadata')
need('sas_endflag_live=1' in log and 'sas_loop_history_restore=1' in log, 'SAS correctness metadata')
need('atrac_virtual_source=1' in log and 'atrac_stall_diag=1' in log, 'ATRAC source metadata')
need('std::uint32_t end_flags{0xFFFFFFFFu};' in source, 'SAS checkpoint-compatible flags stored')
need('sas_refresh_end_flags();\n            ctx.set_gpr(2, sas_state.end_flags);' in source, 'GetEndFlag refreshes current voice flags')
need(source.count('sas_refresh_end_flags();') >= 2, 'Core paths refresh end flags')
need('voice.history1 = voice.loop_start_history1;' in source and 'voice.history2 = voice.loop_start_history2;' in source, 'VAG loop predictor restored')
need('struct SavedataPendingWrite' in source and 'commit_savedata_writes(writes)' in source, 'transactional savedata commit present')
need('SAVEDATA_SAVE acknowledged status=QUIT' in source, 'save lifecycle keeps V8.2 QUIT behavior with diagnostics')
need('utility retained until shutdown' not in source, 'V8.2.1 speculative save lifecycle removed')
need('recent_atrac_reads[dst]' in source and 'virtual_disc_file_at_offset' in source, 'virtual UMD ATRAC producer mapping present')
need('identify_atrac_source(buffer, header_bytes, parsed)' in source, 'ATRAC setup consumes producer mapping')
need('if (voice->paused || !voice->on)' in source, 'V8.2 paused-KeyOff behavior restored')
need('sas_key_off_voice' not in source, 'V8.2.1 paused-KeyOff helper removed')
print('V8.2.2 correctness recovery audit PASS')

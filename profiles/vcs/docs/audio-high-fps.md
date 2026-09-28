# High-FPS audio / animation timing (2026-09-05)

Historical timing work started at `efa4728`; revisions 4–5 were applied on
`32f4b08`. Current user-tested checkout: `3afd892` (`tentativa de corrigir audio ingame`).

## Estado confirmado pelo usuário — 2026-09-05

Build: revisão 5, `VCSNative.exe` compilado em 05/09/2026 às 14:45:35.
Após testar cutscene e provocar a derrapagem, o usuário informou que os sons
em loop durante o gameplay aparentemente foram corrigidos. Isso é validação
da reprodução relatada, não uma garantia sobre todos os sons ou missões.

### Correção dos loops: o que preservar

O VCS acumula comandos em dois buffers, e a thread de áudio os consome depois.
Com vários frames de gameplay entre consumos, duas situações eram possíveis:

1. START seguido de STOP deixava ambos os bits marcados. Como o consumidor
   executa STOP antes de START, religava o som já cancelado. Um STOP novo agora
   cancela o START ainda pendente. STOP seguido de START continua permitido.
2. A consulta de reprodução olhava apenas o SAS, que ainda não havia recebido
   o START pendente. O jogo podia considerar o canal terminado antes de começar
   e perder o controle sobre ele. A consulta agora inclui os dois buffers;
   cada canal visitado pelo consumidor tem seu START pendente removido.

Implementação a preservar ao regenerar código:

* `host/vcs_audio_commands.hpp`: ordem dos comandos e máscara dos canais
  pendentes, com empacotamento de 24 + 4 bits.
* `generated/generated_unit_0001.cpp`: consulta `0x0880A5FC`, cancelamento
  `0x0880A6A4`, consumo `0x0880AE90`.
* `tests/vcs_frame_timing_tests.cpp`: 510 sequências START/STOP, estados dos
  buffers e guardas contra remoção dos pontos de integração gerados.

Não substituir isso por timeout de sons, limitação de FPS ou mudanças gerais
de envelope: motores, buzinas e derrapagens podem legitimamente usar loops.
Não remover a correção de root motion ao investigar áudio.

Evidência do log da revisão 5, execução 15:01:39–15:04:26:
`cancelled_starts=1`, `pending_owner_guards=2281`, `rejected_on=0`.
Os contadores mostram que os novos caminhos foram acionados; não equivalem
à quantidade de sons presos. O caminho anterior de aborto de lote permaneceu
inativo (`aborted_batches=0`, `recovered_stops=0`).

### Pendente: rádio da primeira cena na cutscene seguinte

O usuário esclareceu que a primeira cena usa uma rádio do jogo. Ao pular para
a próxima cutscene (Victor e o sargento no escritório), às vezes continua
ouvindo essa rádio. Tratar como problema separado dos loops SAS de gameplay.
A sincronização labial também não deve ser declarada totalmente resolvida.

O log desta execução registra:

* 15:01:56.414: encerra decoder 0 de `EMOTION.AT3`, sample 102400.
* 15:01:56.462: cria novamente decoder 0 de `EMOTION.AT3`, buffer `0x08BFEC80`.
* 15:01:56.507: seek para sample 0; a rádio volta a ser decodificada.
* 15:01:59.550: encerra a rádio; em seguida abre `CITY.AT3`.
* `JERA1.AT3`, presente na reprodução anterior da cena do escritório, não
  aparece nessa execução.

Na reabertura, `ATRAC_SOURCE` registra `direct_buffer=0` e
`fallback_candidates=1`: a identificação foi pelo conteúdo do buffer, sem
associação direta a uma leitura de arquivo. Isso não prova se o jogo pediu
a faixa errada, se o buffer conservou dados antigos ou se o identificador
escolheu uma fonte incorreta. É o próximo ponto de investigação. O log
mostra uma reabertura efetiva da rádio, não apenas uma pequena cauda de PCM
na fila de saída. Não aplicar um flush global como se isso explicasse tudo.

Não abrir o jogo automaticamente. O usuário realiza os testes visuais.

## Changes

* Revision 6 (based on `2d6006e`) addresses the stream-request branch, not the
  confirmed SAS/gameplay fixes. At `0x08AABAA0`, an existing READY flag allowed
  a request for a different track to become SEEK (event 2) on the current
  decoder; a preload-only request could be ignored altogether. Compare the
  requested ID with the active stream ID at `0x08BD5AE4` before either action.
  A different ID takes the existing open path at `0x08AABAC4`, with STOP|OPEN
  (events 4|1), so an in-progress old refill cannot bypass teardown. Same-ID
  seeks and preload no-ops retain the stock behavior. Requests while the
  initial busy flag is set retain the existing behavior; this is not a new
  generalized streaming scheduler or a change to custom-track filename lookup.
* Revision 6 tests replay READY transitions for radio/cutscene IDs, same-ID
  requests, preload-only calls, and a refill in progress. A negative control
  with the original identity-blind branch fails: "New cutscene request retained
  the previous audio source". Integration guards check both generated labels.
  `STREAM_REQUEST` logs current/requested IDs and event bits. User validation
  of the intermittent office transition is still required; the test proves
  this faulty branch, not that every wrong-track symptom has the same cause.

* Revision 5 handles render/audio command ordering, not SAS envelope timeouts.
  A later stop cancels an unconsumed producer start (`0x0880A6A4`), while
  STOP->START still retriggers. The VCS playing query (`0x0880A5FC`) considers
  pending starts in both command buffers, so an extra render update cannot
  observe a false EOF and drop ownership before KeyOn. Each consumer visit
  retires its start bit (`0x0880AE90`); stale completed batches never hide EOF.
  Tests exhaust all 510 start/stop sequences of lengths 1–8 and both pending
  buffer phases for each channel. A negative control using the original
  independent masks fails with "Audio command order restarted a stopped voice".
* Revision 5 carries fractional milliseconds for the two CTimer integer
  counters at `0x08A11358/0x08A113B0`. The confirmed reproduction loses about
  350 ms relative to animation time in an eight-second interval. Stock mode,
  time scale, pause, and root-motion code remain unchanged. External counter
  changes reset the carry; tests check 60 seconds at 30–240 FPS with <1 ms
  cumulative quantization error. This does not remove the host device queue
  or establish an exact visual lip-sync offset without another user test.
* The revision 4 reproduction reported `aborted_batches=0`: its abort drain
  did not explain the confirmed stuck skid. Revision 5 logs cancelled starts
  and protected pending owners independently of that older path.

* Revision 4 (based on `32f4b08`) preserves unvisited KeyOff commands when
  `0x0880AB48` abandons a command batch because a sample is unavailable. The
  stock exit clears the batch-ready flag without reaching the remaining
  channels. Stops in words `0x08BB4AA8/0x08BB4AAC` must be drained first.
  Tests inject a failure at each of the 28 channels, check the 24+4 bit
  packing, and verify unrelated and paused voices are unaffected. This fixes
  a lost-command path, not a verified reproduction of every reported loop.
* Revision 4 logs SAS lifecycle counters/masks once per guest second and
  ATRAC create/release/seek events, including decoder IDs. Game-clock evidence
  is enabled with Audio Diagnostics too. A fresh user reproduction is needed
  to determine whether the cutscene issue is an unreleased decoder, a seek on
  the previous stream, or output latency. No new cutscene timing adjustment,
  FPS limit, root-motion change, or timeout on legitimate loops is introduced.

* Revision 3 fixes the root-motion regression exposed by removing the timer
  floor. Ped code at `0x0891997C` multiplies displacement by **0.5** when
  timestep < 0.5, while cutscene-object code at `0x08A199DC` divides by **0.5**.
  Both must instead divide displacement by the actual timestep in unlocked
  mode. At 200 FPS the old ped path preserved only 12.5% of displacement and
  the cutscene-object path only 50%. Keep the clock fix, fix both consumers,
  preserve stock mode, and return a zero scale for a paused/zero-duration frame.
* Revision 3 also releases the SAS KeyOn latch when the envelope ends naturally,
  and advances the source clock even when envelope gain is zero. Tests cover
  a looping source with natural envelope completion and a silent finite source.
  These are reproducible mixer defects; they do not alone establish that every
  horn/skid loop reported in gameplay has the same cause.
* The VCS `CTimer::Update` code at guest `0x08A11438` imposed a minimum
  timestep of `0.5`. Its units are 1/50 second, so a render frame always advanced
  animation by at least 10 ms. At 200 FPS this advances animation at twice the
  sample clock's rate. Unlocked modes now retain the measured timestep. Stock
  30 FPS behavior, pause branches, time scale, and the maximum timestep remain
  unchanged. No audio sample rate or pitch is changed.
* `sceSasGetEndFlag` now refreshes its cached mask from current voice state.
  Previously a just-restarted voice could still report its preceding EOF until
  a Core mix occurred. Extra game updates between two mixer grains could thus
  observe an ended voice that was actually playing. The cached field is kept
  for checkpoint layout compatibility. This follows the live `playing` check in
  [PPSSPP's implementation](https://github.com/hrydgard/ppsspp/blob/master/Core/HLE/sceSas.cpp).

The generated CTimer label uses a local jump; registering a host override for
that address does not intercept it. Consequently the fix is in generated unit
0131, with its arithmetic in `host/vcs_frame_timing.hpp`. A regression test also
checks that regeneration has not removed the call at the actual label. Preserve
this patch when regenerating the VCS code. The root-motion consumer patches in
units 0069 and 0133 must be preserved together with the timer patch.

## Verification and diagnostics

`vcs_profile_tests`, `audio_resampler_tests`, `audio_output_timeline_tests`, and
`vcs_frame_timing_tests` pass. Coverage includes KeyOn / repeated GetEndFlag /
Core / EOF / retrigger, 30–240 FPS clock accumulation over one minute, mixed
3/12 ms frames, and the production host audio queue with a fake waveOut device.

`ATRAC_TIMELINE` records source samples versus guest microseconds once per source
second. Audio Diagnostics (or `PSPRECOMP_GAME_TIMING_DIAG=1`) adds deduplicated
render-frame animation time and the guest game clock. `AUDIO_TIMING_REVISION=5`
identifies the user-tested build described above.

These tests do not prove that every looping in-game sound is fixed. The user
has now reported success for the gameplay loop reproduction; cutscene source
selection and subjective lip sync remain separate, unresolved validation items.

During isolated automated runs, sending Cross+Start to skip TITLES reproduced
an access violation on BOTH the pre-change executable and the patched one:
guest dispatch `0x08AC43BC`, write to host fastmem offset `0x04800000`. This is
not evidence of a regression from these changes. The regular user's logs,
configuration, and saves were not overwritten by these isolated runs.

#pragma once

// Generic PSP audio HLE shared by title profiles: the eight sceAudio PCM
// channels plus the SRC/Output2 channel, paced on the virtual-time line the
// way the hardware DAC drains queued buffers, and the sceSasCore software
// mixer (VAG/noise voices, ADSR envelopes, reverb send). Host playback is the
// profile's job, reached through AudioHooks; without hooks audio is silent but
// keeps hardware timing.

#include "psprecomp/allegrex_context.hpp"
#include "psprecomp/runtime.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace psprecomp::hle {

struct AudioChannelState {
    bool reserved{};
    std::uint32_t sample_count{};
    std::uint32_t format{};
    std::uint32_t left_volume{};
    std::uint32_t right_volume{};
    std::uint64_t busy_until_us{};
    // sceAudioSRCChReserve picks these per channel. They used to be discarded,
    // so a 22050 Hz talk-radio stream was played as if it were 44100 and came
    // out at double speed.
    std::uint32_t frequency{44100u};
    std::uint32_t channel_count{2u};

    // Hardware pacing anchor.  The DAC consumes queued buffers back to back, so
    // the start time of buffer N is the end time of buffer N-1 -- never "now
    // plus one buffer".  Deriving it from an accumulated frame count keeps the
    // channel exactly on the 44100 Hz grid instead of charging the guest's own
    // decode time to the audio timeline.
    bool queue_active{};
    std::uint64_t queue_anchor_us{};
    std::uint64_t queued_frames{};
};

enum class SasVoiceType : std::uint8_t {
    Off,
    Vag,
    Noise,
};

enum class SasEnvelopePhase : std::uint8_t {
    Attack,
    Decay,
    Sustain,
    Release,
    Off,
};

struct SasVoiceState {
    SasVoiceType type{SasVoiceType::Off};
    std::uint32_t data_address{};
    std::int32_t data_size{};
    bool loop{};
    std::int32_t noise_frequency{};
    std::int32_t pitch{0x1000};
    std::int32_t left_volume{};
    std::int32_t right_volume{};
    std::int32_t effect_left_volume{};
    std::int32_t effect_right_volume{};
    std::array<std::int32_t, 4> adsr_rates{};
    // Attack rises, everything else falls -- the same parity rule that
    // __sceSasSetADSRmode enforces on the game (even for attack, odd for decay
    // and release).  Defaulting all four to zero broke that rule: zero is
    // "linear increase", so a voice configured through __sceSasSetADSR alone,
    // which sets rates and never touches the modes, walked its release phase
    // *upward*.  The envelope pinned at maximum, `height <= 0` never happened,
    // and a keyed-off looping voice -- the vehicle engine -- kept sounding at
    // full volume under the pause menu.
    std::array<std::int32_t, 4> adsr_modes{0, 1, 1, 1};
    std::int32_t sustain_level{};
    std::uint32_t simple_adsr1{};
    std::uint32_t simple_adsr2{};
    bool adsr_configured{};
    SasEnvelopePhase envelope_phase{SasEnvelopePhase::Off};
    std::uint32_t key_on_delay_samples{};
    bool on{};
    bool playing{};
    bool paused{};
    std::uint32_t envelope_height{};
    std::uint64_t total_samples{};
    std::uint64_t remaining_samples{};

    // Stateful VAG decoder.  A voice is frequently re-used by the game, so a
    // KeyOn must rewind all of these fields rather than resuming at the end of
    // the previous sound.
    std::uint32_t decode_offset{};
    std::int32_t history1{};
    std::int32_t history2{};
    std::array<std::int16_t, 28> block_samples{};
    std::uint32_t block_position{28u};
    std::uint32_t loop_start_offset{};
    std::int32_t loop_start_history1{};
    std::int32_t loop_start_history2{};
    bool loop_start_valid{};
    bool finished{};

    // Pitch interpolation keeps a source sample pair alive across grain
    // boundaries.  The previous nearest-neighbour stepping clicked badly on
    // pitched engine/weapon/ambient effects.
    std::int16_t current_sample{};
    std::int16_t next_sample{};
    bool current_sample_valid{};
    bool next_sample_valid{};
    std::uint32_t pitch_accumulator{}; // 12-bit fraction, 0x1000 == one sample

    // Deterministic noise generator for the SAS noise-voice path.
    std::uint32_t noise_lfsr{0x13579BDFu};
    std::uint32_t noise_phase{};
    std::int16_t noise_sample{};
};
struct SasReverbState {
    std::int32_t type{-1};
    std::int32_t delay{};
    std::int32_t feedback{};
    std::uint32_t left_volume{};
    std::uint32_t right_volume{};
    // PSP SAS starts with the dry bus enabled.  Wet processing is opt-in.
    bool dry{true};
    bool wet{};
    // Persistent effect history.  This is intentionally owned by the SAS core
    // rather than rebuilt per grain so effect-only voices do not disappear at
    // grain boundaries and delay tails remain continuous.
    std::vector<std::int32_t> history_left;
    std::vector<std::int32_t> history_right;
    std::size_t history_cursor{};
};

struct SasState {
    bool initialized{};
    std::uint32_t core_address{};
    std::uint32_t grain_size{};
    std::uint32_t max_voices{32u};
    std::uint32_t output_mode{};
    std::uint32_t sample_rate{44100u};
    std::array<SasVoiceState, 32> voices{};
    SasReverbState reverb{};
};

inline constexpr std::uint32_t kSasErrorInvalidGrain = 0x80420001u;
inline constexpr std::uint32_t kSasErrorInvalidMaxVoices = 0x80420002u;
inline constexpr std::uint32_t kSasErrorInvalidOutputMode = 0x80420003u;
inline constexpr std::uint32_t kSasErrorInvalidSampleRate = 0x80420004u;
inline constexpr std::uint32_t kSasErrorBadAddress = 0x80420005u;
inline constexpr std::uint32_t kSasErrorInvalidVoice = 0x80420010u;
inline constexpr std::uint32_t kSasErrorInvalidNoiseFrequency = 0x80420011u;
inline constexpr std::uint32_t kSasErrorInvalidPitch = 0x80420012u;
inline constexpr std::uint32_t kSasErrorInvalidAdsrMode = 0x80420013u;
inline constexpr std::uint32_t kSasErrorInvalidParameter = 0x80420014u;
inline constexpr std::uint32_t kSasErrorInvalidLoop = 0x80420015u;
inline constexpr std::uint32_t kSasErrorVoicePaused = 0x80420016u;
inline constexpr std::uint32_t kSasErrorInvalidVolume = 0x80420018u;
inline constexpr std::uint32_t kSasErrorInvalidAdsrRate = 0x80420019u;
inline constexpr std::uint32_t kSasErrorReverbType = 0x80420020u;
inline constexpr std::uint32_t kSasErrorReverbFeedback = 0x80420021u;
inline constexpr std::uint32_t kSasErrorReverbDelay = 0x80420022u;
inline constexpr std::uint32_t kSasErrorReverbVolume = 0x80420023u;
inline constexpr std::uint32_t kSasErrorNotInitialized = 0x80420100u;
inline constexpr std::uint32_t kSasEnvelopeMaximum = 0x40000000u;

struct AudioHooks {
    bool (*enabled)() = nullptr;
    // Mix one submitted PSP buffer starting at guest_time_us into the host stream.
    void (*submit)(std::span<const std::int16_t> pcm, std::uint32_t frames, bool stereo,
                   std::uint32_t left, std::uint32_t right, std::uint32_t source_rate,
                   std::uint32_t channel, std::uint64_t guest_time_us) = nullptr;
    // Forget stream continuity for one channel (release / re-reserve).
    void (*reset_channel)(std::uint32_t channel) = nullptr;
};

extern AudioHooks g_audio_hooks;
extern std::array<AudioChannelState, 9> audio_channels;
extern SasState sas_state;
extern std::uint64_t sas_core_mix_calls;
extern std::uint64_t sas_core_with_mix_calls;

bool sas_audio_diagnostics_enabled();
std::size_t sas_playing_voice_count();
void sas_log_mix_checkpoint(const char *kind, std::uint64_t count);
bool sas_valid_core(std::uint32_t core) noexcept;
SasVoiceState *sas_voice(std::uint32_t core, std::int32_t voice, psprecomp::AllegrexContext &ctx);
void sas_reset_voice_duration(SasVoiceState &voice) noexcept;
void sas_reset_decoder(SasVoiceState &voice) noexcept;
std::int64_t sas_walk_envelope_curve(std::int64_t height, std::int32_t mode,
                                     std::int32_t rate) noexcept;
std::uint32_t sas_step_envelope(SasVoiceState &voice) noexcept;
std::int32_t sas_simple_rate(std::uint32_t value) noexcept;
std::int32_t sas_exponent_rate(std::uint32_t value) noexcept;
void sas_decode_simple_adsr(SasVoiceState &voice) noexcept;
bool sas_decode_next_block(const psprecomp::GuestMemory &memory, SasVoiceState &voice);
bool sas_fetch_vag_sample(const psprecomp::GuestMemory &memory, SasVoiceState &voice,
                          std::int16_t &sample);
bool sas_prepare_sample_pair(const psprecomp::GuestMemory &memory, SasVoiceState &voice);
std::int32_t sas_render_vag_sample(const psprecomp::GuestMemory &memory, SasVoiceState &voice);
std::int32_t sas_render_noise_sample(SasVoiceState &voice) noexcept;
void sas_render_voice(const psprecomp::GuestMemory &memory, SasVoiceState &voice,
                      std::vector<std::int32_t> &dry_mix,
                      std::vector<std::int32_t> &effect_send,
                      std::uint32_t frames);
void sas_render_buses(const psprecomp::GuestMemory &memory, std::uint32_t frames,
                      std::vector<std::int32_t> &dry_mix,
                      std::vector<std::int32_t> &effect_send);
void sas_process_effect_send(const std::vector<std::int32_t> &effect_send,
                             std::vector<std::int32_t> &wet_mix,
                             std::uint32_t frames);
void sas_mix_into(psprecomp::Runtime &rt, std::uint32_t output, std::uint32_t frames,
                  bool include_input = false,
                  std::uint32_t input_left = 0x1000u,
                  std::uint32_t input_right = 0x1000u);
void sas_mix_raw(psprecomp::Runtime &rt, std::uint32_t output, std::uint32_t frames);
std::uint32_t audio_remaining_samples(const AudioChannelState &channel);
std::uint32_t audio_buffer_duration_us(std::uint32_t samples);
std::uint64_t audio_queue_buffer(AudioChannelState &channel, std::uint32_t frames);

void reset_audio(const AudioHooks &hooks = {});
void install_audio_hle(Runtime &runtime);

} // namespace psprecomp::hle

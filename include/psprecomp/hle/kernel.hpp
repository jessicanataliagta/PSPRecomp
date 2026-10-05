#pragma once

// Generic PSP kernel HLE shared by title profiles: threads and the cooperative
// scheduler, guest callbacks and interrupt returns, semaphores, event flags,
// fixed pools, user partition memory and the matching ThreadManForUser /
// SysMemUserForUser / sceSuspendForUser / UtilsForUser / InterruptManager imports.
//
// State is process-global (one guest per process), matching how the runtime's
// HLE hooks are installed. Profiles reset it with reset_kernel() and add
// title-specific behaviour through KernelHooks.

#include "psprecomp/allegrex_context.hpp"
#include "psprecomp/runtime.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace psprecomp::hle {

enum class ThreadState {
    Created,
    Ready,
    Running,
    Sleeping,
    Delayed,
    IoDeferred,
    Completed,
};

struct ThreadRecord {
    std::string name;
    std::uint32_t entry{};
    std::uint32_t priority{};
    std::uint32_t stack_size{};
    std::uint32_t attributes{};
    std::uint32_t stack_top{};
    std::uint32_t stack_bottom{};
    std::uint32_t kernel_context{};
    ThreadState state{ThreadState::Created};
    std::uint32_t exit_status{};
    bool externally_suspended{};
    psprecomp::AllegrexContext suspended_context{};
    std::uint32_t wakeup_count{};
    std::uint64_t delay_until_us{};
    std::uint64_t delay_sequence{};
    // Wait with a timeout (only armed when honor_wait_timeouts); the deadline
    // is delay_until_us. Kind: 1 semaphore, 2 thread end, 3 event flag.
    bool timed_wait{};
    std::uint8_t timed_wait_kind{};
    std::int32_t timed_wait_object{};
    std::uint32_t timed_wait_address{};
};

struct ThreadContinuation {
    std::int32_t uid{};
    psprecomp::AllegrexContext context{};
    std::uint64_t ready_sequence{};
};

struct FreeThreadStack {
    std::uint32_t bottom{};
    std::uint32_t top{};
};

struct PartitionBlock {
    std::string name;
    std::uint32_t address{};
    std::uint32_t size{};
};

struct PartitionTable {
    std::int32_t next_uid{0x100};
    std::uint32_t next_address{};
    std::unordered_map<std::int32_t, PartitionBlock> blocks;
};

struct CallbackRecord {
    std::string name;
    std::uint32_t function{};
    std::uint32_t common{};
    std::int32_t owner_uid{};
    std::uint32_t notify_count{};
    std::uint32_t notify_argument{};
};

struct CallbackTable {
    std::int32_t next_uid{0x200};
    std::unordered_map<std::int32_t, CallbackRecord> callbacks;
};

struct SemaphoreWaiter {
    std::int32_t uid{};
    psprecomp::AllegrexContext context{};
    std::int32_t requested{};
};

struct SemaphoreRecord {
    std::string name;
    std::int32_t count{};
    std::int32_t maximum{};
    std::vector<SemaphoreWaiter> waiters;
};

struct SemaphoreTable {
    std::int32_t next_uid{0x300};
    std::unordered_map<std::int32_t, SemaphoreRecord> semaphores;
};

struct EventFlagWaiter {
    std::int32_t uid{};
    psprecomp::AllegrexContext context{};
    std::uint32_t requested{};
    std::uint32_t mode{};
    std::uint32_t output_address{};
};

struct EventFlagRecord {
    std::string name;
    std::uint32_t attributes{};
    std::uint32_t initial_pattern{};
    std::uint32_t current_pattern{};
    std::vector<EventFlagWaiter> waiters;
};

struct EventFlagTable {
    std::int32_t next_uid{0x600};
    std::unordered_map<std::int32_t, EventFlagRecord> flags;
};

struct MutexWaiter {
    std::int32_t uid{};
    AllegrexContext context{};
    std::int32_t count{};
};

struct MutexRecord {
    std::string name;
    std::uint32_t attributes{};
    std::int32_t owner_uid{-1};
    std::int32_t lock_count{};
    std::vector<MutexWaiter> waiters;
};

struct MutexTable {
    std::int32_t next_uid{0x700};
    std::unordered_map<std::int32_t, MutexRecord> mutexes;
};

struct FixedPoolRecord {
    std::string name;
    std::uint32_t address{};
    std::uint32_t block_size{};
    std::uint32_t block_count{};
    std::vector<bool> allocated;
};

struct FixedPoolTable {
    std::int32_t next_uid{0x500};
    std::unordered_map<std::int32_t, FixedPoolRecord> pools;
};

struct ThreadTable {
    std::int32_t next_uid{1};
    std::int32_t current_uid{0};
    // PSP user RAM ends at 0x0A000000.  User thread stacks are allocated
    // downward from the real partition top with 256-byte granularity.
    std::uint32_t next_stack_top{0x0A000000u};
    std::uint64_t next_ready_sequence{1u};
    std::uint64_t next_delay_sequence{1u};
    std::unordered_map<std::int32_t, ThreadRecord> threads;
    std::vector<ThreadContinuation> continuations;
    std::unordered_map<std::int32_t, std::vector<ThreadContinuation>> thread_end_waiters;
    std::vector<FreeThreadStack> free_stacks;
};

struct GuestCallbackInvocation {
    std::uint32_t function{};
    std::uint32_t a0{};
    std::uint32_t a1{};
    std::uint32_t a2{};
};

enum class AsyncReturnKind : std::uint8_t {
    GeCallbackChain,
    SubInterrupt,
    MpegRingbuffer,
    UserCallback,
};

struct AsyncReturnFrame {
    AsyncReturnKind kind{AsyncReturnKind::GeCallbackChain};
    psprecomp::AllegrexContext resume{};
    std::uint32_t ring_address{};
    std::int32_t remaining_packets{};
    std::int32_t requested_this_round{};
    std::int32_t total_packets{};
    std::int32_t callback_uid{};
};

struct SubInterruptRecord {
    std::uint32_t handler{};
    std::uint32_t argument{};
    bool enabled{};
    bool occurred{};
};

struct KernelHooks {
    // A thread left the scheduler for good (exit, terminate, delete or return).
    void (*thread_removed)(std::int32_t uid) = nullptr;
    // Resumes an async-return frame kind owned by the profile (for example an
    // MPEG ringbuffer callback). Returns true when it re-entered guest code.
    bool (*continue_async_frame)(Runtime &runtime, AllegrexContext &ctx, AsyncReturnFrame &frame) = nullptr;
    // Appends profile state to the PSPRECOMP_EVENT_DIAG stall dump.
    void (*dump_event_stall_extra)() = nullptr;
};

extern KernelHooks g_kernel_hooks;
// Lower bound applied to sceKernelDelayThread(CB) requests. On hardware a delay,
// even of 0, waits for a scheduler tick and lets lower-priority threads run;
// with 0 here a short delay resumes immediately if the caller is still the
// highest-priority ready thread. Not reset by reset_kernel().
extern std::uint32_t minimum_thread_delay_us;
// When set, registering a power, UMD or memory-stick status callback notifies
// it at once with the current (always present / powered) state, as the PSP
// kernel does. Off by default, which keeps those registrations silent.
extern bool notify_device_callbacks;
// When set, sceKernelWaitSema(CB) honours its timeout argument: the wait ends
// with SCE_KERNEL_ERROR_WAIT_TIMEOUT once it expires, and the remaining time is
// written back on success. Off by default (waits never time out).
extern bool honor_wait_timeouts;
// When set, sceKernelExitDeleteThread and the end of module_start free the
// thread's stack, as the PSP kernel does, so later sceKernelMaxFreeMemSize /
// stack allocations see that memory. Off by default (stacks of exited threads
// stay reserved).
extern bool release_deleted_thread_stacks;
// Arms a timeout for the wait the current thread is about to enter on `object`
// of `kind` (1 semaphore, 2 thread end, 3 event flag), reading the microsecond
// budget from guest `timeout_address`. No-op unless honor_wait_timeouts.
void arm_wait_timeout(Runtime &runtime, std::uint8_t kind, std::int32_t object, std::uint32_t timeout_address);

extern ThreadTable thread_table;
extern PartitionTable partition_table;
extern CallbackTable callback_table;
extern SemaphoreTable semaphore_table;
extern EventFlagTable event_flag_table;
extern FixedPoolTable fixed_pool_table;
extern MutexTable mutex_table;
extern std::uint32_t compiled_sdk_version;
extern std::uint32_t compiler_version;
extern std::uint64_t virtual_time_us;
extern bool volatile_memory_locked;
extern std::uint32_t general_purpose_io;
extern std::unordered_map<std::int32_t, std::vector<GuestCallbackInvocation>> pending_guest_callbacks;
extern std::unordered_map<std::int32_t, std::vector<AsyncReturnFrame>> async_return_frames;
extern std::unordered_map<std::uint64_t, SubInterruptRecord> sub_interrupts;
extern std::uint64_t display_vblank_index;
extern std::uint64_t event_diag_poll_count;
extern std::uint64_t event_diag_stop_polls;
extern bool event_diag_stall_reported;

inline void set_success(AllegrexContext &ctx) { ctx.set_gpr(2, 0u); }

std::uint64_t sub_interrupt_key(std::uint32_t interrupt_number, std::uint32_t sub_number);
std::uint64_t system_time_microseconds();

void enqueue_continuation(std::int32_t uid, const AllegrexContext &context);
std::uint32_t thread_priority(std::int32_t uid);
std::vector<ThreadContinuation>::iterator best_ready_thread();
bool preempt_if_higher_priority(AllegrexContext &ctx, const char *reason);
void promote_expired_delays();
bool activate_next_thread(AllegrexContext &ctx, const char *reason);
bool yield_current_thread(AllegrexContext &ctx);
AllegrexContext make_wait_context(const AllegrexContext &ctx);
bool delay_current_thread(Runtime &runtime, AllegrexContext &ctx,
                          std::uint32_t delay_microseconds, std::uint32_t return_value = 0u);
bool suspend_current_thread(Runtime &runtime, AllegrexContext &ctx,
                            const AllegrexContext &suspended, const std::string &reason);
bool sleep_current_thread(Runtime &runtime, AllegrexContext &ctx);
std::uint32_t wake_thread(std::int32_t uid);
void release_thread_stack(const ThreadRecord &thread);
bool allocate_thread_stack(std::uint32_t stack_size, std::uint32_t &bottom, std::uint32_t &top);
void remove_thread_from_wait_queues(std::int32_t uid);
void wake_thread_end_waiters(std::int32_t completed_uid, std::uint32_t result = 0u);

bool start_next_guest_callback(AllegrexContext &ctx, bool begin_chain);
bool maybe_start_pending_guest_callback(AllegrexContext &ctx);
void queue_guest_callback_chain(AllegrexContext &ctx, const AllegrexContext &resume,
                                std::vector<GuestCallbackInvocation> callbacks);

// Marks a callback notified (sceKernelNotifyCallback semantics); false for an unknown uid.
bool notify_callback(std::int32_t uid, std::uint32_t argument);
// Enters the current thread's first notified callback, returning to `resume`
// through the interrupt-return trampoline. False when none is pending or the
// thread is already inside a callback/interrupt.
bool start_pending_user_callback(AllegrexContext &ctx, const AllegrexContext &resume);
void complete_current_thread(Runtime &runtime, AllegrexContext &ctx);
// Registered at guest PC 0 (thread return) and 4 (interrupt/callback return).
void psp_thread_return(Runtime &runtime, AllegrexContext &ctx);
void psp_interrupt_return(Runtime &runtime, AllegrexContext &ctx);
void psp_interrupt_park(Runtime &runtime, AllegrexContext &ctx);

const char *thread_state_name(ThreadState state);
bool event_diag_matches(const EventFlagRecord &flag);
void dump_event_stall_state(const EventFlagRecord &flag, std::int32_t flag_uid,
                            const AllegrexContext &ctx);
bool event_flag_matches(const EventFlagRecord &flag, std::uint32_t requested, std::uint32_t mode);
void consume_event_flag(EventFlagRecord &flag, std::uint32_t requested, std::uint32_t mode);

// Resets all kernel state, creates the module_start thread (uid 0) with its
// stack at the top of user RAM, points the CPU at it and registers the thread /
// interrupt return trampolines. User partition allocations start at
// user_arena_start.
void reset_kernel(Runtime &runtime, std::uint32_t user_arena_start, const KernelHooks &hooks = {});
// Execution-driven virtual time: every dispatch_interval outer dispatches,
// advance virtual time by interval/4 us, wake expired delays and preempt the
// running thread for a higher-priority ready one. Without it a guest thread
// that spins without kernel calls freezes PSP time (no vblanks, no timers).
// 0 disables the clock.
void install_execution_clock(std::uint64_t dispatch_interval = 256u);
std::uint64_t execution_clock_tick_microseconds();
// Periodic sub-interrupt (e.g. vblank, 30/15): every period_us of PSP time the
// registered, enabled handler runs on a kernel interrupt context (a
// highest-priority pseudo-thread that parks itself through guest PC 8), whether
// or not any thread is waiting for it. period_us == 0 disables it.
void set_periodic_sub_interrupt(std::uint32_t interrupt, std::uint32_t sub, std::uint64_t period_us);
[[nodiscard]] bool periodic_sub_interrupt_active() noexcept;
// Registers the kernel imports listed above.
void install_kernel_hle(Runtime &runtime);

} // namespace psprecomp::hle

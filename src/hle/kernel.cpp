#include "psprecomp/hle/kernel.hpp"

#include "psprecomp/common.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// Moved verbatim from the VCS profile host (profiles/vcs/host/vcs_profile.cpp);
// title-specific behaviour is reached through g_kernel_hooks.

namespace psprecomp::hle {
namespace {

std::uint64_t parse_environment_u64(const char *name, std::uint64_t fallback = 0u) {
    const char *text = std::getenv(name);
    if (text == nullptr || *text == '\0') return fallback;
    char *end = nullptr;
    const unsigned long long value = std::strtoull(text, &end, 0);
    return end != text && *end == '\0' ? static_cast<std::uint64_t>(value) : fallback;
}

void notify_thread_removed(std::int32_t uid) {
    if (g_kernel_hooks.thread_removed != nullptr) g_kernel_hooks.thread_removed(uid);
}

} // namespace

KernelHooks g_kernel_hooks{};
// Runtime whose memory receives timed-wait results; set by install_kernel_hle.
psprecomp::Runtime *g_runtime_for_timeouts{};

namespace {

struct PeriodicInterrupt {
    std::uint32_t interrupt{};
    std::uint32_t sub{};
    std::uint64_t period_us{};
    std::uint64_t next_us{};
};
PeriodicInterrupt periodic_interrupt{};
// sceKernelCpuSuspendIntr critical section: interrupts and timer preemption wait.
bool interrupts_suspended{};
constexpr std::int32_t kInterruptThreadUid = 0x7FFF0000;
constexpr std::uint32_t kInterruptStackSize = 0x2000u;

const SubInterruptRecord *periodic_handler() {
    if (periodic_interrupt.period_us == 0u) return nullptr;
    const auto found = sub_interrupts.find(sub_interrupt_key(periodic_interrupt.interrupt, periodic_interrupt.sub));
    if (found == sub_interrupts.end() || !found->second.enabled || found->second.handler == 0u) return nullptr;
    return &found->second;
}

ThreadRecord *interrupt_thread() {
    auto found = thread_table.threads.find(kInterruptThreadUid);
    if (found != thread_table.threads.end()) return &found->second;
    std::uint32_t bottom = 0u, top = 0u;
    if (!allocate_thread_stack(kInterruptStackSize, bottom, top)) return nullptr;
    ThreadRecord record{"interrupt", 0u, 0u, kInterruptStackSize, 0u};
    record.stack_bottom = bottom;
    record.stack_top = top;
    record.state = ThreadState::Sleeping;
    return &thread_table.threads.emplace(kInterruptThreadUid, std::move(record)).first->second;
}

// Queues the periodic handler on the interrupt context once its deadline has
// passed. Missed periods coalesce into one delivery, like a pending interrupt.
bool deliver_due_periodic_interrupt(const psprecomp::AllegrexContext &template_ctx) {
    if (interrupts_suspended) return false;  // stays pending until sceKernelCpuResumeIntr
    if (periodic_interrupt.period_us == 0u || virtual_time_us < periodic_interrupt.next_us) return false;
    while (periodic_interrupt.next_us <= virtual_time_us) periodic_interrupt.next_us += periodic_interrupt.period_us;
    const SubInterruptRecord *handler = periodic_handler();
    static const bool diag = std::getenv("PSPRECOMP_INTR_DIAG") != nullptr;
    if (diag) std::cerr << "[intr] periodic due t=" << virtual_time_us << " handler=" << (handler != nullptr) << "\n";
    if (handler == nullptr) return false;
    ThreadRecord *thread = interrupt_thread();
    if (diag) std::cerr << "[intr] thread=" << (thread != nullptr) << " state="
                        << (thread != nullptr ? thread_state_name(thread->state) : "-") << "\n";
    if (thread == nullptr || thread->state != ThreadState::Sleeping) return false;
    psprecomp::AllegrexContext entry{};
    entry.set_gpr(28, template_ctx.gpr[28]);
    entry.set_gpr(29, thread->stack_top - 0x40u);
    entry.set_gpr(4, periodic_interrupt.sub);
    entry.set_gpr(5, handler->argument);
    entry.set_gpr(31, 0x00000004u);
    entry.pc = handler->handler;
    psprecomp::AllegrexContext park{};
    park.pc = 0x00000008u;
    async_return_frames[kInterruptThreadUid].push_back(AsyncReturnFrame{AsyncReturnKind::SubInterrupt, park});
    sub_interrupts.at(sub_interrupt_key(periodic_interrupt.interrupt, periodic_interrupt.sub)).occurred = true;
    enqueue_continuation(kInterruptThreadUid, entry);
    return true;
}

} // namespace

void set_periodic_sub_interrupt(std::uint32_t interrupt, std::uint32_t sub, std::uint64_t period_us) {
    periodic_interrupt = PeriodicInterrupt{interrupt, sub, period_us, virtual_time_us + period_us};
}

bool periodic_sub_interrupt_active() noexcept { return periodic_interrupt.period_us != 0u; }
std::uint32_t minimum_thread_delay_us{};
bool notify_device_callbacks{};
bool honor_wait_timeouts{};
bool release_deleted_thread_stacks{};


std::uint64_t event_diag_poll_count{};
std::uint64_t event_diag_stop_polls{};
bool event_diag_stall_reported{};


ThreadTable thread_table;
PartitionTable partition_table;
CallbackTable callback_table;
SemaphoreTable semaphore_table;
EventFlagTable event_flag_table;
FixedPoolTable fixed_pool_table;
MutexTable mutex_table;
std::uint32_t compiled_sdk_version{};
std::uint32_t compiler_version{};

std::uint64_t virtual_time_us{};

bool volatile_memory_locked{};
std::uint32_t general_purpose_io{};


std::unordered_map<std::int32_t, std::vector<GuestCallbackInvocation>> pending_guest_callbacks;


std::unordered_map<std::int32_t, std::vector<AsyncReturnFrame>> async_return_frames;

std::unordered_map<std::uint64_t, SubInterruptRecord> sub_interrupts;
std::uint64_t sub_interrupt_key(std::uint32_t interrupt_number, std::uint32_t sub_number) {
    return (static_cast<std::uint64_t>(interrupt_number) << 32u) | sub_number;
}

std::uint64_t display_vblank_index{};

std::uint64_t system_time_microseconds() {
    return virtual_time_us;
}

void enqueue_continuation(std::int32_t uid, const psprecomp::AllegrexContext &context) {
    auto thread = thread_table.threads.find(uid);
    if (thread != thread_table.threads.end() && thread->second.timed_wait) {
        // Woken before its timeout: report the unused budget, as the PSP does.
        thread->second.timed_wait = false;
        const std::uint64_t remaining = thread->second.delay_until_us > virtual_time_us
            ? thread->second.delay_until_us - virtual_time_us : 0u;
        if (g_runtime_for_timeouts != nullptr &&
            g_runtime_for_timeouts->memory().contains(thread->second.timed_wait_address, 4u))
            g_runtime_for_timeouts->memory().store32(thread->second.timed_wait_address,
                                                     static_cast<std::uint32_t>(remaining));
    }
    if (thread != thread_table.threads.end()) {
        thread->second.state = ThreadState::Ready;
        thread->second.suspended_context = context;
        if (thread->second.externally_suspended) {
            thread_table.continuations.erase(
                std::remove_if(thread_table.continuations.begin(), thread_table.continuations.end(),
                               [uid](const ThreadContinuation &item) { return item.uid == uid; }),
                thread_table.continuations.end());
            return;
        }
    }
    const auto existing = std::find_if(
        thread_table.continuations.begin(), thread_table.continuations.end(),
        [uid](const ThreadContinuation &item) { return item.uid == uid; });
    if (existing != thread_table.continuations.end()) {
        existing->context = context;
    } else {
        thread_table.continuations.push_back(
            ThreadContinuation{uid, context, thread_table.next_ready_sequence++});
    }
}

std::uint32_t thread_priority(std::int32_t uid) {
    const auto found = thread_table.threads.find(uid);
    return found != thread_table.threads.end() ? found->second.priority : 0xFFFFFFFFu;
}

std::vector<ThreadContinuation>::iterator best_ready_thread() {
    return std::min_element(
        thread_table.continuations.begin(), thread_table.continuations.end(),
        [](const ThreadContinuation &left, const ThreadContinuation &right) {
            const std::uint32_t left_priority = thread_priority(left.uid);
            const std::uint32_t right_priority = thread_priority(right.uid);
            if (left_priority != right_priority) return left_priority < right_priority;
            return left.ready_sequence < right.ready_sequence;
        });
}

bool preempt_if_higher_priority(psprecomp::AllegrexContext &ctx, const char *reason) {
    const auto current = thread_table.threads.find(thread_table.current_uid);
    if (current == thread_table.threads.end() || current->second.state != ThreadState::Running) return false;
    const auto best = best_ready_thread();
    if (best == thread_table.continuations.end() ||
        thread_priority(best->uid) >= thread_priority(thread_table.current_uid)) {
        return false;
    }

    const std::int32_t caller_uid = thread_table.current_uid;
    const std::int32_t target_uid = best->uid;
    const std::uint32_t target_priority = thread_priority(target_uid);
    psprecomp::AllegrexContext caller = ctx;
    caller.pc = ctx.gpr[31];
    enqueue_continuation(caller_uid, caller);
    if (std::getenv("PSPRECOMP_SCHED_DIAG") != nullptr || std::getenv("PSPRECOMP_TRACE") != nullptr) {
        std::cerr << "[sched] preempt reason=" << reason
                  << " caller=" << caller_uid
                  << " caller_priority=" << thread_priority(caller_uid)
                  << " target=" << target_uid
                  << " target_priority=" << target_priority << "\n";
    }
    return activate_next_thread(ctx, reason);
}

void promote_expired_delays() {
    struct ExpiredDelay {
        std::int32_t uid{};
        std::uint64_t deadline{};
        std::uint64_t sequence{};
    };
    std::vector<ExpiredDelay> expired;
    expired.reserve(thread_table.threads.size());
    for (const auto &[uid, thread] : thread_table.threads) {
        if (thread.state == ThreadState::Delayed && thread.delay_until_us <= virtual_time_us)
            expired.push_back(ExpiredDelay{uid, thread.delay_until_us, thread.delay_sequence});
    }
    // unordered_map iteration must never decide PSP scheduling order.  Kernel
    // wakeups are replayed by deadline and by the order in which the waits were
    // armed, with UID only as a final total-order guard.
    std::sort(expired.begin(), expired.end(), [](const ExpiredDelay &left, const ExpiredDelay &right) {
        if (left.deadline != right.deadline) return left.deadline < right.deadline;
        if (left.sequence != right.sequence) return left.sequence < right.sequence;
        return left.uid < right.uid;
    });
    for (const ExpiredDelay &item : expired) {
        const auto thread = thread_table.threads.find(item.uid);
        if (thread != thread_table.threads.end()) enqueue_continuation(item.uid, thread->second.suspended_context);
    }

    // Timed semaphore waits whose deadline passed (honor_wait_timeouts only).
    expired.clear();
    for (const auto &[uid, thread] : thread_table.threads) {
        if (thread.timed_wait && thread.state == ThreadState::Sleeping && thread.delay_until_us <= virtual_time_us)
            expired.push_back(ExpiredDelay{uid, thread.delay_until_us, thread.delay_sequence});
    }
    std::sort(expired.begin(), expired.end(), [](const ExpiredDelay &left, const ExpiredDelay &right) {
        if (left.deadline != right.deadline) return left.deadline < right.deadline;
        if (left.sequence != right.sequence) return left.sequence < right.sequence;
        return left.uid < right.uid;
    });
    for (const ExpiredDelay &item : expired) {
        ThreadRecord &thread = thread_table.threads.at(item.uid);
        thread.timed_wait = false;
        const auto not_this = [&item](const auto &waiter) { return waiter.uid == item.uid; };
        if (thread.timed_wait_kind == 1u) {
            if (const auto found = semaphore_table.semaphores.find(thread.timed_wait_object);
                found != semaphore_table.semaphores.end())
                std::erase_if(found->second.waiters, not_this);
        } else if (thread.timed_wait_kind == 2u) {
            if (const auto found = thread_table.thread_end_waiters.find(thread.timed_wait_object);
                found != thread_table.thread_end_waiters.end())
                std::erase_if(found->second, not_this);
        } else if (thread.timed_wait_kind == 3u) {
            if (const auto found = event_flag_table.flags.find(thread.timed_wait_object);
                found != event_flag_table.flags.end())
                std::erase_if(found->second.waiters, not_this);
        }
        thread.suspended_context.set_gpr(2, 0x800201A8u);  // SCE_KERNEL_ERROR_WAIT_TIMEOUT
        if (g_runtime_for_timeouts != nullptr && thread.timed_wait_address != 0u &&
            g_runtime_for_timeouts->memory().contains(thread.timed_wait_address, 4u))
            g_runtime_for_timeouts->memory().store32(thread.timed_wait_address, 0u);
        enqueue_continuation(item.uid, thread.suspended_context);
    }
}

bool activate_next_thread(psprecomp::AllegrexContext &ctx, const char *reason) {
    const std::int32_t previous_uid = thread_table.current_uid;
    promote_expired_delays();
    (void)deliver_due_periodic_interrupt(ctx);
    thread_table.continuations.erase(
        std::remove_if(thread_table.continuations.begin(), thread_table.continuations.end(),
                       [](const ThreadContinuation &item) {
                           const auto thread = thread_table.threads.find(item.uid);
                           return thread == thread_table.threads.end() || thread->second.externally_suspended;
                       }),
        thread_table.continuations.end());
    if (thread_table.continuations.empty()) {
        std::uint64_t earliest = UINT64_MAX;
        for (const auto &[uid, thread] : thread_table.threads) {
            (void)uid;
            if (thread.state == ThreadState::Delayed || (thread.timed_wait && thread.state == ThreadState::Sleeping))
                earliest = std::min(earliest, thread.delay_until_us);
        }
        if (periodic_handler() != nullptr) earliest = std::min(earliest, periodic_interrupt.next_us);
        if (earliest != UINT64_MAX) {
            // The recomp runtime uses deterministic virtual PSP time.  When no
            // thread is runnable, advance directly to the next kernel wakeup.
            virtual_time_us = std::max(virtual_time_us, earliest);
            promote_expired_delays();
            (void)deliver_due_periodic_interrupt(ctx);
        }
    }
    if (thread_table.continuations.empty()) return false;

    // PSP priorities are inverted: a smaller numeric value means a higher
    // scheduling priority. Equal-priority threads keep explicit FIFO order.
    const auto selected = best_ready_thread();
    ThreadContinuation continuation = *selected;
    thread_table.continuations.erase(selected);
    thread_table.current_uid = continuation.uid;
    std::string thread_name = "unknown";
    if (auto thread = thread_table.threads.find(continuation.uid); thread != thread_table.threads.end()) {
        thread->second.state = ThreadState::Running;
        thread_name = thread->second.name;
    }
    ctx = continuation.context;
    psprecomp::set_runtime_thread_identity(continuation.uid, thread_name);

    if (std::getenv("PSPRECOMP_SCHED_DIAG") != nullptr || std::getenv("PSPRECOMP_TRACE") != nullptr) {
        const auto pending = pending_guest_callbacks.find(continuation.uid);
        const auto frames = async_return_frames.find(continuation.uid);
        std::cerr << "[sched] reason=" << reason
                  << " from_uid=" << previous_uid
                  << " to_uid=" << continuation.uid
                  << " name=" << thread_name
                  << " priority=" << thread_priority(continuation.uid)
                  << " pc=" << psprecomp::hex32(ctx.pc)
                  << " sp=" << psprecomp::hex32(ctx.gpr[29])
                  << " ra=" << psprecomp::hex32(ctx.gpr[31])
                  << " gp=" << psprecomp::hex32(ctx.gpr[28])
                  << " a0=" << psprecomp::hex32(ctx.gpr[4])
                  << " a1=" << psprecomp::hex32(ctx.gpr[5])
                  << " a2=" << psprecomp::hex32(ctx.gpr[6])
                  << " a3=" << psprecomp::hex32(ctx.gpr[7])
                  << " t0=" << psprecomp::hex32(ctx.gpr[8])
                  << " t1=" << psprecomp::hex32(ctx.gpr[9])
                  << " t2=" << psprecomp::hex32(ctx.gpr[10])
                  << " t3=" << psprecomp::hex32(ctx.gpr[11])
                  << " s0=" << psprecomp::hex32(ctx.gpr[16])
                  << " s1=" << psprecomp::hex32(ctx.gpr[17])
                  << " s2=" << psprecomp::hex32(ctx.gpr[18])
                  << " s3=" << psprecomp::hex32(ctx.gpr[19])
                  << " s4=" << psprecomp::hex32(ctx.gpr[20])
                  << " s5=" << psprecomp::hex32(ctx.gpr[21])
                  << " s6=" << psprecomp::hex32(ctx.gpr[22])
                  << " s7=" << psprecomp::hex32(ctx.gpr[23])
                  << " ready=" << thread_table.continuations.size()
                  << " pending_callbacks=" << (pending == pending_guest_callbacks.end() ? 0u : pending->second.size())
                  << " async_frames=" << (frames == async_return_frames.end() ? 0u : frames->second.size())
                  << "\n";
    }

    (void)maybe_start_pending_guest_callback(ctx);
    return true;
}

bool yield_current_thread(psprecomp::AllegrexContext &ctx) {
    psprecomp::AllegrexContext suspended = ctx;
    suspended.set_gpr(2, 0u);
    suspended.pc = ctx.gpr[31];
    enqueue_continuation(thread_table.current_uid, suspended);
    return activate_next_thread(ctx, "yield");
}

psprecomp::AllegrexContext make_wait_context(const psprecomp::AllegrexContext &ctx) {
    psprecomp::AllegrexContext suspended = ctx;
    suspended.set_gpr(2, 0u);
    suspended.pc = ctx.gpr[31];
    return suspended;
}

bool delay_current_thread(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx,
                          std::uint32_t delay_microseconds, std::uint32_t return_value) {
    auto current = thread_table.threads.find(thread_table.current_uid);
    if (current == thread_table.threads.end()) {
        ctx.set_gpr(2, 0x80020198u);
        return false;
    }
    psprecomp::AllegrexContext suspended = make_wait_context(ctx);
    suspended.set_gpr(2, return_value);
    current->second.state = ThreadState::Delayed;
    current->second.suspended_context = suspended;
    current->second.delay_until_us = virtual_time_us + delay_microseconds;
    current->second.delay_sequence = thread_table.next_delay_sequence++;
    if (std::getenv("PSPRECOMP_TRACE") != nullptr) {
        std::cerr << "[sched] delay uid=" << thread_table.current_uid
                  << " usec=" << delay_microseconds
                  << " resume=" << psprecomp::hex32(suspended.pc) << "\n";
    }
    if (!activate_next_thread(ctx, "delay")) {
        runtime.stop("PSP scheduler deadlock while delaying thread");
        return false;
    }
    return true;
}

bool suspend_current_thread(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx,
                            const psprecomp::AllegrexContext &suspended,
                            const std::string &reason) {
    if (auto current = thread_table.threads.find(thread_table.current_uid);
        current != thread_table.threads.end()) {
        current->second.state = ThreadState::Sleeping;
        current->second.suspended_context = suspended;
    }
    if (std::getenv("PSPRECOMP_TRACE") != nullptr) {
        const auto found = thread_table.threads.find(thread_table.current_uid);
        std::cerr << "[sched] block uid=" << thread_table.current_uid
                  << " name=" << (found != thread_table.threads.end() ? found->second.name : "unknown")
                  << " reason=" << reason << " resume=" << psprecomp::hex32(suspended.pc) << "\n";
    }
    if (!activate_next_thread(ctx, reason.c_str())) {
        runtime.stop("PSP scheduler deadlock while waiting for " + reason);
        return false;
    }
    return true;
}

bool sleep_current_thread(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx) {
    auto current = thread_table.threads.find(thread_table.current_uid);
    if (current == thread_table.threads.end()) {
        ctx.set_gpr(2, 0x80020198u);
        return false;
    }
    if (current->second.wakeup_count != 0u) {
        --current->second.wakeup_count;
        set_success(ctx);
        return true;
    }
    const psprecomp::AllegrexContext suspended = make_wait_context(ctx);
    current->second.state = ThreadState::Sleeping;
    current->second.suspended_context = suspended;
    if (!activate_next_thread(ctx, "sleep")) {
        runtime.stop("PSP scheduler deadlock: every thread is sleeping");
        return false;
    }
    return true;
}

std::uint32_t wake_thread(std::int32_t uid) {
    const auto found = thread_table.threads.find(uid);
    if (found == thread_table.threads.end()) return 0x80020198u;
    ThreadRecord &thread = found->second;
    if (thread.state == ThreadState::Completed || thread.state == ThreadState::Created)
        return 0x800201A2u;
    if (thread.state == ThreadState::Sleeping) {
        enqueue_continuation(uid, thread.suspended_context);
    } else {
        ++thread.wakeup_count;
    }
    return 0u;
}

void release_thread_stack(const ThreadRecord &thread) {
    if (thread.stack_bottom == 0u || thread.stack_top <= thread.stack_bottom) return;
    thread_table.free_stacks.push_back({thread.stack_bottom, thread.stack_top});
    std::sort(thread_table.free_stacks.begin(), thread_table.free_stacks.end(),
              [](const FreeThreadStack &left, const FreeThreadStack &right) {
                  return left.bottom < right.bottom;
              });
    std::vector<FreeThreadStack> merged;
    for (const FreeThreadStack block : thread_table.free_stacks) {
        if (!merged.empty() && block.bottom <= merged.back().top) {
            merged.back().top = std::max(merged.back().top, block.top);
        } else {
            merged.push_back(block);
        }
    }
    thread_table.free_stacks = std::move(merged);

    // Collapse any free block adjacent to the downward allocation frontier.
    for (;;) {
        const auto adjacent = std::find_if(thread_table.free_stacks.begin(), thread_table.free_stacks.end(),
            [](const FreeThreadStack &block) { return block.bottom == thread_table.next_stack_top; });
        if (adjacent == thread_table.free_stacks.end()) break;
        thread_table.next_stack_top = adjacent->top;
        thread_table.free_stacks.erase(adjacent);
    }
}

bool allocate_thread_stack(std::uint32_t stack_size, std::uint32_t &bottom, std::uint32_t &top) {
    // Reuse a deleted thread stack first. Allocate from the high end to retain
    // the PSP's top-down stack layout and leave any remainder reusable.
    auto best = thread_table.free_stacks.end();
    for (auto it = thread_table.free_stacks.begin(); it != thread_table.free_stacks.end(); ++it) {
        const std::uint32_t size = it->top - it->bottom;
        if (size < stack_size) continue;
        if (best == thread_table.free_stacks.end() || size < best->top - best->bottom) best = it;
    }
    if (best != thread_table.free_stacks.end()) {
        top = best->top;
        bottom = top - stack_size;
        if (bottom == best->bottom)
            thread_table.free_stacks.erase(best);
        else
            best->top = bottom;
        return true;
    }

    top = thread_table.next_stack_top & ~0xFFu;
    if (top < stack_size) return false;
    bottom = top - stack_size;
    if (bottom < partition_table.next_address) return false;
    thread_table.next_stack_top = bottom;
    return true;
}

void remove_thread_from_wait_queues(std::int32_t uid) {
    for (auto &[semaphore_uid, semaphore] : semaphore_table.semaphores) {
        (void)semaphore_uid;
        semaphore.waiters.erase(std::remove_if(semaphore.waiters.begin(), semaphore.waiters.end(),
            [uid](const SemaphoreWaiter &waiter) { return waiter.uid == uid; }), semaphore.waiters.end());
    }
    for (auto &[flag_uid, flag] : event_flag_table.flags) {
        (void)flag_uid;
        flag.waiters.erase(std::remove_if(flag.waiters.begin(), flag.waiters.end(),
            [uid](const EventFlagWaiter &waiter) { return waiter.uid == uid; }), flag.waiters.end());
    }
    for (auto &[mutex_uid, mutex] : mutex_table.mutexes) {
        (void)mutex_uid;
        mutex.waiters.erase(std::remove_if(mutex.waiters.begin(), mutex.waiters.end(),
            [uid](const MutexWaiter &waiter) { return waiter.uid == uid; }), mutex.waiters.end());
    }
    for (auto &[target_uid, waiters] : thread_table.thread_end_waiters) {
        (void)target_uid;
        waiters.erase(std::remove_if(waiters.begin(), waiters.end(),
            [uid](const ThreadContinuation &waiter) { return waiter.uid == uid; }), waiters.end());
    }
    std::erase_if(thread_table.thread_end_waiters,
                  [](const auto &entry) { return entry.second.empty(); });
    std::erase_if(callback_table.callbacks,
                  [uid](const auto &entry) { return entry.second.owner_uid == uid; });
}

void wake_thread_end_waiters(std::int32_t completed_uid, std::uint32_t result) {
    const auto found = thread_table.thread_end_waiters.find(completed_uid);
    if (found == thread_table.thread_end_waiters.end()) return;
    for (auto &waiter : found->second) {
        waiter.context.set_gpr(2, result);
        enqueue_continuation(waiter.uid, waiter.context);
    }
    thread_table.thread_end_waiters.erase(found);
}

bool start_next_guest_callback(psprecomp::AllegrexContext &ctx, bool begin_chain) {
    const std::int32_t uid = thread_table.current_uid;
    const auto found = pending_guest_callbacks.find(uid);
    if (found == pending_guest_callbacks.end() || found->second.empty()) return false;

    auto &frames = async_return_frames[uid];
    if (begin_chain) {
        // A GE callback is interrupt-like, but must never re-enter another guest
        // callback or a sub-interrupt already running on this thread.
        if (!frames.empty()) return false;
        frames.push_back(AsyncReturnFrame{AsyncReturnKind::GeCallbackChain, ctx});
    } else if (frames.empty() || frames.back().kind != AsyncReturnKind::GeCallbackChain) {
        return false;
    }

    const GuestCallbackInvocation invocation = found->second.front();
    found->second.erase(found->second.begin());
    if (found->second.empty()) pending_guest_callbacks.erase(found);
    ctx.set_gpr(4, invocation.a0);
    ctx.set_gpr(5, invocation.a1);
    ctx.set_gpr(6, invocation.a2);
    ctx.set_gpr(31, 0x00000004u);
    ctx.pc = invocation.function;
    if (std::getenv("PSPRECOMP_GE_DIAG") != nullptr || std::getenv("PSPRECOMP_SCHED_DIAG") != nullptr) {
        std::cerr << "[callback] start uid=" << uid
                  << " function=" << psprecomp::hex32(invocation.function)
                  << " a0=" << psprecomp::hex32(invocation.a0)
                  << " a1=" << psprecomp::hex32(invocation.a1)
                  << " a2=" << psprecomp::hex32(invocation.a2)
                  << " remaining=" << (pending_guest_callbacks.contains(uid) ? pending_guest_callbacks[uid].size() : 0u)
                  << "\n";
    }
    return true;
}

bool maybe_start_pending_guest_callback(psprecomp::AllegrexContext &ctx) {
    const auto thread = thread_table.threads.find(thread_table.current_uid);
    if (thread == thread_table.threads.end() || thread->second.state != ThreadState::Running) return false;
    const auto frames = async_return_frames.find(thread_table.current_uid);
    if (frames != async_return_frames.end() && !frames->second.empty()) return false;
    return start_next_guest_callback(ctx, true);
}

void queue_guest_callback_chain(psprecomp::AllegrexContext &ctx,
                                const psprecomp::AllegrexContext &resume,
                                std::vector<GuestCallbackInvocation> callbacks) {
    callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(), [](const GuestCallbackInvocation &item) {
        return item.function == 0u;
    }), callbacks.end());
    ctx = resume;
    if (callbacks.empty()) return;
    auto &pending = pending_guest_callbacks[thread_table.current_uid];
    pending.insert(pending.end(), callbacks.begin(), callbacks.end());
    if (std::getenv("PSPRECOMP_GE_DIAG") != nullptr || std::getenv("PSPRECOMP_SCHED_DIAG") != nullptr) {
        std::cerr << "[callback] queued uid=" << thread_table.current_uid
                  << " count=" << callbacks.size()
                  << " total=" << pending.size()
                  << " resume=" << psprecomp::hex32(resume.pc) << "\n";
    }
}

void complete_current_thread(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx) {
    const std::int32_t completed_uid = thread_table.current_uid;
    if (auto current = thread_table.threads.find(completed_uid); current != thread_table.threads.end())
        current->second.state = ThreadState::Completed;
    thread_table.continuations.erase(
        std::remove_if(thread_table.continuations.begin(), thread_table.continuations.end(),
                       [completed_uid](const ThreadContinuation &item) { return item.uid == completed_uid; }),
        thread_table.continuations.end());
    pending_guest_callbacks.erase(completed_uid);
    async_return_frames.erase(completed_uid);
    notify_thread_removed(completed_uid);
    wake_thread_end_waiters(completed_uid);
    if (!activate_next_thread(ctx, "thread-complete")) {
        ctx.set_gpr(2, 0u);
        runtime.stop("All PSP threads completed");
    }
}

namespace {

// The thread is gone for good (exit-delete, or the loader thread finishing):
// hand its stack back to the allocator.
void release_completed_thread_stack(std::int32_t uid) {
    const auto found = thread_table.threads.find(uid);
    if (!release_deleted_thread_stacks || found == thread_table.threads.end()) return;
    release_thread_stack(found->second);
    found->second.stack_bottom = found->second.stack_top = 0u;
}

} // namespace

// Guest PC 8: the interrupt context finished its handler and goes idle again.
void psp_interrupt_park(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx) {
    if (auto current = thread_table.threads.find(thread_table.current_uid); current != thread_table.threads.end())
        current->second.state = ThreadState::Sleeping;
    if (!activate_next_thread(ctx, "interrupt-done")) runtime.stop("PSP scheduler deadlock after interrupt");
}

void psp_thread_return(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx) {
    const std::int32_t uid = thread_table.current_uid;
    complete_current_thread(runtime, ctx);
    if (uid == 0) release_completed_thread_stack(uid);  // the kernel deletes module_start
}

void psp_interrupt_return(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx) {
    const std::int32_t uid = thread_table.current_uid;
    const auto found = async_return_frames.find(uid);
    if (found == async_return_frames.end() || found->second.empty()) {
        runtime.stop("PSP interrupt/callback return without a saved thread context");
        return;
    }

    const AsyncReturnKind kind = found->second.back().kind;
    if (kind == AsyncReturnKind::MpegRingbuffer) {
        if (g_kernel_hooks.continue_async_frame != nullptr &&
            g_kernel_hooks.continue_async_frame(runtime, ctx, found->second.back())) return;
        found->second.pop_back();
        if (found->second.empty()) async_return_frames.erase(found);
        (void)maybe_start_pending_guest_callback(ctx);
        return;
    }
    if (kind == AsyncReturnKind::UserCallback) {
        ctx = found->second.back().resume;
        found->second.pop_back();
        if (found->second.empty()) async_return_frames.erase(found);
        (void)maybe_start_pending_guest_callback(ctx);
        return;
    }
    if (kind == AsyncReturnKind::GeCallbackChain) {
        // Restore the interrupted guest state before every callback in the
        // chain.  A callback must not leak SP, callee-saved GPRs, FPU or VFPU
        // state into the next callback merely because both were queued by one
        // display list.
        ctx = found->second.back().resume;
        if (start_next_guest_callback(ctx, false)) return;
    }

    ctx = found->second.back().resume;
    found->second.pop_back();
    if (found->second.empty()) async_return_frames.erase(found);

    if (std::getenv("PSPRECOMP_GE_DIAG") != nullptr || std::getenv("PSPRECOMP_SCHED_DIAG") != nullptr) {
        std::cerr << "[async-return] uid=" << uid
                  << " kind=" << (kind == AsyncReturnKind::GeCallbackChain ? "ge" : "subintr")
                  << " resume=" << psprecomp::hex32(ctx.pc) << "\n";
    }

    // A pending GE callback may now run only after the interrupted frame has
    // completely unwound.  Never inject it into a nested callback/interrupt.
    (void)maybe_start_pending_guest_callback(ctx);
}

const char *thread_state_name(ThreadState state) {
    switch (state) {
    case ThreadState::Created: return "Created";
    case ThreadState::Ready: return "Ready";
    case ThreadState::Running: return "Running";
    case ThreadState::Sleeping: return "Sleeping";
    case ThreadState::Delayed: return "Delayed";
    case ThreadState::IoDeferred: return "IoDeferred";
    case ThreadState::Completed: return "Completed";
    }
    return "Unknown";
}

bool event_diag_matches(const EventFlagRecord &flag) {
    if (std::getenv("PSPRECOMP_EVENT_DIAG") == nullptr) return false;
    const char *filter = std::getenv("PSPRECOMP_EVENT_DIAG_FILTER");
    return filter == nullptr || *filter == '\0' || flag.name.find(filter) != std::string::npos;
}

void dump_event_stall_state(const EventFlagRecord &flag, std::int32_t flag_uid,
                            const psprecomp::AllegrexContext &ctx) {
    std::cerr << "[event-stall] uid=" << flag_uid
              << " name=\"" << flag.name << "\""
              << " pattern=" << psprecomp::hex32(flag.current_pattern)
              << " polls=" << event_diag_poll_count
              << " vblank=" << display_vblank_index
              << " virtual_time_us=" << virtual_time_us
              << " dispatch_pc=" << psprecomp::hex32(psprecomp::runtime_dispatch_pc())
              << " ctx_pc=" << psprecomp::hex32(ctx.pc)
              << " ra=" << psprecomp::hex32(ctx.gpr[31])
              << " current_uid=" << thread_table.current_uid
              << " ready=" << thread_table.continuations.size() << "\n";

    std::vector<std::int32_t> uids;
    uids.reserve(thread_table.threads.size());
    for (const auto &[uid, unused] : thread_table.threads) {
        (void)unused;
        uids.push_back(uid);
    }
    std::sort(uids.begin(), uids.end());
    for (const std::int32_t uid : uids) {
        const ThreadRecord &thread = thread_table.threads.at(uid);
        std::cerr << "[event-stall-thread] uid=" << uid
                  << " name=\"" << thread.name << "\""
                  << " priority=" << thread.priority
                  << " state=" << thread_state_name(thread.state)
                  << " external=" << (thread.externally_suspended ? 1 : 0)
                  << " pc=" << psprecomp::hex32(thread.suspended_context.pc)
                  << " ra=" << psprecomp::hex32(thread.suspended_context.gpr[31])
                  << " delay_until=" << thread.delay_until_us
                  << " wakeups=" << thread.wakeup_count << "\n";
    }
    for (const ThreadContinuation &ready : thread_table.continuations) {
        std::cerr << "[event-stall-ready] uid=" << ready.uid
                  << " pc=" << psprecomp::hex32(ready.context.pc)
                  << " ra=" << psprecomp::hex32(ready.context.gpr[31])
                  << " sequence=" << ready.ready_sequence << "\n";
    }
    if (g_kernel_hooks.dump_event_stall_extra != nullptr) g_kernel_hooks.dump_event_stall_extra();
    for (const auto &[uid, item] : event_flag_table.flags) {
        if (item.name.find("World") == std::string::npos &&
            item.name.find("Umd") == std::string::npos) continue;
        std::cerr << "[event-stall-flag] uid=" << uid
                  << " name=\"" << item.name << "\""
                  << " pattern=" << psprecomp::hex32(item.current_pattern)
                  << " waiters=" << item.waiters.size() << "\n";
        for (const EventFlagWaiter &waiter : item.waiters) {
            std::cerr << "[event-stall-waiter] flag_uid=" << uid
                      << " thread_uid=" << waiter.uid
                      << " requested=" << psprecomp::hex32(waiter.requested)
                      << " mode=" << psprecomp::hex32(waiter.mode)
                      << " pc=" << psprecomp::hex32(waiter.context.pc)
                      << " ra=" << psprecomp::hex32(waiter.context.gpr[31]) << "\n";
        }
    }
    for (const auto &[uid, semaphore] : semaphore_table.semaphores) {
        if (semaphore.name.find("Stream") == std::string::npos &&
            semaphore.name.find("stream") == std::string::npos) continue;
        std::cerr << "[event-stall-sema] uid=" << uid
                  << " name=\"" << semaphore.name << "\""
                  << " count=" << semaphore.count
                  << " maximum=" << semaphore.maximum
                  << " waiters=" << semaphore.waiters.size() << "\n";
    }
}

bool event_flag_matches(const EventFlagRecord &flag, std::uint32_t requested, std::uint32_t mode) {
    if ((mode & 1u) != 0u) return (flag.current_pattern & requested) != 0u;
    return (flag.current_pattern & requested) == requested;
}

void consume_event_flag(EventFlagRecord &flag, std::uint32_t requested, std::uint32_t mode) {
    if ((mode & 0x20u) != 0u) flag.current_pattern &= ~requested;
    if ((mode & 0x10u) != 0u) flag.current_pattern = 0u;
}



namespace {

// Microseconds of guest time credited per outer dispatch.  A 333 MHz Allegrex
// retires roughly a few hundred instructions in a microsecond, and one chained
// dispatch covers a comparable amount of translated work, so a quarter of a
// microsecond per dispatch is the right order of magnitude.  Only monotonicity
// and rough scale matter: every consumer compares relative deadlines.
std::uint64_t starvation_tick_microseconds = 1u;

void execution_clock_tick(psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
    virtual_time_us += starvation_tick_microseconds;
    promote_expired_delays();
    if (interrupts_suspended) return;  // no preemption inside an interrupt-disabled section
    (void)deliver_due_periodic_interrupt(ctx);  // preempts below: the interrupt context has top priority

    const auto current = thread_table.threads.find(thread_table.current_uid);
    if (current == thread_table.threads.end() || current->second.state != ThreadState::Running) return;
    const auto best = best_ready_thread();
    if (best == thread_table.continuations.end()) return;
    if (thread_priority(best->uid) >= thread_priority(thread_table.current_uid)) return;

    // Resume exactly here.  Unlike an HLE-boundary preemption the thread is not
    // inside a call, so ctx.pc -- not $ra -- is the continuation point.
    enqueue_continuation(thread_table.current_uid, ctx);
    (void)activate_next_thread(ctx, "timer-preempt");
}

} // namespace

void install_execution_clock(std::uint64_t dispatch_interval) {
    starvation_tick_microseconds = std::max<std::uint64_t>(1u, dispatch_interval / 4u);
    psprecomp::set_runtime_starvation_hook(dispatch_interval == 0u ? nullptr : &execution_clock_tick,
                                           dispatch_interval);
}

std::uint64_t execution_clock_tick_microseconds() { return starvation_tick_microseconds; }

bool notify_callback(std::int32_t uid, std::uint32_t argument) {
    const auto found = callback_table.callbacks.find(uid);
    if (found == callback_table.callbacks.end()) return false;
    ++found->second.notify_count;
    found->second.notify_argument = argument;
    return true;
}

bool start_pending_user_callback(psprecomp::AllegrexContext &ctx, const psprecomp::AllegrexContext &resume) {
    const auto pending = std::find_if(callback_table.callbacks.begin(), callback_table.callbacks.end(),
        [](const auto &item) {
            return item.second.owner_uid == thread_table.current_uid &&
                item.second.notify_count != 0u && item.second.function != 0u;
        });
    if (pending == callback_table.callbacks.end()) return false;
    auto &frames = async_return_frames[thread_table.current_uid];
    if (!frames.empty()) return false;  // never nest inside another callback/interrupt
    CallbackRecord &callback = pending->second;
    frames.push_back(AsyncReturnFrame{AsyncReturnKind::UserCallback, resume, 0u, 0, 0, 0, pending->first});
    ctx.set_gpr(4, callback.notify_count);
    ctx.set_gpr(5, callback.notify_argument);
    ctx.set_gpr(6, callback.common);
    ctx.set_gpr(31, 0x00000004u);
    ctx.pc = callback.function;
    callback.notify_count = 0u;
    return true;
}

void arm_wait_timeout(psprecomp::Runtime &runtime, std::uint8_t kind, std::int32_t object,
                      std::uint32_t timeout_address) {
    if (!honor_wait_timeouts || timeout_address == 0u || !runtime.memory().contains(timeout_address, 4u)) return;
    const auto current = thread_table.threads.find(thread_table.current_uid);
    if (current == thread_table.threads.end()) return;
    ThreadRecord &thread = current->second;
    thread.timed_wait = true;
    thread.timed_wait_kind = kind;
    thread.timed_wait_object = object;
    thread.timed_wait_address = timeout_address;
    thread.delay_until_us = virtual_time_us + runtime.memory().load32(timeout_address);
    thread.delay_sequence = thread_table.next_delay_sequence++;
}

void reset_kernel(psprecomp::Runtime &runtime, std::uint32_t user_arena_start, const KernelHooks &hooks) {
    g_kernel_hooks = hooks;
    thread_table = ThreadTable{};
    event_diag_poll_count = 0u;
    event_diag_stop_polls = parse_environment_u64("PSPRECOMP_EVENT_DIAG_STOP_POLLS", 0u);
    event_diag_stall_reported = false;
    partition_table = PartitionTable{};
    partition_table.next_address = (user_arena_start + 0xFFu) & ~0xFFu;
    callback_table = CallbackTable{};
    semaphore_table = SemaphoreTable{};
    event_flag_table = EventFlagTable{};
    fixed_pool_table = FixedPoolTable{};
    mutex_table = MutexTable{};
    ThreadRecord module_thread{};
    module_thread.name = "module_start";
    module_thread.priority = 32u;
    module_thread.stack_size = 0x10000u;
    // Keep the loader/module stack at the top of user RAM.  The game arena grows
    // upward from the aligned end of the ELF, and subsequent thread stacks grow
    // downward below this reserved loader stack.
    module_thread.stack_top = 0x0A000000u;
    module_thread.stack_bottom = module_thread.stack_top - module_thread.stack_size;
    module_thread.kernel_context = module_thread.stack_top - 0x100u;
    thread_table.next_stack_top = module_thread.stack_bottom;
    module_thread.state = ThreadState::Running;
    if (!runtime.memory().contains(module_thread.stack_bottom, module_thread.stack_size))
        throw psprecomp::Error("module_start stack falls outside PSP user RAM");
    runtime.memory().zero(module_thread.stack_bottom, module_thread.stack_size);
    runtime.memory().store32(module_thread.stack_bottom, 0u);
    runtime.memory().store32(module_thread.kernel_context + 0xC0u, 0u);
    runtime.memory().store32(module_thread.kernel_context + 0xC8u, module_thread.stack_bottom);
    runtime.memory().store32(module_thread.kernel_context + 0xF8u, 0xFFFFFFFFu);
    runtime.memory().store32(module_thread.kernel_context + 0xFCu, 0xFFFFFFFFu);
    runtime.cpu().set_gpr(26, module_thread.kernel_context);
    runtime.cpu().set_gpr(29, module_thread.kernel_context);
    thread_table.threads.emplace(0, std::move(module_thread));
    virtual_time_us = 0u;
    volatile_memory_locked = false;
    general_purpose_io = 0u;
    pending_guest_callbacks.clear();
    async_return_frames.clear();
    display_vblank_index = 0u;
    sub_interrupts.clear();
    psprecomp::set_runtime_thread_identity(0, "module_start");
    runtime.register_function(0x00000000u, &psp_thread_return, "psp_thread_return");
    runtime.register_function(0x00000004u, &psp_interrupt_return, "psp_interrupt_return");
    periodic_interrupt = PeriodicInterrupt{};
    interrupts_suspended = false;
    runtime.register_function(0x00000008u, &psp_interrupt_park, "psp_interrupt_park");
}

void install_kernel_hle(psprecomp::Runtime &runtime) {
    g_runtime_for_timeouts = &runtime;
    // CPU interrupt control (not used by VCS). The flags value is 1 when
    // interrupts were enabled before the call.
    runtime.register_hle("Kernel_Library", 0x092968F4u,  // sceKernelCpuSuspendIntr
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.set_gpr(2, interrupts_suspended ? 0u : 1u);
            interrupts_suspended = true;
        });
    auto resume_interrupts = [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
        interrupts_suspended = ctx.gpr[4] == 0u;
        set_success(ctx);
    };
    runtime.register_hle("Kernel_Library", 0x5F10D406u, resume_interrupts);  // sceKernelCpuResumeIntr
    runtime.register_hle("Kernel_Library", 0x3B84732Du, resume_interrupts);  // sceKernelCpuResumeIntrWithSync
    runtime.register_hle("Kernel_Library", 0x47A0B729u,  // sceKernelIsCpuIntrSuspended(flags)
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, ctx.gpr[4] == 0u ? 1u : 0u); });
    runtime.register_hle("Kernel_Library", 0xB55249D2u,  // sceKernelIsCpuIntrEnable
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, interrupts_suspended ? 0u : 1u); });
    runtime.register_hle("SysMemUserForUser", 0x7591C7DBu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            compiled_sdk_version = ctx.gpr[4];
            set_success(ctx);
        });
    // sceKernelSetCompiledSdkVersion500_505: same contract, newer SDK range.
    runtime.register_hle("SysMemUserForUser", 0x91DE343Cu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            compiled_sdk_version = ctx.gpr[4];
            set_success(ctx);
        });
    runtime.register_hle("SysMemUserForUser", 0xF77D77CBu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            compiler_version = ctx.gpr[4];
            set_success(ctx);
        });

    runtime.register_hle("SysMemUserForUser", 0xA291F107u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            // The bootstrap owns one contiguous user arena growing upward,
            // while thread stacks grow downward.  Report the actual gap.
            const std::uint32_t low = (partition_table.next_address + 0xFFu) & ~0xFFu;
            const std::uint32_t high = thread_table.next_stack_top & ~0xFFu;
            ctx.set_gpr(2, high > low ? high - low : 0u);
        });

    runtime.register_hle("SysMemUserForUser", 0x237DBD4Fu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::string name = ctx.gpr[5] != 0u ? rt.memory().read_c_string(ctx.gpr[5], 128u) : "partition";
            const std::uint32_t size = ctx.gpr[7];
            const std::uint32_t alignment = 0x100u;
            const std::uint32_t aligned_size = (size + alignment - 1u) & ~(alignment - 1u);
            const std::uint32_t address = (partition_table.next_address + alignment - 1u) & ~(alignment - 1u);
            if (aligned_size == 0u || !rt.memory().contains(address, aligned_size)) {
                ctx.set_gpr(2, 0x80020190u);
                return;
            }
            rt.memory().zero(address, aligned_size);
            const std::int32_t uid = partition_table.next_uid++;
            partition_table.blocks.emplace(uid, PartitionBlock{name, address, aligned_size});
            partition_table.next_address = address + aligned_size;
            if (std::getenv("PSPRECOMP_PARTITION_DIAG") != nullptr) {
                std::cerr << "[partition] alloc uid=" << uid << " name=\"" << name
                          << "\" addr=" << psprecomp::hex32(address)
                          << " size=" << psprecomp::hex32(aligned_size)
                          << " next=" << psprecomp::hex32(partition_table.next_address)
                          << " stack_top=" << psprecomp::hex32(thread_table.next_stack_top) << "\n";
            }
            ctx.set_gpr(2, static_cast<std::uint32_t>(uid));
        });

    runtime.register_hle("SysMemUserForUser", 0x9D9A5BA1u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto it = partition_table.blocks.find(uid);
            ctx.set_gpr(2, it == partition_table.blocks.end() ? 0u : it->second.address);
        });

    runtime.register_hle("SysMemUserForUser", 0xB6D61D02u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto it = partition_table.blocks.find(uid);
            if (std::getenv("PSPRECOMP_PARTITION_DIAG") != nullptr) {
                std::cerr << "[partition] free uid=" << uid;
                if (it != partition_table.blocks.end()) {
                    std::cerr << " name=\"" << it->second.name << "\" addr="
                              << psprecomp::hex32(it->second.address)
                              << " size=" << psprecomp::hex32(it->second.size);
                }
                std::cerr << "\n";
            }
            ctx.set_gpr(2, partition_table.blocks.erase(uid) == 1u ? 0u : 0x800200CBu);
        });

    runtime.register_hle("ThreadManForUser", 0x446D8DE6u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::string name = ctx.gpr[4] != 0u ? rt.memory().read_c_string(ctx.gpr[4], 128u) : "unnamed";
            const std::uint32_t requested_stack = ctx.gpr[7];
            if (requested_stack < 0x200u) {
                ctx.set_gpr(2, 0x80020194u);
                return;
            }
            const std::uint32_t stack_size = (requested_stack + 0xFFu) & ~0xFFu;
            std::uint32_t stack_bottom = 0u;
            std::uint32_t stack_top = 0u;
            if (!allocate_thread_stack(stack_size, stack_bottom, stack_top) ||
                !rt.memory().contains(stack_bottom, stack_size)) {
                ctx.set_gpr(2, 0x80020190u);
                return;
            }

            ThreadRecord record{
                name,
                ctx.gpr[5],
                ctx.gpr[6],
                stack_size,
                ctx.gpr[8],
            };
            const std::int32_t uid = thread_table.next_uid++;
            record.stack_top = stack_top;
            record.stack_bottom = stack_bottom;
            record.kernel_context = stack_top - 0x100u;
            rt.memory().zero(stack_bottom, stack_size);
            rt.memory().store32(stack_bottom, static_cast<std::uint32_t>(uid));
            rt.memory().store32(record.kernel_context + 0xC0u, static_cast<std::uint32_t>(uid));
            rt.memory().store32(record.kernel_context + 0xC8u, stack_bottom);
            rt.memory().store32(record.kernel_context + 0xF8u, 0xFFFFFFFFu);
            rt.memory().store32(record.kernel_context + 0xFCu, 0xFFFFFFFFu);

            if (std::getenv("PSPRECOMP_TRACE") != nullptr || std::getenv("PSPRECOMP_THREAD_DIAG") != nullptr) {
                std::cerr << "[sched] create uid=" << uid << " name=" << record.name
                          << " entry=" << psprecomp::hex32(record.entry)
                          << " priority=" << record.priority << " stack=" << record.stack_size
                          << " range=" << psprecomp::hex32(record.stack_bottom) << "-"
                          << psprecomp::hex32(record.stack_top) << "\n";
            }
            thread_table.threads.emplace(uid, std::move(record));
            ctx.set_gpr(2, static_cast<std::uint32_t>(uid));
        });

    runtime.register_hle("ThreadManForUser", 0xF475845Du,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto it = thread_table.threads.find(uid);
            if (it == thread_table.threads.end()) {
                ctx.set_gpr(2, 0x80020198u);
                return;
            }
            ThreadRecord &thread = it->second;
            if (thread.state != ThreadState::Created) {
                ctx.set_gpr(2, 0x800201A4u);
                return;
            }

            const std::uint32_t arg_size = ctx.gpr[5];
            const std::uint32_t arg_ptr = ctx.gpr[6];
            std::uint32_t sp = thread.kernel_context;

            psprecomp::AllegrexContext next{};
            if (arg_ptr != 0u && arg_size != 0u) {
                const std::uint32_t aligned_args = (arg_size + 0xFu) & ~0xFu;
                if (sp < thread.stack_bottom + aligned_args + 64u ||
                    !rt.memory().contains(arg_ptr, arg_size)) {
                    ctx.set_gpr(2, 0x800200D3u);
                    return;
                }
                sp -= aligned_args;
                std::vector<std::uint8_t> arguments(arg_size);
                rt.memory().copy_out(arg_ptr, arguments);
                rt.memory().copy_in(sp, arguments);
                next.set_gpr(4, arg_size);
                next.set_gpr(5, sp);
            } else {
                next.set_gpr(4, 0u);
                next.set_gpr(5, 0u);
            }

            // The PSP kernel consumes another 64 bytes and places the thread
            // return trampoline at the bottom of that frame.  Address zero is
            // registered as the native thread-return target in this runtime.
            sp -= 64u;
            next.set_gpr(26, thread.kernel_context);
            next.set_gpr(28, ctx.gpr[28]);
            next.set_gpr(29, sp);
            next.set_gpr(30, sp);
            next.set_gpr(31, 0u);
            next.pc = thread.entry;
            enqueue_continuation(uid, next);

            const std::int32_t caller_uid = thread_table.current_uid;
            const std::uint32_t caller_priority = thread_priority(caller_uid);
            if (std::getenv("PSPRECOMP_TRACE") != nullptr || std::getenv("PSPRECOMP_THREAD_DIAG") != nullptr) {
                std::cerr << "[sched] start uid=" << uid << " name=" << thread.name
                          << " entry=" << psprecomp::hex32(thread.entry)
                          << " priority=" << thread.priority
                          << " caller=" << caller_uid
                          << " caller_priority=" << caller_priority << "\n";
            }

            // Starting a thread does not automatically hand it the CPU.  It
            // only preempts when its numeric PSP priority is strictly higher.
            if (thread.priority < caller_priority) {
                psprecomp::AllegrexContext caller = ctx;
                caller.set_gpr(2, 0u);
                caller.pc = ctx.gpr[31];
                enqueue_continuation(caller_uid, caller);
                (void)activate_next_thread(ctx, "thread-control");
            } else {
                set_success(ctx);
            }
        });

    runtime.register_hle("ThreadManForUser", 0x809CE29Bu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::int32_t uid = thread_table.current_uid;
            complete_current_thread(rt, ctx);
            release_completed_thread_stack(uid);
        });

    runtime.register_hle("ThreadManForUser", 0x383F7BCCu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::int32_t uid = static_cast<std::int32_t>(ctx.gpr[4]);
            if (uid == 0 || uid == thread_table.current_uid) {
                ctx.set_gpr(2, 0x80020197u);  // SCE_KERNEL_ERROR_ILLEGAL_THID
                return;
            }
            const auto found = thread_table.threads.find(uid);
            if (found == thread_table.threads.end()) {
                ctx.set_gpr(2, 0x80020198u);  // SCE_KERNEL_ERROR_UNKNOWN_THID
                return;
            }
            const bool was_active = found->second.state != ThreadState::Created &&
                                    found->second.state != ThreadState::Completed;
            if (std::getenv("PSPRECOMP_THREAD_DIAG") != nullptr) {
                std::cerr << "[thread] terminate-delete uid=" << uid
                          << " name=" << found->second.name
                          << " active=" << (was_active ? 1 : 0) << "\n";
            }
            thread_table.continuations.erase(
                std::remove_if(thread_table.continuations.begin(), thread_table.continuations.end(),
                    [uid](const ThreadContinuation &item) { return item.uid == uid; }),
                thread_table.continuations.end());
            pending_guest_callbacks.erase(uid);
            async_return_frames.erase(uid);
            notify_thread_removed(uid);
            if (was_active) wake_thread_end_waiters(uid, 0x800201ACu);
            else wake_thread_end_waiters(uid, 0u);
            remove_thread_from_wait_queues(uid);
            release_thread_stack(found->second);
            thread_table.threads.erase(found);
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0x9FA03CD3u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::int32_t uid = static_cast<std::int32_t>(ctx.gpr[4]);
            if (uid == 0 || uid == thread_table.current_uid) {
                ctx.set_gpr(2, 0x800201A4u);  // SCE_KERNEL_ERROR_NOT_DORMANT
                return;
            }
            const auto found = thread_table.threads.find(uid);
            if (found == thread_table.threads.end()) {
                ctx.set_gpr(2, 0x80020198u);
                return;
            }
            if (found->second.state != ThreadState::Created &&
                found->second.state != ThreadState::Completed) {
                ctx.set_gpr(2, 0x800201A4u);
                return;
            }
            if (std::getenv("PSPRECOMP_THREAD_DIAG") != nullptr)
                std::cerr << "[thread] delete uid=" << uid << " name=" << found->second.name << "\n";
            remove_thread_from_wait_queues(uid);
            pending_guest_callbacks.erase(uid);
            async_return_frames.erase(uid);
            notify_thread_removed(uid);
            release_thread_stack(found->second);
            thread_table.threads.erase(found);
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0x9944F31Fu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::int32_t uid = static_cast<std::int32_t>(ctx.gpr[4]);
            // The PSP does not accept 0 as an alias for the current thread
            // here.  Suspending the caller (explicitly or through 0) is
            // illegal; only another live thread may be suspended.
            if (uid == 0 || uid == thread_table.current_uid) {
                ctx.set_gpr(2, 0x80020197u);  // SCE_KERNEL_ERROR_ILLEGAL_THID
                return;
            }
            const auto found = thread_table.threads.find(uid);
            if (found == thread_table.threads.end()) {
                ctx.set_gpr(2, 0x80020198u);  // SCE_KERNEL_ERROR_UNKNOWN_THID
                return;
            }
            ThreadRecord &thread = found->second;
            if (std::getenv("PSPRECOMP_THREAD_DIAG") != nullptr) {
                std::cerr << "[thread] suspend requested=" << uid
                          << " current=" << thread_table.current_uid
                          << " name=" << thread.name << " state=" << static_cast<int>(thread.state)
                          << " continuations=" << thread_table.continuations.size() << "\n";
            }
            if (thread.state == ThreadState::Completed || thread.state == ThreadState::Created) {
                ctx.set_gpr(2, 0x800201A2u);  // SCE_KERNEL_ERROR_DORMANT
                return;
            }
            if (thread.externally_suspended) {
                ctx.set_gpr(2, 0x800201A3u);  // SCE_KERNEL_ERROR_SUSPEND
                return;
            }

            thread.externally_suspended = true;
            const auto continuation = std::find_if(
                thread_table.continuations.begin(), thread_table.continuations.end(),
                [uid](const ThreadContinuation &item) { return item.uid == uid; });
            if (continuation != thread_table.continuations.end()) {
                thread.suspended_context = continuation->context;
                thread.state = ThreadState::Ready;
                thread_table.continuations.erase(continuation);
            }
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0x75156E8Fu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::int32_t uid = static_cast<std::int32_t>(ctx.gpr[4]);
            if (uid == 0 || uid == thread_table.current_uid) {
                ctx.set_gpr(2, 0x80020197u);  // SCE_KERNEL_ERROR_ILLEGAL_THID
                return;
            }
            const auto found = thread_table.threads.find(uid);
            if (found == thread_table.threads.end()) {
                ctx.set_gpr(2, 0x80020198u);  // SCE_KERNEL_ERROR_UNKNOWN_THID
                return;
            }
            ThreadRecord &thread = found->second;
            if (std::getenv("PSPRECOMP_THREAD_DIAG") != nullptr) {
                std::cerr << "[thread] resume requested=" << uid
                          << " current=" << thread_table.current_uid
                          << " name=" << thread.name << " state=" << static_cast<int>(thread.state)
                          << " continuations=" << thread_table.continuations.size() << "\n";
            }
            if (!thread.externally_suspended) {
                ctx.set_gpr(2, 0x800201A5u);  // SCE_KERNEL_ERROR_NOT_SUSPEND
                return;
            }
            thread.externally_suspended = false;
            if (thread.state == ThreadState::Ready)
                enqueue_continuation(uid, thread.suspended_context);
            set_success(ctx);
            (void)preempt_if_higher_priority(ctx, "thread-resume");
        });

    runtime.register_hle("ThreadManForUser", 0x293B45B8u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.set_gpr(2, static_cast<std::uint32_t>(thread_table.current_uid));
        });

    runtime.register_hle("ThreadManForUser", 0x71BC9871u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            std::int32_t uid = static_cast<std::int32_t>(ctx.gpr[4]);
            if (uid == 0)
                uid = thread_table.current_uid;

            std::uint32_t priority = ctx.gpr[5];
            if (priority == 0u)
                priority = thread_priority(thread_table.current_uid);

            const auto found = thread_table.threads.find(uid);
            if (found == thread_table.threads.end()) {
                ctx.set_gpr(2, 0x80020198u);  // SCE_KERNEL_ERROR_UNKNOWN_THID
                return;
            }
            ThreadRecord &thread = found->second;
            if (thread.state == ThreadState::Created || thread.state == ThreadState::Completed) {
                ctx.set_gpr(2, 0x800201A2u);  // SCE_KERNEL_ERROR_DORMANT
                return;
            }
            if (priority < 0x08u || priority > 0x77u) {
                ctx.set_gpr(2, 0x80020193u);  // SCE_KERNEL_ERROR_ILLEGAL_PRIORITY
                return;
            }

            thread.priority = priority;
            if (std::getenv("PSPRECOMP_THREAD_DIAG") != nullptr) {
                std::cerr << "[thread] priority uid=" << uid
                          << " current=" << thread_table.current_uid
                          << " value=" << priority << "\n";
            }

            // Changing priority is a scheduling point on the PSP.  Save the
            // current HLE return state only when a strictly higher-priority
            // ready thread exists, then let the normal dispatcher select it.
            const auto best = best_ready_thread();
            const std::uint32_t current_priority = thread_priority(thread_table.current_uid);
            if (best != thread_table.continuations.end() &&
                thread_priority(best->uid) < current_priority) {
                const std::int32_t caller_uid = thread_table.current_uid;
                psprecomp::AllegrexContext caller = ctx;
                caller.set_gpr(2, 0u);
                caller.pc = ctx.gpr[31];
                enqueue_continuation(caller_uid, caller);
                (void)activate_next_thread(ctx, "thread-control");
                return;
            }
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0x110DEC9Au,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t output = ctx.gpr[5];
            if (!rt.memory().contains(output, 8u)) {
                ctx.set_gpr(2, 0x800200D3u);
                return;
            }
            rt.memory().store32(output, ctx.gpr[4]);
            rt.memory().store32(output + 4u, 0u);
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0xC8CD158Cu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.set_gpr(2, ctx.gpr[4]);
            ctx.set_gpr(3, 0u);
        });

    runtime.register_hle("ThreadManForUser", 0xBA6B92E2u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t clock = ctx.gpr[4];
            const std::uint32_t seconds_out = ctx.gpr[5];
            const std::uint32_t usec_out = ctx.gpr[6];
            if (!rt.memory().contains(clock, 8u)) {
                ctx.set_gpr(2, 0x800200D3u);
                return;
            }
            const std::uint64_t ticks = static_cast<std::uint64_t>(rt.memory().load32(clock)) |
                (static_cast<std::uint64_t>(rt.memory().load32(clock + 4u)) << 32u);
            if (rt.memory().contains(seconds_out, 4u))
                rt.memory().store32(seconds_out, static_cast<std::uint32_t>(ticks / 1'000'000u));
            if (rt.memory().contains(usec_out, 4u))
                rt.memory().store32(usec_out, static_cast<std::uint32_t>(ticks % 1'000'000u));
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0xE1619D7Cu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint64_t ticks = static_cast<std::uint64_t>(ctx.gpr[4]) |
                (static_cast<std::uint64_t>(ctx.gpr[5]) << 32u);
            if (rt.memory().contains(ctx.gpr[6], 4u))
                rt.memory().store32(ctx.gpr[6], static_cast<std::uint32_t>(ticks / 1'000'000u));
            if (rt.memory().contains(ctx.gpr[7], 4u))
                rt.memory().store32(ctx.gpr[7], static_cast<std::uint32_t>(ticks % 1'000'000u));
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0xDB738F35u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t output = ctx.gpr[4];
            if (!rt.memory().contains(output, 8u)) {
                ctx.set_gpr(2, 0x800200D3u);
                return;
            }
            const std::uint64_t usec = system_time_microseconds();
            rt.memory().store32(output, static_cast<std::uint32_t>(usec));
            rt.memory().store32(output + 4u, static_cast<std::uint32_t>(usec >> 32u));
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0x82BC5777u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint64_t usec = system_time_microseconds();
            ctx.set_gpr(2, static_cast<std::uint32_t>(usec));
            ctx.set_gpr(3, static_cast<std::uint32_t>(usec >> 32u));
        });

    runtime.register_hle("ThreadManForUser", 0x369ED59Du,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.set_gpr(2, static_cast<std::uint32_t>(system_time_microseconds()));
        });

    // The PSP profiler query APIs return a null profiler register block in
    // ordinary user-mode execution.  VCS probes both during startup.
    const auto refer_profiler = [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
        ctx.set_gpr(2, 0u);
    };
    runtime.register_hle("ThreadManForUser", 0x64D4540Eu, refer_profiler);
    runtime.register_hle("ThreadManForUser", 0x8218B4DDu, refer_profiler);

    runtime.register_hle("ThreadManForUser", 0xEA748E31u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            // PSPSDK documents the first argument as reserved/zero.  VCS uses
            // this call to opt the current thread into VFPU context handling.
            const std::uint32_t reserved = ctx.gpr[4];
            const std::uint32_t attributes = ctx.gpr[5];
            if (reserved != 0u) {
                ctx.set_gpr(2, 0x800200D2u);
                return;
            }
            if (auto current = thread_table.threads.find(thread_table.current_uid);
                current != thread_table.threads.end()) {
                current->second.attributes |= attributes;
            }
            if (std::getenv("PSPRECOMP_TRACE") != nullptr) {
                std::cerr << "[hle] sceKernelChangeCurrentThreadAttr uid="
                          << thread_table.current_uid << " add=0x" << std::hex
                          << std::uppercase << attributes << std::dec << "\n";
            }
            set_success(ctx);
        });

    auto sleep_thread = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
        (void)sleep_current_thread(rt, ctx);
    };
    runtime.register_hle("ThreadManForUser", 0x9ACE131Eu, sleep_thread);
    runtime.register_hle("ThreadManForUser", 0x82826F70u, sleep_thread);
    runtime.register_hle("ThreadManForUser", 0xD59EAD2Fu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t result = wake_thread(static_cast<std::int32_t>(ctx.gpr[4]));
            ctx.set_gpr(2, result);
            if (result == 0u) (void)preempt_if_higher_priority(ctx, "thread-wakeup");
        });
    runtime.register_hle("ThreadManForUser", 0xFCCFAD26u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto found = thread_table.threads.find(static_cast<std::int32_t>(ctx.gpr[4]));
            if (found == thread_table.threads.end()) {
                ctx.set_gpr(2, 0x80020198u);
                return;
            }
            const std::uint32_t previous = found->second.wakeup_count;
            found->second.wakeup_count = 0u;
            ctx.set_gpr(2, previous);
        });

    runtime.register_hle("ThreadManForUser", 0xAA73C935u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (auto current = thread_table.threads.find(thread_table.current_uid);
                current != thread_table.threads.end()) {
                current->second.exit_status = ctx.gpr[4];
                if (std::getenv("PSPRECOMP_THREAD_DIAG") != nullptr)
                    std::cerr << "[thread] exit uid=" << thread_table.current_uid
                              << " name=" << current->second.name
                              << " status=" << psprecomp::hex32(ctx.gpr[4]) << "\n";
            }
            complete_current_thread(rt, ctx);
        });

    runtime.register_hle("ThreadManForUser", 0x278C0DF5u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto it = thread_table.threads.find(uid);
            if (uid <= 0 || it == thread_table.threads.end()) {
                ctx.set_gpr(2, 0x80020198u);
                return;
            }
            if (it->second.state == ThreadState::Completed) {
                set_success(ctx);
                return;
            }
            psprecomp::AllegrexContext waiter = ctx;
            waiter.set_gpr(2, 0u);
            waiter.pc = ctx.gpr[31];
            thread_table.thread_end_waiters[uid].push_back({thread_table.current_uid, waiter});
            if (auto current = thread_table.threads.find(thread_table.current_uid);
                current != thread_table.threads.end()) {
                current->second.state = ThreadState::Sleeping;
                current->second.suspended_context = waiter;
            }
            arm_wait_timeout(rt, 2u, uid, ctx.gpr[5]);
            if (!activate_next_thread(ctx, "kernel-wait")) {
                rt.stop("PSP thread wait deadlock on uid " + std::to_string(uid));
            }
        });

    auto delay_thread = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
        (void)delay_current_thread(rt, ctx, std::max(ctx.gpr[4], minimum_thread_delay_us));
    };
    runtime.register_hle("ThreadManForUser", 0xCEADEB47u, delay_thread);
    // sceKernelDelayThreadCB: a CB wait delivers the thread's notified callbacks.
    // The callback runs in place of the delay, which then counts as elapsed.
    runtime.register_hle("ThreadManForUser", 0x68DA9E36u,
        [delay_thread](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            psprecomp::AllegrexContext resume = ctx;
            resume.pc = ctx.gpr[31];
            resume.set_gpr(2, 0u);
            if (!start_pending_user_callback(ctx, resume)) delay_thread(rt, ctx);
        });

    runtime.register_hle("ThreadManForUser", 0xE81CAF8Fu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::string name = ctx.gpr[4] != 0u ? rt.memory().read_c_string(ctx.gpr[4], 128u) : "callback";
            const std::int32_t uid = callback_table.next_uid++;
            callback_table.callbacks.emplace(uid, CallbackRecord{
                name, ctx.gpr[5], ctx.gpr[6], thread_table.current_uid, 0u, 0u});
            ctx.set_gpr(2, static_cast<std::uint32_t>(uid));
        });
    runtime.register_hle("ThreadManForUser", 0xEDBA5844u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            ctx.set_gpr(2, callback_table.callbacks.erase(uid) == 1u ? 0u : 0x800201A1u);
        });

    runtime.register_hle("ThreadManForUser", 0x349D6D6Cu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            // Even an empty callback checkpoint consumes CPU time on real PSP
            // hardware. Without this, a busy polling thread can freeze virtual
            // time forever and starve delayed video/audio workers.
            virtual_time_us += 25u;
            promote_expired_delays();

            auto pending = std::find_if(callback_table.callbacks.begin(), callback_table.callbacks.end(),
                [](const auto &item) {
                    return item.second.owner_uid == thread_table.current_uid &&
                        item.second.notify_count != 0u && item.second.function != 0u;
                });
            if (pending == callback_table.callbacks.end()) {
                set_success(ctx);
                (void)preempt_if_higher_priority(ctx, "check-callback");
                return;
            }

            CallbackRecord &callback = pending->second;
            const std::uint32_t count = callback.notify_count;
            const std::uint32_t argument = callback.notify_argument;
            callback.notify_count = 0u;

            psprecomp::AllegrexContext resume = ctx;
            resume.pc = ctx.gpr[31];
            resume.set_gpr(2, 1u);
            auto &frames = async_return_frames[thread_table.current_uid];
            if (!frames.empty()) {
                ctx.set_gpr(2, 0u);
                return;
            }
            frames.push_back(AsyncReturnFrame{AsyncReturnKind::UserCallback, resume, 0u, 0, 0, 0, pending->first});
            ctx.set_gpr(4, count);
            ctx.set_gpr(5, argument);
            ctx.set_gpr(6, callback.common);
            ctx.set_gpr(31, 0x00000004u);
            ctx.pc = callback.function;
        });

    runtime.register_hle("ThreadManForUser", 0xD6DA4BA1u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::string name = ctx.gpr[4] != 0u ? rt.memory().read_c_string(ctx.gpr[4], 128u) : "semaphore";
            const auto initial = static_cast<std::int32_t>(ctx.gpr[6]);
            const auto maximum = static_cast<std::int32_t>(ctx.gpr[7]);
            if (initial < 0 || maximum <= 0 || initial > maximum) {
                ctx.set_gpr(2, 0x800201B0u);
                return;
            }
            const std::int32_t uid = semaphore_table.next_uid++;
            semaphore_table.semaphores.emplace(uid, SemaphoreRecord{name, initial, maximum, {}});
            ctx.set_gpr(2, static_cast<std::uint32_t>(uid));
        });
    runtime.register_hle("ThreadManForUser", 0x28B6489Cu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto found = semaphore_table.semaphores.find(uid);
            if (found == semaphore_table.semaphores.end()) {
                ctx.set_gpr(2, 0x80020199u);
                return;
            }
            for (auto &waiter : found->second.waiters) {
                waiter.context.set_gpr(2, 0x800201A7u);
                enqueue_continuation(waiter.uid, waiter.context);
            }
            semaphore_table.semaphores.erase(found);
            set_success(ctx);
            (void)preempt_if_higher_priority(ctx, "semaphore-delete");
        });
    runtime.register_hle("ThreadManForUser", 0x3F53E640u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto amount = static_cast<std::int32_t>(ctx.gpr[5]);
            const auto it = semaphore_table.semaphores.find(uid);
            if (it == semaphore_table.semaphores.end() || amount <= 0 ||
                static_cast<std::int64_t>(it->second.count) + amount > it->second.maximum) {
                ctx.set_gpr(2, 0x80020199u);
                return;
            }
            SemaphoreRecord &semaphore = it->second;
            semaphore.count += amount;
            auto waiter = semaphore.waiters.begin();
            while (waiter != semaphore.waiters.end()) {
                if (semaphore.count >= waiter->requested) {
                    semaphore.count -= waiter->requested;
                    waiter->context.set_gpr(2, 0u);
                    enqueue_continuation(waiter->uid, waiter->context);
                    waiter = semaphore.waiters.erase(waiter);
                } else {
                    ++waiter;
                }
            }
            set_success(ctx);
            (void)preempt_if_higher_priority(ctx, "semaphore-signal");
        });
    auto semaphore_wait = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
        const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
        const auto amount = static_cast<std::int32_t>(ctx.gpr[5]);
        const auto it = semaphore_table.semaphores.find(uid);
        if (it == semaphore_table.semaphores.end() || amount <= 0 || amount > it->second.maximum) {
            ctx.set_gpr(2, 0x80020199u);
            return;
        }
        if (it->second.count >= amount) {
            it->second.count -= amount;
            set_success(ctx);
            return;
        }
        const psprecomp::AllegrexContext suspended = make_wait_context(ctx);
        it->second.waiters.push_back(SemaphoreWaiter{thread_table.current_uid, suspended, amount});
        arm_wait_timeout(rt, 1u, uid, ctx.gpr[6]);
        (void)suspend_current_thread(rt, ctx, suspended, "semaphore " + std::to_string(uid));
    };
    runtime.register_hle("ThreadManForUser", 0x4E3A1105u, semaphore_wait);
    runtime.register_hle("ThreadManForUser", 0x6D212BACu, semaphore_wait);
    runtime.register_hle("ThreadManForUser", 0x58B1F937u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto amount = static_cast<std::int32_t>(ctx.gpr[5]);
            const auto it = semaphore_table.semaphores.find(uid);
            if (it == semaphore_table.semaphores.end() || amount <= 0 || amount > it->second.maximum) {
                ctx.set_gpr(2, 0x80020199u);
                return;
            }
            if (it->second.count < amount) {
                ctx.set_gpr(2, 0x800201AEu);
                return;
            }
            it->second.count -= amount;
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0x55C20A00u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::string name = ctx.gpr[4] != 0u ? rt.memory().read_c_string(ctx.gpr[4], 128u) : "event_flag";
            const std::int32_t uid = event_flag_table.next_uid++;
            event_flag_table.flags.emplace(uid, EventFlagRecord{name, ctx.gpr[5], ctx.gpr[6], ctx.gpr[6], {}});
            if (event_diag_matches(event_flag_table.flags.at(uid))) {
                std::cerr << "[event] create uid=" << uid << " name=\"" << name << "\""
                          << " attr=" << psprecomp::hex32(ctx.gpr[5])
                          << " initial=" << psprecomp::hex32(ctx.gpr[6])
                          << " thread=" << thread_table.current_uid
                          << " dispatch_pc=" << psprecomp::hex32(psprecomp::runtime_dispatch_pc())
                          << " ra=" << psprecomp::hex32(ctx.gpr[31]) << "\n";
            }
            ctx.set_gpr(2, static_cast<std::uint32_t>(uid));
        });
    runtime.register_hle("ThreadManForUser", 0xEF9E4C70u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto found = event_flag_table.flags.find(uid);
            if (found == event_flag_table.flags.end()) {
                ctx.set_gpr(2, 0x8002019Au);
                return;
            }
            for (auto &waiter : found->second.waiters) {
                waiter.context.set_gpr(2, 0x800201A7u);
                enqueue_continuation(waiter.uid, waiter.context);
            }
            event_flag_table.flags.erase(found);
            set_success(ctx);
            (void)preempt_if_higher_priority(ctx, "event-flag-delete");
        });
    runtime.register_hle("ThreadManForUser", 0x1FB15A32u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto it = event_flag_table.flags.find(static_cast<std::int32_t>(ctx.gpr[4]));
            if (it == event_flag_table.flags.end()) {
                ctx.set_gpr(2, 0x8002019Au);
                return;
            }
            EventFlagRecord &flag = it->second;
            const std::uint32_t previous_pattern = flag.current_pattern;
            flag.current_pattern |= ctx.gpr[5];
            if (event_diag_matches(flag)) {
                std::cerr << "[event] set uid=" << static_cast<std::int32_t>(ctx.gpr[4])
                          << " name=\"" << flag.name << "\""
                          << " bits=" << psprecomp::hex32(ctx.gpr[5])
                          << " old=" << psprecomp::hex32(previous_pattern)
                          << " new=" << psprecomp::hex32(flag.current_pattern)
                          << " thread=" << thread_table.current_uid
                          << " dispatch_pc=" << psprecomp::hex32(psprecomp::runtime_dispatch_pc())
                          << " ctx_pc=" << psprecomp::hex32(ctx.pc)
                          << " ra=" << psprecomp::hex32(ctx.gpr[31]) << "\n";
            }
            auto waiter = flag.waiters.begin();
            while (waiter != flag.waiters.end()) {
                if (!event_flag_matches(flag, waiter->requested, waiter->mode)) {
                    ++waiter;
                    continue;
                }
                if (waiter->output_address != 0u && rt.memory().contains(waiter->output_address, 4u))
                    rt.memory().store32(waiter->output_address, flag.current_pattern);
                consume_event_flag(flag, waiter->requested, waiter->mode);
                waiter->context.set_gpr(2, 0u);
                enqueue_continuation(waiter->uid, waiter->context);
                waiter = flag.waiters.erase(waiter);
            }
            set_success(ctx);
            (void)preempt_if_higher_priority(ctx, "event-flag-set");
        });
    runtime.register_hle("ThreadManForUser", 0x812346E4u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto it = event_flag_table.flags.find(static_cast<std::int32_t>(ctx.gpr[4]));
            if (it == event_flag_table.flags.end()) {
                ctx.set_gpr(2, 0x8002019Au);
                return;
            }
            // PSP clear semantics retain only the bits present in the mask.
            const std::uint32_t previous_pattern = it->second.current_pattern;
            it->second.current_pattern &= ctx.gpr[5];
            if (event_diag_matches(it->second)) {
                std::cerr << "[event] clear uid=" << static_cast<std::int32_t>(ctx.gpr[4])
                          << " name=\"" << it->second.name << "\""
                          << " mask=" << psprecomp::hex32(ctx.gpr[5])
                          << " old=" << psprecomp::hex32(previous_pattern)
                          << " new=" << psprecomp::hex32(it->second.current_pattern)
                          << " thread=" << thread_table.current_uid
                          << " dispatch_pc=" << psprecomp::hex32(psprecomp::runtime_dispatch_pc())
                          << " ctx_pc=" << psprecomp::hex32(ctx.pc)
                          << " ra=" << psprecomp::hex32(ctx.gpr[31]) << "\n";
            }
            set_success(ctx);
        });
    auto event_flag_wait = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
        const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
        const auto it = event_flag_table.flags.find(uid);
        if (it == event_flag_table.flags.end()) {
            ctx.set_gpr(2, 0x8002019Au);
            return;
        }
        const std::uint32_t requested = ctx.gpr[5];
        const std::uint32_t mode = ctx.gpr[6];
        if (requested == 0u || (mode & ~0x31u) != 0u) {
            ctx.set_gpr(2, 0x800201B1u);
            return;
        }
        if (event_flag_matches(it->second, requested, mode)) {
            if (ctx.gpr[7] != 0u && rt.memory().contains(ctx.gpr[7], 4u))
                rt.memory().store32(ctx.gpr[7], it->second.current_pattern);
            consume_event_flag(it->second, requested, mode);
            set_success(ctx);
            return;
        }
        const psprecomp::AllegrexContext suspended = make_wait_context(ctx);
        it->second.waiters.push_back(EventFlagWaiter{
            thread_table.current_uid, suspended, requested, mode, ctx.gpr[7]});
        arm_wait_timeout(rt, 3u, uid, ctx.gpr[8]);
        (void)suspend_current_thread(rt, ctx, suspended, "event flag " + std::to_string(uid));
    };
    runtime.register_hle("ThreadManForUser", 0x402FCF22u, event_flag_wait);
    runtime.register_hle("ThreadManForUser", 0x328C546Au, event_flag_wait);
    runtime.register_hle("ThreadManForUser", 0x30FD48F0u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto it = event_flag_table.flags.find(static_cast<std::int32_t>(ctx.gpr[4]));
            if (it == event_flag_table.flags.end()) {
                ctx.set_gpr(2, 0x8002019Au);
                return;
            }
            const std::uint32_t requested = ctx.gpr[5];
            const std::uint32_t mode = ctx.gpr[6];
            const bool diag = event_diag_matches(it->second);
            if (diag) ++event_diag_poll_count;
            if (diag && (event_diag_poll_count <= 32u || event_diag_poll_count % 100000u == 0u)) {
                std::cerr << "[event] poll count=" << event_diag_poll_count
                          << " uid=" << static_cast<std::int32_t>(ctx.gpr[4])
                          << " name=\"" << it->second.name << "\""
                          << " requested=" << psprecomp::hex32(requested)
                          << " mode=" << psprecomp::hex32(mode)
                          << " current=" << psprecomp::hex32(it->second.current_pattern)
                          << " match=" << (event_flag_matches(it->second, requested, mode) ? 1 : 0)
                          << " thread=" << thread_table.current_uid
                          << " dispatch_pc=" << psprecomp::hex32(psprecomp::runtime_dispatch_pc())
                          << " ctx_pc=" << psprecomp::hex32(ctx.pc)
                          << " ra=" << psprecomp::hex32(ctx.gpr[31]) << "\n";
            }
            if (diag && !event_diag_stall_reported && event_diag_stop_polls != 0u &&
                event_diag_poll_count >= event_diag_stop_polls) {
                event_diag_stall_reported = true;
                dump_event_stall_state(it->second, static_cast<std::int32_t>(ctx.gpr[4]), ctx);
                rt.stop("Target event flag exceeded PSPRECOMP_EVENT_DIAG_STOP_POLLS; scheduler state captured.");
                return;
            }
            if (!event_flag_matches(it->second, requested, mode)) {
                if (ctx.gpr[7] != 0u && rt.memory().contains(ctx.gpr[7], 4u))
                    rt.memory().store32(ctx.gpr[7], it->second.current_pattern);
                ctx.set_gpr(2, 0x800201AFu);
                return;
            }
            if (ctx.gpr[7] != 0u && rt.memory().contains(ctx.gpr[7], 4u))
                rt.memory().store32(ctx.gpr[7], it->second.current_pattern);
            consume_event_flag(it->second, requested, mode);
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0xC07BB470u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::string name = ctx.gpr[4] != 0u ? rt.memory().read_c_string(ctx.gpr[4], 128u) : "fpl";
            const std::uint32_t block_size = ctx.gpr[7];
            const std::uint32_t block_count = ctx.gpr[8];
            if (block_size == 0u || block_count == 0u ||
                block_size > 0xFFFFFFFFu / block_count) {
                ctx.set_gpr(2, 0x800201B0u);
                return;
            }
            const std::uint32_t alignment = 0x100u;
            const std::uint32_t total = block_size * block_count;
            const std::uint32_t address = (partition_table.next_address + alignment - 1u) & ~(alignment - 1u);
            const std::uint32_t reserved = (total + alignment - 1u) & ~(alignment - 1u);
            if (!rt.memory().contains(address, reserved) || address + reserved > thread_table.next_stack_top) {
                ctx.set_gpr(2, 0x80020190u);
                return;
            }
            rt.memory().zero(address, reserved);
            const std::int32_t uid = fixed_pool_table.next_uid++;
            fixed_pool_table.pools.emplace(uid, FixedPoolRecord{name, address, block_size, block_count,
                std::vector<bool>(block_count, false)});
            partition_table.next_address = address + reserved;
            if (std::getenv("PSPRECOMP_TRACE") != nullptr) {
                std::cerr << "[hle] sceKernelCreateFpl uid=" << uid << " name=" << name
                          << " block=0x" << std::hex << std::uppercase << block_size
                          << " count=" << std::dec << block_count << " base=0x"
                          << std::hex << std::uppercase << address << std::dec << "\n";
            }
            ctx.set_gpr(2, static_cast<std::uint32_t>(uid));
        });

    runtime.register_hle("ThreadManForUser", 0xD979E9BFu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const std::uint32_t output = ctx.gpr[5];
            const auto it = fixed_pool_table.pools.find(uid);
            if (it == fixed_pool_table.pools.end() || !rt.memory().contains(output, 4u)) {
                ctx.set_gpr(2, 0x800201A8u);
                return;
            }
            auto &pool = it->second;
            const auto free_it = std::find(pool.allocated.begin(), pool.allocated.end(), false);
            if (free_it == pool.allocated.end()) {
                ctx.set_gpr(2, 0x80020190u);
                return;
            }
            const std::size_t index = static_cast<std::size_t>(free_it - pool.allocated.begin());
            pool.allocated[index] = true;
            rt.memory().store32(output, pool.address + static_cast<std::uint32_t>(index) * pool.block_size);
            set_success(ctx);
        });

    // sceKernelTryAllocateFpl. The allocation above never blocks, so the
    // try-form is the same call: it either has a free block or it does not.
    runtime.register_hle("ThreadManForUser", 0x623AE665u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            rt.invoke_import("ThreadManForUser", 0xD979E9BFu, ctx);
        });

    // sceKernelFreeFpl.
    runtime.register_hle("ThreadManForUser", 0xF6414A71u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const std::uint32_t block = ctx.gpr[5];
            const auto it = fixed_pool_table.pools.find(uid);
            if (it == fixed_pool_table.pools.end()) {
                ctx.set_gpr(2, 0x800201A8u);
                return;
            }
            auto &pool = it->second;
            if (block < pool.address || pool.block_size == 0u) {
                ctx.set_gpr(2, 0x800201A9u);
                return;
            }
            const std::uint32_t offset = block - pool.address;
            const std::size_t index = offset / pool.block_size;
            if (offset % pool.block_size != 0u || index >= pool.allocated.size()) {
                ctx.set_gpr(2, 0x800201A9u);
                return;
            }
            pool.allocated[index] = false;
            set_success(ctx);
        });

    // sceKernelDeleteFpl. Loading a saved game from inside the game tears down
    // the previous session's pools, which is why this only ever mattered there:
    // the import was missing and the runtime stopped on a black screen.
    runtime.register_hle("ThreadManForUser", 0xED1410E0u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto it = fixed_pool_table.pools.find(uid);
            if (it == fixed_pool_table.pools.end()) {
                ctx.set_gpr(2, 0x800201A8u);
                return;
            }
            // The partition allocator only ever bumps a cursor. Returning the
            // memory when this pool happens to be the most recent allocation
            // costs one comparison and is what keeps a create/delete cycle --
            // exactly what repeated in-game loads are -- from walking the
            // cursor into the thread stacks and failing the fourth or fifth
            // time. Older pools still leak their range until a proper
            // allocator exists.
            const auto &pool = it->second;
            constexpr std::uint32_t alignment = 0x100u;
            const std::uint32_t reserved =
                (pool.block_size * pool.block_count + alignment - 1u) & ~(alignment - 1u);
            if (pool.address + reserved == partition_table.next_address)
                partition_table.next_address = pool.address;
            fixed_pool_table.pools.erase(it);
            set_success(ctx);
        });

    auto volatile_mem_lock = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
        constexpr std::uint32_t volatile_base = 0x08400000u;
        constexpr std::uint32_t volatile_size = 0x00400000u;
        if (ctx.gpr[4] != 0u) {
            ctx.set_gpr(2, 0x80000107u);
            return;
        }
        if (volatile_memory_locked) {
            ctx.set_gpr(2, 0x80000021u);
            return;
        }
        if (!rt.memory().contains(volatile_base, volatile_size) ||
            !rt.memory().contains(ctx.gpr[5], 4u) || !rt.memory().contains(ctx.gpr[6], 4u)) {
            ctx.set_gpr(2, 0x800200D3u);
            return;
        }
        rt.memory().store32(ctx.gpr[5], volatile_base);
        rt.memory().store32(ctx.gpr[6], volatile_size);
        rt.memory().zero(volatile_base, volatile_size);
        volatile_memory_locked = true;
        set_success(ctx);
    };
    runtime.register_hle("sceSuspendForUser", 0x3E0271D3u, volatile_mem_lock);
    runtime.register_hle("sceSuspendForUser", 0xA14F40B2u, volatile_mem_lock);
    runtime.register_hle("sceSuspendForUser", 0xA569E425u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            if (ctx.gpr[4] != 0u) {
                ctx.set_gpr(2, 0x80000107u);
                return;
            }
            if (!volatile_memory_locked) {
                ctx.set_gpr(2, 0x800201AEu);
                return;
            }
            volatile_memory_locked = false;
            set_success(ctx);
        });
    runtime.register_hle("sceSuspendForUser", 0xEADB1BD7u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.set_gpr(2, ctx.gpr[4] == 0u ? 0u : 0x80000107u);
        });
    runtime.register_hle("sceSuspendForUser", 0x3AEE7261u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.set_gpr(2, ctx.gpr[4] == 0u ? 0u : 0x80000107u);
        });
    runtime.register_hle("sceSuspendForUser", 0x090CCB3Fu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });

    runtime.register_hle("UtilsForUser", 0x37FB5C42u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.set_gpr(2, general_purpose_io);
        });
    runtime.register_hle("UtilsForUser", 0x6AD345D7u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            general_purpose_io = ctx.gpr[4];
            set_success(ctx);
        });

    // The statically recompiled CPU and host share one coherent guest-memory
    // backing store. Data-cache maintenance is therefore complete at the
    // call boundary. Instruction-cache invalidation is recorded as success;
    // dynamically loaded executable modules are handled by the PRX loader,
    // rather than by mutating the generated host code in place.
    auto cache_maintenance = [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
        set_success(ctx);
    };
    runtime.register_hle("UtilsForUser", 0xBFA98062u, cache_maintenance);
    runtime.register_hle("UtilsForUser", 0x79D1C3FAu, cache_maintenance);
    runtime.register_hle("UtilsForUser", 0xB435DEC5u, cache_maintenance);
    runtime.register_hle("UtilsForUser", 0x3EE30821u, cache_maintenance);
    runtime.register_hle("UtilsForUser", 0x34B9FA9Eu, cache_maintenance);
    runtime.register_hle("UtilsForUser", 0x920F104Au, cache_maintenance);
    runtime.register_hle("UtilsForUser", 0xC2DF770Eu, cache_maintenance);
    runtime.register_hle("UtilsForUser", 0x80001C4Cu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 0u); });
    runtime.register_hle("UtilsForUser", 0x16641D70u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 0u); });
    runtime.register_hle("UtilsForUser", 0x4FD31C9Du,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 0u); });
    runtime.register_hle("UtilsForUser", 0xFB05FAD0u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 0u); });

    runtime.register_hle("InterruptManager", 0xCA04A2B9u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t interrupt_number = ctx.gpr[4];
            const std::uint32_t sub_number = ctx.gpr[5];
            const std::uint32_t handler = ctx.gpr[6];
            const std::uint32_t argument = ctx.gpr[7];
            if (interrupt_number >= 67u || handler == 0u) {
                ctx.set_gpr(2, 0x80020064u);
                return;
            }
            const std::uint64_t key = sub_interrupt_key(interrupt_number, sub_number);
            if (sub_interrupts.contains(key)) {
                ctx.set_gpr(2, 0x80020067u);  // handler already present
                return;
            }
            sub_interrupts.emplace(key, SubInterruptRecord{handler, argument, false, false});
            if (std::getenv("PSPRECOMP_GE_DIAG") != nullptr) {
                std::cerr << "[intr] register int=" << interrupt_number << " sub=" << sub_number
                          << " handler=" << psprecomp::hex32(handler)
                          << " arg=" << psprecomp::hex32(argument) << "\n";
            }
            set_success(ctx);
        });
    runtime.register_hle("InterruptManager", 0xD61E6961u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint64_t key = sub_interrupt_key(ctx.gpr[4], ctx.gpr[5]);
            ctx.set_gpr(2, sub_interrupts.erase(key) == 1u ? 0u : 0x80020068u);
        });
    runtime.register_hle("InterruptManager", 0xFB8E22ECu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto found = sub_interrupts.find(sub_interrupt_key(ctx.gpr[4], ctx.gpr[5]));
            if (found == sub_interrupts.end()) { ctx.set_gpr(2, 0x80020068u); return; }
            found->second.enabled = true;
            set_success(ctx);
        });
    runtime.register_hle("InterruptManager", 0x8A389411u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto found = sub_interrupts.find(sub_interrupt_key(ctx.gpr[4], ctx.gpr[5]));
            if (found == sub_interrupts.end()) { ctx.set_gpr(2, 0x80020068u); return; }
            found->second.enabled = false;
            set_success(ctx);
        });
    runtime.register_hle("InterruptManager", 0x5CB5A78Bu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto found = sub_interrupts.find(sub_interrupt_key(ctx.gpr[4], ctx.gpr[5]));
            if (found == sub_interrupts.end()) { ctx.set_gpr(2, 0x80020068u); return; }
            if (ctx.gpr[6] != 0u) {
                if (!rt.memory().contains(ctx.gpr[6], 4u)) { ctx.set_gpr(2, 0x800200D3u); return; }
                rt.memory().store32(ctx.gpr[6], found->second.enabled ? 1u : 0u);
            }
            found->second.enabled = false;
            set_success(ctx);
        });
    runtime.register_hle("InterruptManager", 0x7860E0DCu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto found = sub_interrupts.find(sub_interrupt_key(ctx.gpr[4], ctx.gpr[5]));
            if (found == sub_interrupts.end()) { ctx.set_gpr(2, 0x80020068u); return; }
            found->second.enabled = ctx.gpr[6] != 0u;
            set_success(ctx);
        });
    runtime.register_hle("InterruptManager", 0xFC4374B8u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto found = sub_interrupts.find(sub_interrupt_key(ctx.gpr[4], ctx.gpr[5]));
            ctx.set_gpr(2, found != sub_interrupts.end() && found->second.occurred ? 1u : 0u);
        });


    // Mutexes (not used by VCS). Attribute 0x200 permits recursive locking.
    // Waiters are served FIFO; lock timeouts are not modelled, matching the
    // semaphore and event-flag waits above.
    constexpr std::uint32_t kMutexNotFound = 0x800201C3u;
    constexpr std::uint32_t kMutexTryLockFailed = 0x800201C4u;
    constexpr std::uint32_t kMutexNotOwned = 0x800201C5u;
    constexpr std::uint32_t kMutexLockOverflow = 0x800201C6u;
    constexpr std::uint32_t kMutexUnlockUnderflow = 0x800201C7u;
    constexpr std::uint32_t kMutexAlreadyLocked = 0x800201C8u;
    constexpr std::uint32_t kWaitDeleted = 0x800201B5u;
    constexpr std::uint32_t kMutexRecursive = 0x200u;
    runtime.register_hle("ThreadManForUser", 0xB7D098C6u,  // sceKernelCreateMutex
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::string name = ctx.gpr[4] != 0u ? rt.memory().read_c_string(ctx.gpr[4], 128u) : "mutex";
            const auto initial = static_cast<std::int32_t>(ctx.gpr[6]);
            if (initial < 0 || (initial > 1 && (ctx.gpr[5] & kMutexRecursive) == 0u)) {
                ctx.set_gpr(2, kMutexLockOverflow);
                return;
            }
            const std::int32_t uid = mutex_table.next_uid++;
            MutexRecord mutex{name, ctx.gpr[5]};
            if (initial > 0) {
                mutex.owner_uid = thread_table.current_uid;
                mutex.lock_count = initial;
            }
            mutex_table.mutexes.emplace(uid, std::move(mutex));
            ctx.set_gpr(2, static_cast<std::uint32_t>(uid));
        });
    runtime.register_hle("ThreadManForUser", 0xF8170FBEu,  // sceKernelDeleteMutex
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto found = mutex_table.mutexes.find(static_cast<std::int32_t>(ctx.gpr[4]));
            if (found == mutex_table.mutexes.end()) {
                ctx.set_gpr(2, kMutexNotFound);
                return;
            }
            for (auto &waiter : found->second.waiters) {
                waiter.context.set_gpr(2, kWaitDeleted);
                enqueue_continuation(waiter.uid, waiter.context);
            }
            mutex_table.mutexes.erase(found);
            set_success(ctx);
            (void)preempt_if_higher_priority(ctx, "mutex-delete");
        });
    // Returns 0 when the current thread now holds the mutex, or the PSP error.
    static const auto try_acquire_mutex = [](MutexRecord &mutex, std::int32_t count) -> std::uint32_t {
        const std::int32_t self = thread_table.current_uid;
        if (mutex.lock_count == 0) {
            mutex.owner_uid = self;
            mutex.lock_count = count;
            return 0u;
        }
        if (mutex.owner_uid == self) {
            if ((mutex.attributes & kMutexRecursive) == 0u) return kMutexAlreadyLocked;
            if (static_cast<std::int64_t>(mutex.lock_count) + count > 0x7FFFFFFF) return kMutexLockOverflow;
            mutex.lock_count += count;
            return 0u;
        }
        return kMutexTryLockFailed;
    };
    runtime.register_hle("ThreadManForUser", 0xB011B11Fu,  // sceKernelLockMutex
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto count = static_cast<std::int32_t>(ctx.gpr[5]);
            const auto found = mutex_table.mutexes.find(uid);
            if (found == mutex_table.mutexes.end()) {
                ctx.set_gpr(2, kMutexNotFound);
                return;
            }
            if (count <= 0 || (count > 1 && (found->second.attributes & kMutexRecursive) == 0u)) {
                ctx.set_gpr(2, kMutexLockOverflow);
                return;
            }
            const std::uint32_t result = try_acquire_mutex(found->second, count);
            if (result != kMutexTryLockFailed) {
                ctx.set_gpr(2, result);
                return;
            }
            const psprecomp::AllegrexContext suspended = make_wait_context(ctx);
            found->second.waiters.push_back(MutexWaiter{thread_table.current_uid, suspended, count});
            (void)suspend_current_thread(rt, ctx, suspended, "mutex " + std::to_string(uid));
        });
    runtime.register_hle("ThreadManForUser", 0x0DDCD2C9u,  // sceKernelTryLockMutex
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto count = static_cast<std::int32_t>(ctx.gpr[5]);
            const auto found = mutex_table.mutexes.find(static_cast<std::int32_t>(ctx.gpr[4]));
            if (found == mutex_table.mutexes.end()) {
                ctx.set_gpr(2, kMutexNotFound);
                return;
            }
            if (count <= 0 || (count > 1 && (found->second.attributes & kMutexRecursive) == 0u)) {
                ctx.set_gpr(2, kMutexLockOverflow);
                return;
            }
            ctx.set_gpr(2, try_acquire_mutex(found->second, count));
        });
    runtime.register_hle("ThreadManForUser", 0x6B30100Fu,  // sceKernelUnlockMutex
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto count = static_cast<std::int32_t>(ctx.gpr[5]);
            const auto found = mutex_table.mutexes.find(static_cast<std::int32_t>(ctx.gpr[4]));
            if (found == mutex_table.mutexes.end()) {
                ctx.set_gpr(2, kMutexNotFound);
                return;
            }
            MutexRecord &mutex = found->second;
            if (count <= 0) {
                ctx.set_gpr(2, kMutexUnlockUnderflow);
                return;
            }
            if (mutex.lock_count == 0 || mutex.owner_uid != thread_table.current_uid) {
                ctx.set_gpr(2, kMutexNotOwned);
                return;
            }
            if (count > mutex.lock_count) {
                ctx.set_gpr(2, kMutexUnlockUnderflow);
                return;
            }
            mutex.lock_count -= count;
            if (mutex.lock_count == 0) {
                mutex.owner_uid = -1;
                if (!mutex.waiters.empty()) {
                    MutexWaiter next = std::move(mutex.waiters.front());
                    mutex.waiters.erase(mutex.waiters.begin());
                    mutex.owner_uid = next.uid;
                    mutex.lock_count = next.count;
                    next.context.set_gpr(2, 0u);
                    enqueue_continuation(next.uid, next.context);
                }
            }
            set_success(ctx);
            (void)preempt_if_higher_priority(ctx, "mutex-unlock");
        });

}

} // namespace psprecomp::hle

#include "psprecomp/guest_memory.hpp"
#include "psprecomp/common.hpp"

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>

namespace psprecomp {

// Declared in runtime.hpp/runtime.cpp.  guest_memory.cpp intentionally avoids
// including runtime.hpp because Runtime owns a GuestMemory instance.
std::int32_t runtime_thread_uid() noexcept;
const char *runtime_thread_name() noexcept;
std::uint32_t runtime_dispatch_pc() noexcept;

namespace {
struct WriteWatch {
    bool enabled{};
    std::uint32_t address{};
    std::uint32_t size{4u};
};

const WriteWatch &write_watch() {
    static const WriteWatch watch = [] {
        WriteWatch result{};
        const char *text = std::getenv("PSPRECOMP_WATCH_WRITE");
        if (text == nullptr || *text == '\0') return result;
        char *end = nullptr;
        const unsigned long address = std::strtoul(text, &end, 0);
        if (end == text || *end != '\0' || address > 0xFFFFFFFFul) return result;
        result.enabled = true;
        result.address = static_cast<std::uint32_t>(address);
        if (const char *size_text = std::getenv("PSPRECOMP_WATCH_WRITE_SIZE")) {
            char *size_end = nullptr;
            const unsigned long size = std::strtoul(size_text, &size_end, 0);
            if (size_end != size_text && *size_end == '\0' && size != 0ul && size <= 0xFFFFFFFFul)
                result.size = static_cast<std::uint32_t>(size);
        }
        return result;
    }();
    return watch;
}

bool overlaps_watch(std::uint32_t address, std::size_t length) {
    const WriteWatch &watch = write_watch();
    if (!watch.enabled || length == 0u) return false;
    const std::uint32_t canonical_address = address & 0x1FFFFFFFu;
    const std::uint32_t canonical_watch = watch.address & 0x1FFFFFFFu;
    const std::uint64_t first_end = static_cast<std::uint64_t>(canonical_address) + length;
    const std::uint64_t watch_end = static_cast<std::uint64_t>(canonical_watch) + watch.size;
    return static_cast<std::uint64_t>(canonical_address) < watch_end &&
           static_cast<std::uint64_t>(canonical_watch) < first_end;
}

void log_write_watch(std::uint32_t address, std::size_t length, const char *operation,
                     std::uint64_t old_value, std::uint64_t new_value) {
    if (!overlaps_watch(address, length)) return;
    std::cerr << "[watch-write] uid=" << runtime_thread_uid()
              << " name=" << runtime_thread_name()
              << " pc=" << hex32(runtime_dispatch_pc())
              << " op=" << operation
              << " address=" << hex32(address)
              << " size=" << length
              << " old=0x" << std::hex << old_value
              << " new=0x" << new_value << std::dec << "\n";
}
}

GuestMemory::GuestMemory(std::uint32_t size_bytes)
    : vram_(kVramSize, 0u), bytes_(size_bytes, 0u), scratchpad_(kScratchpadSize, 0u), write_watch_enabled_(std::getenv("PSPRECOMP_WATCH_WRITE") != nullptr) {
    if (size_bytes != 32u * 1024u * 1024u && size_bytes != 64u * 1024u * 1024u) {
        throw Error("PSP RAM size must be 32 MiB or 64 MiB");
    }
    // Bind the inline AOT fast paths to main RAM.  bytes_ is never resized
    // afterwards, and the instance is non-copyable, so this stays valid.
    ram_data_ = bytes_.data();
    ram_limit8_ = size_bytes - 1u;
    ram_limit16_ = size_bytes - 2u;
    ram_limit32_ = size_bytes - 4u;
}

std::uint32_t GuestMemory::size() const noexcept { return static_cast<std::uint32_t>(bytes_.size()); }
std::uint32_t GuestMemory::vram_size() const noexcept { return static_cast<std::uint32_t>(vram_.size()); }

bool GuestMemory::is_vram_window(std::uint32_t canonical_address) const noexcept {
    return canonical_address >= kVramPhysicalBase &&
           canonical_address < kVramPhysicalBase + kVramAddressSpan;
}

std::size_t GuestMemory::vram_offset(std::uint32_t canonical_address) const noexcept {
    return static_cast<std::size_t>((canonical_address - kVramPhysicalBase) & (kVramSize - 1u));
}

bool GuestMemory::contains(std::uint32_t address, std::size_t length) const noexcept {
    const std::uint32_t c = canonical(address);
    const std::uint64_t end = static_cast<std::uint64_t>(c) + static_cast<std::uint64_t>(length);
    if (is_vram_window(c) && end <= static_cast<std::uint64_t>(kVramPhysicalBase) + kVramAddressSpan)
        return true;
    if (c >= kPhysicalBase && end <= static_cast<std::uint64_t>(kPhysicalBase) + bytes_.size())
        return true;
    if (is_scratchpad_window(c) && end <= static_cast<std::uint64_t>(kScratchpadPhysicalBase) + kScratchpadSize)
        return true;
    return false;
}

GuestMemory::ResolvedAddress GuestMemory::resolve(std::uint32_t address, std::size_t length) const {
    if (!contains(address, length)) {
        throw Error("Guest memory access outside PSP RAM/EDRAM at " + hex32(address));
    }
    const std::uint32_t c = canonical(address);
    if (is_vram_window(c))
        return {Region::Vram, vram_offset(c)};
    if (is_scratchpad_window(c))
        return {Region::Scratchpad, static_cast<std::size_t>(c - kScratchpadPhysicalBase)};
    return {Region::Ram, static_cast<std::size_t>(c - kPhysicalBase)};
}

const std::vector<std::uint8_t> &GuestMemory::region_bytes(Region region) const noexcept {
    return region == Region::Vram ? vram_ : region == Region::Scratchpad ? scratchpad_ : bytes_;
}
std::vector<std::uint8_t> &GuestMemory::region_bytes(Region region) noexcept {
    return region == Region::Vram ? vram_ : region == Region::Scratchpad ? scratchpad_ : bytes_;
}

// The `_slow` bodies below are the original aot_* implementations, reached only
// when the inline main-RAM fast path in the header declines the access: EDRAM,
// an out-of-range address, a region-crossing width, or an armed write watch.
std::uint8_t GuestMemory::aot_load8_slow(std::uint32_t address) const {
    const std::uint32_t c = canonical(address);
    if (is_vram_window(c)) return vram_[vram_offset(c)];
    if (c >= kPhysicalBase && c - kPhysicalBase < bytes_.size())
        return bytes_[static_cast<std::size_t>(c - kPhysicalBase)];
    return load8(address);
}

std::uint16_t GuestMemory::aot_load16_slow(std::uint32_t address) const {
    const std::uint32_t c = canonical(address);
    if (is_vram_window(c)) {
        const std::size_t offset = vram_offset(c);
        if (offset + 2u <= vram_.size())
            return static_cast<std::uint16_t>(vram_[offset]) |
                   static_cast<std::uint16_t>(static_cast<std::uint16_t>(vram_[offset + 1u]) << 8u);
    } else if (c >= kPhysicalBase) {
        const std::size_t offset = static_cast<std::size_t>(c - kPhysicalBase);
        if (offset + 2u <= bytes_.size())
            return static_cast<std::uint16_t>(bytes_[offset]) |
                   static_cast<std::uint16_t>(static_cast<std::uint16_t>(bytes_[offset + 1u]) << 8u);
    }
    return load16(address);
}

std::uint32_t GuestMemory::aot_load32_slow(std::uint32_t address) const {
    const std::uint32_t c = canonical(address);
    const std::vector<std::uint8_t> *data = nullptr;
    std::size_t offset = 0u;
    if (is_vram_window(c)) {
        data = &vram_;
        offset = vram_offset(c);
    } else if (c >= kPhysicalBase) {
        data = &bytes_;
        offset = static_cast<std::size_t>(c - kPhysicalBase);
    }
    if (data != nullptr && offset + 4u <= data->size()) {
        return static_cast<std::uint32_t>((*data)[offset]) |
               (static_cast<std::uint32_t>((*data)[offset + 1u]) << 8u) |
               (static_cast<std::uint32_t>((*data)[offset + 2u]) << 16u) |
               (static_cast<std::uint32_t>((*data)[offset + 3u]) << 24u);
    }
    return load32(address);
}

std::uint32_t GuestMemory::aot_load_word_left(std::uint32_t address, std::uint32_t existing) const {
    const std::uint32_t shift = (address & 3u) * 8u;
    const std::uint32_t memory_word = aot_load32(address & ~3u);
    return (existing & (0x00FFFFFFu >> shift)) | (memory_word << (24u - shift));
}
std::uint32_t GuestMemory::aot_load_word_right(std::uint32_t address, std::uint32_t existing) const {
    const std::uint32_t shift = (address & 3u) * 8u;
    const std::uint32_t memory_word = aot_load32(address & ~3u);
    return (existing & (0xFFFFFF00u << (24u - shift))) | (memory_word >> shift);
}

void GuestMemory::aot_store8_slow(std::uint32_t address, std::uint8_t value) {
    if (write_watch_enabled_) { store8(address, value); return; }
    const std::uint32_t c = canonical(address);
    if (is_vram_window(c)) { vram_[vram_offset(c)] = value; return; }
    if (c >= kPhysicalBase && c - kPhysicalBase < bytes_.size()) {
        bytes_[static_cast<std::size_t>(c - kPhysicalBase)] = value;
        return;
    }
    store8(address, value);
}
void GuestMemory::aot_store16_slow(std::uint32_t address, std::uint16_t value) {
    if (write_watch_enabled_) { store16(address, value); return; }
    const std::uint32_t c = canonical(address);
    std::vector<std::uint8_t> *data = nullptr;
    std::size_t offset = 0u;
    if (is_vram_window(c)) { data = &vram_; offset = vram_offset(c); }
    else if (c >= kPhysicalBase) { data = &bytes_; offset = static_cast<std::size_t>(c - kPhysicalBase); }
    if (data != nullptr && offset + 2u <= data->size()) {
        (*data)[offset] = static_cast<std::uint8_t>(value & 0xFFu);
        (*data)[offset + 1u] = static_cast<std::uint8_t>((value >> 8u) & 0xFFu);
        return;
    }
    store16(address, value);
}
void GuestMemory::aot_store32_slow(std::uint32_t address, std::uint32_t value) {
    if (write_watch_enabled_) { store32(address, value); return; }
    const std::uint32_t c = canonical(address);
    std::vector<std::uint8_t> *data = nullptr;
    std::size_t offset = 0u;
    if (is_vram_window(c)) { data = &vram_; offset = vram_offset(c); }
    else if (c >= kPhysicalBase) { data = &bytes_; offset = static_cast<std::size_t>(c - kPhysicalBase); }
    if (data != nullptr && offset + 4u <= data->size()) {
        (*data)[offset] = static_cast<std::uint8_t>(value & 0xFFu);
        (*data)[offset + 1u] = static_cast<std::uint8_t>((value >> 8u) & 0xFFu);
        (*data)[offset + 2u] = static_cast<std::uint8_t>((value >> 16u) & 0xFFu);
        (*data)[offset + 3u] = static_cast<std::uint8_t>((value >> 24u) & 0xFFu);
        return;
    }
    store32(address, value);
}
void GuestMemory::aot_store_word_left(std::uint32_t address, std::uint32_t value) {
    const std::uint32_t shift = (address & 3u) * 8u;
    const std::uint32_t aligned = address & ~3u;
    const std::uint32_t memory_word = aot_load32(aligned);
    aot_store32(aligned, (value >> (24u - shift)) | (memory_word & (0xFFFFFF00u << shift)));
}
void GuestMemory::aot_store_word_right(std::uint32_t address, std::uint32_t value) {
    const std::uint32_t shift = (address & 3u) * 8u;
    const std::uint32_t aligned = address & ~3u;
    const std::uint32_t memory_word = aot_load32(aligned);
    aot_store32(aligned, (value << shift) | (memory_word & (0x00FFFFFFu >> (24u - shift))));
}

void GuestMemory::aot_copy_lz_match(std::uint32_t destination, std::uint32_t source,
                                    std::uint32_t length) {
    if (length == 0u) return;

    const std::uint32_t canonical_destination = canonical(destination);
    const std::uint32_t canonical_source = canonical(source);
    if (canonical_source >= canonical_destination)
        throw Error("Invalid forward LZ match from " + hex32(source) + " to " + hex32(destination));

    // Write watches and mirrored EDRAM boundaries need the ordinary accessors
    // so every byte retains the same observability and wrapping behavior.
    const auto bytewise_copy = [&] {
        for (std::uint32_t index = 0u; index < length; ++index)
            aot_store8(destination + index, aot_load8(source + index));
    };
    if (write_watch_enabled_) {
        bytewise_copy();
        return;
    }

    const ResolvedAddress destination_resolved = resolve(destination, length);
    const ResolvedAddress source_resolved = resolve(source, length);
    if (destination_resolved.region != source_resolved.region) {
        bytewise_copy();
        return;
    }

    auto &data = region_bytes(destination_resolved.region);
    if (destination_resolved.offset + length > data.size() ||
        source_resolved.offset + length > data.size() ||
        source_resolved.offset >= destination_resolved.offset) {
        bytewise_copy();
        return;
    }

    const std::size_t destination_offset = destination_resolved.offset;
    const std::size_t source_offset = source_resolved.offset;
    const std::size_t total = static_cast<std::size_t>(length);
    const std::size_t distance = destination_offset - source_offset;

    // Seed one full match-distance (or the entire short copy), then duplicate
    // the already produced prefix in geometrically growing non-overlapping
    // chunks. This is equivalent to the guest's forward byte loop, including
    // distance=1 runs, but completes in O(log(length)) host copies.
    std::size_t copied = std::min(distance, total);
    std::memcpy(data.data() + destination_offset, data.data() + source_offset, copied);
    while (copied < total) {
        const std::size_t chunk = std::min(copied, total - copied);
        std::memcpy(data.data() + destination_offset + copied, data.data() + destination_offset, chunk);
        copied += chunk;
    }
}

std::uint8_t *GuestMemory::raw_pointer(std::uint32_t address, std::size_t length) noexcept {
    return const_cast<std::uint8_t *>(
        static_cast<const GuestMemory *>(this)->raw_pointer(address, length));
}

const std::uint8_t *GuestMemory::raw_pointer(std::uint32_t address, std::size_t length) const noexcept {
    const std::uint32_t c = canonical(address);
    if (is_vram_window(c)) {
        const std::size_t offset = vram_offset(c);
        // A run that would wrap past the end of the 2 MiB EDRAM image is not
        // contiguous in host memory even though it is legal in guest space.
        if (offset + length <= vram_.size()) return vram_.data() + offset;
        return nullptr;
    }
    if (is_scratchpad_window(c)) {
        const std::size_t offset = static_cast<std::size_t>(c - kScratchpadPhysicalBase);
        if (offset + length <= scratchpad_.size()) return scratchpad_.data() + offset;
        return nullptr;
    }
    if (c < kPhysicalBase) return nullptr;
    const std::size_t offset = static_cast<std::size_t>(c - kPhysicalBase);
    if (offset + length <= bytes_.size()) return bytes_.data() + offset;
    return nullptr;
}

std::uint8_t GuestMemory::load8(std::uint32_t address) const {
    const auto r = resolve(address, 1u);
    return region_bytes(r.region)[r.offset];
}
std::uint16_t GuestMemory::load16(std::uint32_t address) const {
    return static_cast<std::uint16_t>(load8(address)) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(load8(address + 1u)) << 8u);
}
std::uint32_t GuestMemory::load32(std::uint32_t address) const {
    return static_cast<std::uint32_t>(load8(address)) |
           (static_cast<std::uint32_t>(load8(address + 1u)) << 8u) |
           (static_cast<std::uint32_t>(load8(address + 2u)) << 16u) |
           (static_cast<std::uint32_t>(load8(address + 3u)) << 24u);
}
std::uint32_t GuestMemory::load_word_left(std::uint32_t address, std::uint32_t existing) const {
    const std::uint32_t shift = (address & 3u) * 8u;
    const std::uint32_t memory_word = load32(address & ~3u);
    return (existing & (0x00FFFFFFu >> shift)) | (memory_word << (24u - shift));
}
std::uint32_t GuestMemory::load_word_right(std::uint32_t address, std::uint32_t existing) const {
    const std::uint32_t shift = (address & 3u) * 8u;
    const std::uint32_t memory_word = load32(address & ~3u);
    return (existing & (0xFFFFFF00u << (24u - shift))) | (memory_word >> shift);
}
void GuestMemory::store8(std::uint32_t address, std::uint8_t value) {
    const auto r = resolve(address, 1u);
    auto &data = region_bytes(r.region);
    const std::uint8_t old = data[r.offset];
    log_write_watch(address, 1u, "store8", old, value);
    data[r.offset] = value;
}
void GuestMemory::store16(std::uint32_t address, std::uint16_t value) {
    const std::uint16_t old = load16(address);
    log_write_watch(address, 2u, "store16", old, value);
    const auto write_byte = [this](std::uint32_t byte_address, std::uint8_t byte) {
        const auto r = resolve(byte_address, 1u);
        region_bytes(r.region)[r.offset] = byte;
    };
    write_byte(address, static_cast<std::uint8_t>(value & 0xFFu));
    write_byte(address + 1u, static_cast<std::uint8_t>((value >> 8u) & 0xFFu));
}
void GuestMemory::store32(std::uint32_t address, std::uint32_t value) {
    const std::uint32_t old = load32(address);
    log_write_watch(address, 4u, "store32", old, value);
    const auto write_byte = [this](std::uint32_t byte_address, std::uint8_t byte) {
        const auto r = resolve(byte_address, 1u);
        region_bytes(r.region)[r.offset] = byte;
    };
    write_byte(address, static_cast<std::uint8_t>(value & 0xFFu));
    write_byte(address + 1u, static_cast<std::uint8_t>((value >> 8u) & 0xFFu));
    write_byte(address + 2u, static_cast<std::uint8_t>((value >> 16u) & 0xFFu));
    write_byte(address + 3u, static_cast<std::uint8_t>((value >> 24u) & 0xFFu));
}
void GuestMemory::store_word_left(std::uint32_t address, std::uint32_t value) {
    const std::uint32_t shift = (address & 3u) * 8u;
    const std::uint32_t aligned = address & ~3u;
    const std::uint32_t memory_word = load32(aligned);
    store32(aligned, (value >> (24u - shift)) | (memory_word & (0xFFFFFF00u << shift)));
}
void GuestMemory::store_word_right(std::uint32_t address, std::uint32_t value) {
    const std::uint32_t shift = (address & 3u) * 8u;
    const std::uint32_t aligned = address & ~3u;
    const std::uint32_t memory_word = load32(aligned);
    store32(aligned, (value << shift) | (memory_word & (0x00FFFFFFu >> (24u - shift))));
}
void GuestMemory::memory_barrier() const noexcept {
    std::atomic_thread_fence(std::memory_order_seq_cst);
}
void GuestMemory::copy_in(std::uint32_t address, std::span<const std::uint8_t> source) {
    if (!contains(address, source.size()))
        throw Error("Guest memory access outside PSP RAM/EDRAM at " + hex32(address));
    log_write_watch(address, source.size(), "copy_in", 0u, 0u);
    std::size_t copied = 0u;
    while (copied < source.size()) {
        const std::uint32_t current = address + static_cast<std::uint32_t>(copied);
        const auto r = resolve(current, 1u);
        auto &data = region_bytes(r.region);
        const std::size_t chunk = std::min(source.size() - copied, data.size() - r.offset);
        std::copy_n(source.begin() + static_cast<std::ptrdiff_t>(copied), chunk,
                    data.begin() + static_cast<std::ptrdiff_t>(r.offset));
        copied += chunk;
    }
}
void GuestMemory::copy_out(std::uint32_t address, std::span<std::uint8_t> destination) const {
    if (!contains(address, destination.size()))
        throw Error("Guest memory access outside PSP RAM/EDRAM at " + hex32(address));
    std::size_t copied = 0u;
    while (copied < destination.size()) {
        const std::uint32_t current = address + static_cast<std::uint32_t>(copied);
        const auto r = resolve(current, 1u);
        const auto &data = region_bytes(r.region);
        const std::size_t chunk = std::min(destination.size() - copied, data.size() - r.offset);
        std::copy_n(data.begin() + static_cast<std::ptrdiff_t>(r.offset), chunk,
                    destination.begin() + static_cast<std::ptrdiff_t>(copied));
        copied += chunk;
    }
}
void GuestMemory::zero(std::uint32_t address, std::size_t length) {
    if (!contains(address, length))
        throw Error("Guest memory access outside PSP RAM/EDRAM at " + hex32(address));
    log_write_watch(address, length, "zero", 0u, 0u);
    std::size_t cleared = 0u;
    while (cleared < length) {
        const std::uint32_t current = address + static_cast<std::uint32_t>(cleared);
        const auto r = resolve(current, 1u);
        auto &data = region_bytes(r.region);
        const std::size_t chunk = std::min(length - cleared, data.size() - r.offset);
        std::fill_n(data.begin() + static_cast<std::ptrdiff_t>(r.offset), chunk, 0u);
        cleared += chunk;
    }
}
std::string GuestMemory::read_c_string(std::uint32_t address, std::size_t max_length) const {
    std::string out;
    out.reserve(std::min<std::size_t>(max_length, 64u));
    for (std::size_t i = 0; i < max_length; ++i) {
        const char ch = static_cast<char>(load8(address + static_cast<std::uint32_t>(i)));
        if (ch == '\0') return out;
        out.push_back(ch);
    }
    throw Error("Unterminated guest string at " + hex32(address));
}
const std::vector<std::uint8_t> &GuestMemory::bytes() const noexcept { return bytes_; }
const std::vector<std::uint8_t> &GuestMemory::vram_bytes() const noexcept { return vram_; }

} // namespace psprecomp

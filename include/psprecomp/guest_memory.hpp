#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <string>
#include <vector>

namespace psprecomp {

// Each generated AOT unit is one enormous function, and MSVC stops inlining
// into a caller that large -- measured: moving the fast paths into this header
// grew the linked image by only 2 KB, so the calls survived.  Roughly a third of
// the translated instructions are guest loads and stores, so the call has to go.
#if defined(_MSC_VER)
#define PSPRECOMP_MEMORY_FAST_PATH __forceinline
#else
#define PSPRECOMP_MEMORY_FAST_PATH inline __attribute__((always_inline))
#endif

class GuestMemory {
public:
    static constexpr std::uint32_t kVramPhysicalBase = 0x04000000u;
    static constexpr std::uint32_t kVramSize = 2u * 1024u * 1024u;
    static constexpr std::uint32_t kVramMirrorCount = 4u;
    static constexpr std::uint32_t kVramAddressSpan = kVramSize * kVramMirrorCount;
    static constexpr std::uint32_t kPhysicalBase = 0x08000000u;
    // 16 KiB on-chip scratchpad RAM (0x00010000, uncached alias 0x40010000).
    static constexpr std::uint32_t kScratchpadPhysicalBase = 0x00010000u;
    static constexpr std::uint32_t kScratchpadSize = 16u * 1024u;

    explicit GuestMemory(std::uint32_t size_bytes = 32u * 1024u * 1024u);

    // The AOT fast paths below index cached region pointers, so an instance may
    // not be relocated after construction.  Runtime owns exactly one by value
    // and never copies it; making that explicit turns a future copy into a
    // compile error instead of a dangling read.
    GuestMemory(const GuestMemory &) = delete;
    GuestMemory &operator=(const GuestMemory &) = delete;
    GuestMemory(GuestMemory &&) = delete;
    GuestMemory &operator=(GuestMemory &&) = delete;

    [[nodiscard]] std::uint32_t size() const noexcept;
    [[nodiscard]] std::uint32_t vram_size() const noexcept;
    // Allegrex uses cached/uncached MIPS aliases; this maps addresses such as
    // 0x44000000 and 0x88000000 to the physical VRAM/RAM windows.
    //
    // Inline for the same reason as the AOT fast paths below: it is one AND, and
    // Runtime::invoke_chained_call performs it on every translated guest call
    // from 234 separate translation units, where an out-of-line accessor is a
    // real call into psprecomp_core.
    [[nodiscard]] static constexpr std::uint32_t canonical(std::uint32_t address) noexcept {
        return address & 0x1FFFFFFFu;
    }
    [[nodiscard]] bool contains(std::uint32_t address, std::size_t length = 1u) const noexcept;

    // cached AOT memory view. Generated units contain hundreds to
    // thousands of guest loads/stores each. Calling the inline GuestMemory
    // accessors still asks the optimizer to rediscover ram_data_, three limits
    // and the immutable write-watch flag at every static site. Materialize those
    // values once when a unit is entered, then keep them as ordinary locals that
    // MSVC/LTCG can retain in registers across the unit's basic blocks.
    //
    // The underlying RAM/VRAM vectors never resize after construction and
    // write_watch_enabled_ is fixed by the constructor, so this view remains
    // valid across nested AOT/HLE calls. Slow/VRAM paths delegate to the owning
    // GuestMemory and preserve the original validation/watch behavior.
    class AotFastView {
    public:
        [[nodiscard]] PSPRECOMP_MEMORY_FAST_PATH std::uint8_t aot_load8(std::uint32_t address) const {
            const std::uint32_t offset = ram_offset_of_fast(address);
            if (offset <= ram_limit8_) return ram_data_[offset];
            return owner_->aot_load8_slow(address);
        }
        [[nodiscard]] PSPRECOMP_MEMORY_FAST_PATH std::uint16_t aot_load16(std::uint32_t address) const {
            const std::uint32_t offset = ram_offset_of_fast(address);
            if (offset <= ram_limit16_) return GuestMemory::read_le16(ram_data_ + offset);
            return owner_->aot_load16_slow(address);
        }
        [[nodiscard]] PSPRECOMP_MEMORY_FAST_PATH std::uint32_t aot_load32(std::uint32_t address) const {
            const std::uint32_t offset = ram_offset_of_fast(address);
            if (offset <= ram_limit32_) return GuestMemory::read_le32(ram_data_ + offset);
            return owner_->aot_load32_slow(address);
        }
        PSPRECOMP_MEMORY_FAST_PATH void aot_store8(std::uint32_t address, std::uint8_t value) const {
            const std::uint32_t offset = ram_offset_of_fast(address);
#if defined(PSPRECOMP_AOT_ASSUME_NO_WRITE_WATCH)
            if (offset <= ram_limit8_) {
#else
            if (!write_watch_enabled_ && offset <= ram_limit8_) {
#endif
                ram_data_[offset] = value;
                return;
            }
            owner_->aot_store8_slow(address, value);
        }
        PSPRECOMP_MEMORY_FAST_PATH void aot_store16(std::uint32_t address, std::uint16_t value) const {
            const std::uint32_t offset = ram_offset_of_fast(address);
#if defined(PSPRECOMP_AOT_ASSUME_NO_WRITE_WATCH)
            if (offset <= ram_limit16_) {
#else
            if (!write_watch_enabled_ && offset <= ram_limit16_) {
#endif
                GuestMemory::write_le16(ram_data_ + offset, value);
                return;
            }
            owner_->aot_store16_slow(address, value);
        }
        PSPRECOMP_MEMORY_FAST_PATH void aot_store32(std::uint32_t address, std::uint32_t value) const {
            const std::uint32_t offset = ram_offset_of_fast(address);
#if defined(PSPRECOMP_AOT_ASSUME_NO_WRITE_WATCH)
            if (offset <= ram_limit32_) {
#else
            if (!write_watch_enabled_ && offset <= ram_limit32_) {
#endif
                GuestMemory::write_le32(ram_data_ + offset, value);
                return;
            }
            owner_->aot_store32_slow(address, value);
        }

    private:
        friend class GuestMemory;
        AotFastView(GuestMemory *owner, std::uint8_t *ram_data,
                    std::uint32_t limit8, std::uint32_t limit16,
                    std::uint32_t limit32, bool write_watch) noexcept
            : owner_(owner), ram_data_(ram_data), ram_limit8_(limit8),
              ram_limit16_(limit16), ram_limit32_(limit32),
              write_watch_enabled_(write_watch) {}
        [[nodiscard]] PSPRECOMP_MEMORY_FAST_PATH static constexpr std::uint32_t ram_offset_of_fast(
            std::uint32_t address) noexcept {
            return (address & 0x1FFFFFFFu) - GuestMemory::kPhysicalBase;
        }
        GuestMemory *owner_{};
        std::uint8_t *ram_data_{};
        std::uint32_t ram_limit8_{};
        std::uint32_t ram_limit16_{};
        std::uint32_t ram_limit32_{};
        bool write_watch_enabled_{};
    };

    [[nodiscard]] PSPRECOMP_MEMORY_FAST_PATH AotFastView aot_fast_view() noexcept {
        return AotFastView(this, ram_data_, ram_limit8_, ram_limit16_, ram_limit32_,
                           write_watch_enabled_);
    }

    // Fast paths used only by statically generated AOT code. They retain
    // strict fallback behavior for invalid/cross-boundary accesses and write watches.
    //
    // These are defined inline because the generated units are separate
    // translation units linking against psprecomp_core without LTCG: an
    // out-of-line accessor turned every guest load and store into a real call,
    // and roughly a third of the translated instructions are memory accesses.
    // The inline body covers only main RAM, which is where the overwhelming
    // majority of guest traffic goes.  EDRAM, out-of-range and write-watched
    // accesses fall through to the out-of-line helpers, which keep exactly the
    // behavior these functions had when they lived entirely in the .cpp.
    [[nodiscard]] PSPRECOMP_MEMORY_FAST_PATH std::uint8_t aot_load8(std::uint32_t address) const {
        const std::uint32_t offset = ram_offset_of(address);
        if (offset <= ram_limit8_) return ram_data_[offset];
        return aot_load8_slow(address);
    }
    [[nodiscard]] PSPRECOMP_MEMORY_FAST_PATH std::uint16_t aot_load16(std::uint32_t address) const {
        const std::uint32_t offset = ram_offset_of(address);
        if (offset <= ram_limit16_) return read_le16(ram_data_ + offset);
        return aot_load16_slow(address);
    }
    [[nodiscard]] PSPRECOMP_MEMORY_FAST_PATH std::uint32_t aot_load32(std::uint32_t address) const {
        const std::uint32_t offset = ram_offset_of(address);
        if (offset <= ram_limit32_) return read_le32(ram_data_ + offset);
        return aot_load32_slow(address);
    }
    [[nodiscard]] std::uint32_t aot_load_word_left(std::uint32_t address, std::uint32_t existing) const;
    [[nodiscard]] std::uint32_t aot_load_word_right(std::uint32_t address, std::uint32_t existing) const;
    PSPRECOMP_MEMORY_FAST_PATH void aot_store8(std::uint32_t address, std::uint8_t value) {
        const std::uint32_t offset = ram_offset_of(address);
#if defined(PSPRECOMP_AOT_ASSUME_NO_WRITE_WATCH)
        if (offset <= ram_limit8_) {
#else
        if (!write_watch_enabled_ && offset <= ram_limit8_) {
#endif
            ram_data_[offset] = value;
            return;
        }
        aot_store8_slow(address, value);
    }
    PSPRECOMP_MEMORY_FAST_PATH void aot_store16(std::uint32_t address, std::uint16_t value) {
        const std::uint32_t offset = ram_offset_of(address);
#if defined(PSPRECOMP_AOT_ASSUME_NO_WRITE_WATCH)
        if (offset <= ram_limit16_) {
#else
        if (!write_watch_enabled_ && offset <= ram_limit16_) {
#endif
            write_le16(ram_data_ + offset, value);
            return;
        }
        aot_store16_slow(address, value);
    }
    PSPRECOMP_MEMORY_FAST_PATH void aot_store32(std::uint32_t address, std::uint32_t value) {
        const std::uint32_t offset = ram_offset_of(address);
#if defined(PSPRECOMP_AOT_ASSUME_NO_WRITE_WATCH)
        if (offset <= ram_limit32_) {
#else
        if (!write_watch_enabled_ && offset <= ram_limit32_) {
#endif
            write_le32(ram_data_ + offset, value);
            return;
        }
        aot_store32_slow(address, value);
    }
    void aot_store_word_left(std::uint32_t address, std::uint32_t value);
    void aot_store_word_right(std::uint32_t address, std::uint32_t value);

    // DEFLATE/LZ-style forward overlap copy. Unlike memmove, bytes written at
    // the destination become immediately available as source bytes, so a short
    // match (for example distance=1) can expand to a long repeated run.
    // Generated AOT can use this for verified overlap-aware match-copy loops such as
    // hot guest loops instead of executing one translated load/store per byte.
    void aot_copy_lz_match(std::uint32_t destination, std::uint32_t source,
                           std::uint32_t length);

    // Direct pointer to `length` contiguous bytes, or nullptr when the range is
    // not wholly inside one region.  The software rasterizer resolves the frame
    // and depth buffers once per primitive and then indexes raw memory, instead
    // of paying canonicalization plus a bounds check on every pixel.  Callers
    // must keep the range valid; nothing here is revalidated afterwards.
    [[nodiscard]] std::uint8_t *raw_pointer(std::uint32_t address, std::size_t length) noexcept;
    [[nodiscard]] const std::uint8_t *raw_pointer(std::uint32_t address, std::size_t length) const noexcept;

    [[nodiscard]] std::uint8_t load8(std::uint32_t address) const;
    [[nodiscard]] std::uint16_t load16(std::uint32_t address) const;
    [[nodiscard]] std::uint32_t load32(std::uint32_t address) const;
    [[nodiscard]] std::uint32_t load_word_left(std::uint32_t address, std::uint32_t existing) const;
    [[nodiscard]] std::uint32_t load_word_right(std::uint32_t address, std::uint32_t existing) const;
    void store8(std::uint32_t address, std::uint8_t value);
    void store16(std::uint32_t address, std::uint16_t value);
    void store32(std::uint32_t address, std::uint32_t value);
    void store_word_left(std::uint32_t address, std::uint32_t value);
    void store_word_right(std::uint32_t address, std::uint32_t value);
    void memory_barrier() const noexcept;
    void copy_in(std::uint32_t address, std::span<const std::uint8_t> data);
    void copy_out(std::uint32_t address, std::span<std::uint8_t> data) const;
    void zero(std::uint32_t address, std::size_t length);

    [[nodiscard]] std::string read_c_string(std::uint32_t address, std::size_t max_length = 256u) const;
    [[nodiscard]] const std::vector<std::uint8_t> &bytes() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t> &vram_bytes() const noexcept;

private:
    enum class Region { Vram, Ram, Scratchpad };
    struct ResolvedAddress {
        Region region;
        std::size_t offset;
    };

    [[nodiscard]] ResolvedAddress resolve(std::uint32_t address, std::size_t length) const;
    [[nodiscard]] bool is_vram_window(std::uint32_t canonical_address) const noexcept;
    [[nodiscard]] static constexpr bool is_scratchpad_window(std::uint32_t canonical_address) noexcept {
        return canonical_address - kScratchpadPhysicalBase < kScratchpadSize;
    }
    [[nodiscard]] std::size_t vram_offset(std::uint32_t canonical_address) const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t> &region_bytes(Region region) const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> &region_bytes(Region region) noexcept;

    // Canonicalize and rebase in one step.  An address below kPhysicalBase --
    // EDRAM included -- wraps to a value far above any RAM size, so a single
    // unsigned compare rejects it along with every out-of-range access.
    [[nodiscard]] PSPRECOMP_MEMORY_FAST_PATH static std::uint32_t ram_offset_of(std::uint32_t address) noexcept {
        return (address & 0x1FFFFFFFu) - kPhysicalBase;
    }

    // Guest memory is little-endian, so on a little-endian host these are the
    // same bytes the previous per-byte assembly produced, in one access.
    [[nodiscard]] PSPRECOMP_MEMORY_FAST_PATH static std::uint16_t read_le16(const std::uint8_t *source) noexcept {
        std::uint16_t value{};
        std::memcpy(&value, source, sizeof(value));
        if constexpr (std::endian::native == std::endian::big)
            value = static_cast<std::uint16_t>((value >> 8u) | (value << 8u));
        return value;
    }
    [[nodiscard]] PSPRECOMP_MEMORY_FAST_PATH static std::uint32_t read_le32(const std::uint8_t *source) noexcept {
        std::uint32_t value{};
        std::memcpy(&value, source, sizeof(value));
        if constexpr (std::endian::native == std::endian::big)
            value = ((value >> 24u) & 0x000000FFu) | ((value >> 8u) & 0x0000FF00u) |
                    ((value << 8u) & 0x00FF0000u) | ((value << 24u) & 0xFF000000u);
        return value;
    }
    PSPRECOMP_MEMORY_FAST_PATH static void write_le16(std::uint8_t *destination, std::uint16_t value) noexcept {
        if constexpr (std::endian::native == std::endian::big)
            value = static_cast<std::uint16_t>((value >> 8u) | (value << 8u));
        std::memcpy(destination, &value, sizeof(value));
    }
    PSPRECOMP_MEMORY_FAST_PATH static void write_le32(std::uint8_t *destination, std::uint32_t value) noexcept {
        if constexpr (std::endian::native == std::endian::big)
            value = ((value >> 24u) & 0x000000FFu) | ((value >> 8u) & 0x0000FF00u) |
                    ((value << 8u) & 0x00FF0000u) | ((value << 24u) & 0xFF000000u);
        std::memcpy(destination, &value, sizeof(value));
    }

    [[nodiscard]] std::uint8_t aot_load8_slow(std::uint32_t address) const;
    [[nodiscard]] std::uint16_t aot_load16_slow(std::uint32_t address) const;
    [[nodiscard]] std::uint32_t aot_load32_slow(std::uint32_t address) const;
    void aot_store8_slow(std::uint32_t address, std::uint8_t value);
    void aot_store16_slow(std::uint32_t address, std::uint16_t value);
    void aot_store32_slow(std::uint32_t address, std::uint32_t value);

    std::vector<std::uint8_t> vram_;
    std::vector<std::uint8_t> bytes_;
    // Reached only through the out-of-line slow paths (resolve()).
    std::vector<std::uint8_t> scratchpad_;
    // Cached view of bytes_ for the inline fast paths.  Neither region is ever
    // resized after construction, so these stay valid for the object's life.
    //
    // Deliberately not __restrict.  It was tried on the theory that aliasing
    // against the AllegrexContext was forcing the guest register file to spill
    // on every memory instruction: measured on the 1500-vblank route it moved
    // nothing (0.599 us/dispatch against 0.595 without it), and it is not even
    // sound here -- this pointer aliases bytes_ below, which other members of
    // this class access directly.
    std::uint8_t *ram_data_{};
    std::uint32_t ram_limit8_{};
    std::uint32_t ram_limit16_{};
    std::uint32_t ram_limit32_{};
    bool write_watch_enabled_{};
};

} // namespace psprecomp

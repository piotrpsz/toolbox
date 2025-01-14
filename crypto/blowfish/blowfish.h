//
// Created by piotr on 09.01.25.
//

#pragma once
#include "../../types.h"
#include <utility>  // for std::pair
#include <memory>   // for std::shared_ptr
#include <vector>


namespace bee::crypto {
    class blowfish final {
        static constexpr size_t ROUND_COUNT = 16;
        static constexpr size_t BLOCK_SIZE = 8;
        static constexpr size_t KEY_MINSIZE = 4;
        static constexpr size_t KEY_MAXSIZE = 56;

        u32 p[ROUND_COUNT + 2]{};
        u32 s[4][256]{};
    public:
        blowfish(void const* key_material, size_t key_size);
        explicit blowfish(BytesView auto const key) : blowfish(key.data(), key.size()) {};
        ~blowfish();

        [[nodiscard]] static size_t key_maxsize() noexcept { return KEY_MAXSIZE; }
        [[nodiscard]] static size_t key_minsize() noexcept { return KEY_MINSIZE; }

        void encrypt_block(u32 const*, u32*) const noexcept;
        void decrypt_block(u32 const*, u32*) const noexcept;

        auto encrypt_ecb(BytesView auto const data) const noexcept
        -> std::vector<unsigned char> {
            return encrypt_ecb(data.data(), data.size());
        }
        auto encrypt_ecb(void const*, size_t) const noexcept
        -> std::vector<unsigned char>;

        auto decrypt_ecb(BytesView auto const data) const noexcept
        -> std::vector<u8> {
            return decrypt_ecb(data.data(), data.size());
        }
        auto decrypt_ecb(const void*, size_t) const noexcept
        -> std::vector<u8>;

        auto encrypt_cbc(void const*, size_t, void const* = nullptr) const noexcept
        -> std::vector<u8>;

        auto decrypt_cbc(void const*, size_t) const noexcept
        -> std::vector<u8>;

        static auto key_min_size() noexcept { return KEY_MINSIZE; }
        static auto key_max_size() noexcept { return KEY_MAXSIZE; }
        static auto block_size() noexcept { return BLOCK_SIZE; }

    private:
        [[nodiscard]] u32 f(u32 x) const noexcept {
            u32 const d = x & 0x00ff; x >>= 8;
            u32 const c = x & 0x00ff; x >>= 8;
            u32 const b = x & 0x00ff; x >>= 8;
            u32 const a = x & 0x00ff;
            return ((s[0][a] + s[1][b]) ^ s[2][c]) + s[3][d];
        }
    };
}


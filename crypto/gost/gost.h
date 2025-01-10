//
// Created by piotr on 10.01.25.
//

#pragma once
#include "../../types.h"
#include <cstddef>  // for size_t

namespace bee::crypto {
    class gost final {
        static constexpr size_t BLOCK_SIZE = 8;
        static constexpr size_t KEY_SIZE = 32;

        u32 k[8]{};
        u8  k87[256]{};
        u8  k65[256]{};
        u8  k43[256]{};
        u8  k21[256]{};
    public:
        gost(void const*, size_t);
        ~gost();

        void encrypt_block(u32 const*, u32*) const noexcept;
        void decrypt_block(u32 const*, u32*) const noexcept;

    private:
        [[nodiscard]] u32 f(const u32 x) const noexcept {
            const auto w0 = static_cast<u32>(k87[(x >> 24) & 0xff]) << 24;
            const auto w1 = static_cast<u32>(k65[(x >> 16) & 0xff]) << 16;
            const auto w2 = static_cast<u32>(k43[(x >> 8) & 0xff]) <<  8;
            const auto w3 = static_cast<u32>(k21[x & 0xff]);

            const u32 w = w0|w1|w2|w3;
            return (w << 11) | (w >> (32 - 11));
        }
    };
}
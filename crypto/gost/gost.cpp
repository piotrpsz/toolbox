//
// Created by piotr on 10.01.25.
//

#include "gost.h"
#include "../crypto.h"
#include <iostream>
#include <cstring>
#include <format>

namespace bee::crypto {

    gost::gost(void const* const key_material, size_t const key_size) {
        if (key_size != KEY_SIZE) {
            std::cerr << std::format("Key size must be equal to {}bytes\n", KEY_SIZE);
            // return;
        }

        static const u8 k8[16] = {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 };
        static const u8 k7[16] = {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10 };
        static const u8 k6[16] = {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 };
        static const u8 k5[16] = { 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15 };
        static const u8 k4[16] = { 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9 };
        static const u8 k3[16] = {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11 };
        static const u8 k2[16] = { 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1 };
        static const u8 k1[16] =  {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7 };

        std::memcpy(k, key_material, key_size);

        for (int i = 0; i < 256; i++) {
            const int p1 = i >> 4;
            const int p2 = i & 15;
            k87[i] = (k8[p1] << 4) | k7[p2];
            k65[i] = (k6[p1] << 4) | k5[p2];
            k43[i] = (k4[p1] << 4) | k3[p2];
            k21[i] = (k2[p1] << 4) | k1[p2];
        }
    }

    gost::~gost() {
        clear_bytes(k, 8 * sizeof(u32));
        clear_bytes(k87, 256);
        clear_bytes(k65, 256);
        clear_bytes(k43, 256);
        clear_bytes(k21, 256);
    }

    void gost::encrypt_block(u32 const* const src, u32* const dst) const noexcept {
        u32 n1 = src[0];
        u32 n2 = src[1];

        n2 ^= f(n1 + k[0]); n1 ^= f(n2 + k[1]);
        n2 ^= f(n1 + k[2]); n1 ^= f(n2 + k[3]);
        n2 ^= f(n1 + k[4]); n1 ^= f(n2 + k[5]);
        n2 ^= f(n1 + k[6]); n1 ^= f(n2 + k[7]);

        n2 ^= f(n1 + k[0]); n1 ^= f(n2 + k[1]);
        n2 ^= f(n1 + k[2]); n1 ^= f(n2 + k[3]);
        n2 ^= f(n1 + k[4]); n1 ^= f(n2 + k[5]);
        n2 ^= f(n1 + k[6]); n1 ^= f(n2 + k[7]);

        n2 ^= f(n1 + k[0]); n1 ^= f(n2 + k[1]);
        n2 ^= f(n1 + k[2]); n1 ^= f(n2 + k[3]);
        n2 ^= f(n1 + k[4]); n1 ^= f(n2 + k[5]);
        n2 ^= f(n1 + k[6]); n1 ^= f(n2 + k[7]);

        n2 ^= f(n1 + k[7]); n1 ^= f(n2 + k[6]);
        n2 ^= f(n1 + k[5]); n1 ^= f(n2 + k[4]);
        n2 ^= f(n1 + k[3]); n1 ^= f(n2 + k[2]);
        n2 ^= f(n1 + k[1]); n1 ^= f(n2 + k[0]);

        dst[0] = n2;
        dst[1] = n1;
    }

    void gost::decrypt_block(u32 const* const src, u32* const dst) const noexcept {
        u32 n1 = src[0];
        u32 n2 = src[1];

        n2 ^= f(n1 + k[0]); n1 ^= f(n2 + k[1]);
        n2 ^= f(n1 + k[2]); n1 ^= f(n2 + k[3]);
        n2 ^= f(n1 + k[4]); n1 ^= f(n2 + k[5]);
        n2 ^= f(n1 + k[6]); n1 ^= f(n2 + k[7]);

        n2 ^= f(n1 + k[7]); n1 ^= f(n2 + k[6]);
        n2 ^= f(n1 + k[5]); n1 ^= f(n2 + k[4]);
        n2 ^= f(n1 + k[3]); n1 ^= f(n2 + k[2]);
        n2 ^= f(n1 + k[1]); n1 ^= f(n2 + k[0]);

        n2 ^= f(n1 + k[7]); n1 ^= f(n2 + k[6]);
        n2 ^= f(n1 + k[5]); n1 ^= f(n2 + k[4]);
        n2 ^= f(n1 + k[3]); n1 ^= f(n2 + k[2]);
        n2 ^= f(n1 + k[1]); n1 ^= f(n2 + k[0]);

        n2 ^= f(n1 + k[7]); n1 ^= f(n2 + k[6]);
        n2 ^= f(n1 + k[5]); n1 ^= f(n2 + k[4]);
        n2 ^= f(n1 + k[3]); n1 ^= f(n2 + k[2]);
        n2 ^= f(n1 + k[1]); n1 ^= f(n2 + k[0]);

        dst[0] = n2;
        dst[1] = n1;
    }


}
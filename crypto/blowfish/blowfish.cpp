//
// Created by piotr on 09.01.25.
//

#include "blowfish.h"
#include "data.h"
#include "../crypto.h"
#include <vector>
#include <iostream>
#include <cstring>  // for std::memcpy

#include "../../toolbox.h"

namespace bee::crypto {

    blowfish::blowfish(void const* const key_material, size_t const key_size)
    {
        if (key_size < KEY_MINSIZE || key_size > KEY_MAXSIZE) {
            std::cerr << "Error (blowfish): invalid key size\n";
            return;
        }

        auto const key = static_cast<u8 const*>(key_material);

        // S - init
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 256; ++j) {
                s[i][j] = orgs[i][j];
            }
        }

        // P - init
        int k = 0;
        for (int i = 0; i < (ROUND_COUNT + 2); i++) {
            u32 d = 0;
            for (int j = 0; j < 4; j++) {
                d = (d << 8) | static_cast<u32>(key[k]);
                ++k;
                if (k >= key_size)
                    k = 0;
            }
            p[i] = orgp[i] ^ d;
        }


        // P
        u32 src[2]{0, 0};
        u32 dst[2]{0, 0};
        for (int i = 0; i < (ROUND_COUNT + 2); i += 2) {
            encrypt_block(src, dst);
            p[i] = src[0] = dst[0];
            p[i+1] = src[1] = dst[1];
        }

        // S
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 256; j += 2) {
                encrypt_block(src, dst);
                s[i][j] = src[0] = dst[0];
                s[i][j+1] = src[1] = dst[1];
            }
        }
    }

    blowfish::~blowfish() {
        clear_bytes(p, (ROUND_COUNT+2) * sizeof(u32));
        clear_bytes(s[0], 256 * sizeof(u32));
        clear_bytes(s[1], 256 * sizeof(u32));
        clear_bytes(s[2], 256 * sizeof(u32));
        clear_bytes(s[3], 256 * sizeof(u32));
    }


    /****************************************************************
    *                                                               *
    *                 e n c r y p t _ b l o c k                     *
    *                                                               *
    ****************************************************************/

    void blowfish::encrypt_block(u32 const* const src, u32* const dst) const noexcept {
        u32 xl = src[0];
        u32 xr = src[1];

        // 0
        xl = xl ^ p[0];
        xr = f(xl) ^ xr;
        xr = xr ^ p[1];
        xl = f(xr) ^ xl;
        // 1
        xl = xl ^ p[2];
        xr = f(xl) ^ xr;
        xr = xr ^ p[3];
        xl = f(xr) ^ xl;
        // 2
        xl = xl ^ p[4];
        xr = f(xl) ^ xr;
        xr = xr ^ p[5];
        xl = f(xr) ^ xl;
        // 3
        xl = xl ^ p[6];
        xr = f(xl) ^ xr;
        xr = xr ^ p[7];
        xl = f(xr) ^ xl;
        // 4
        xl = xl ^ p[8];
        xr = f(xl) ^ xr;
        xr = xr ^ p[9];
        xl = f(xr) ^ xl;
        // 5
        xl = xl ^ p[10];
        xr = f(xl) ^ xr;
        xr = xr ^ p[11];
        xl = f(xr) ^ xl;
        // 6
        xl = xl ^ p[12];
        xr = f(xl) ^ xr;
        xr = xr ^ p[13];
        xl = f(xr) ^ xl;
        // 7
        xl = xl ^ p[14];
        xr = f(xl) ^ xr;
        xr = xr ^ p[15];
        xl = f(xr) ^ xl;

        dst[0] = xr ^ p[17];
        dst[1] = xl ^ p[16];
    }

    /****************************************************************
    *                                                               *
    *                  d e c r y p t _ b l o c k                    *
    *                                                               *
    ****************************************************************/

    void blowfish::decrypt_block(u32 const* const src, u32* const dst) const noexcept {
        u32 xl = src[0];
        u32 xr = src[1];

        xl = xl ^ p[17];
        xr = f(xl) ^ xr;
        xr = xr ^ p[16];
        xl = f(xr) ^ xl;

        xl = xl ^ p[15];
        xr = f(xl) ^ xr;
        xr = xr ^ p[14];
        xl = f(xr) ^ xl;

        xl = xl ^ p[13];
        xr = f(xl) ^ xr;
        xr = xr ^ p[12];
        xl = f(xr) ^ xl;

        xl = xl ^ p[11];
        xr = f(xl) ^ xr;
        xr = xr ^ p[10];
        xl = f(xr) ^ xl;

        xl = xl ^ p[9];
        xr = f(xl) ^ xr;
        xr = xr ^ p[8];
        xl = f(xr) ^ xl;

        xl = xl ^ p[7];
        xr = f(xl) ^ xr;
        xr = xr ^ p[6];
        xl = f(xr) ^ xl;

        xl = xl ^ p[5];
        xr = f(xl) ^ xr;
        xr = xr ^ p[4];
        xl = f(xr) ^ xl;

        xl = xl ^ p[3];
        xr = f(xl) ^ xr;
        xr = xr ^ p[2];
        xl = f(xr) ^ xl;

        dst[0] = xr ^ p[0];
        dst[1] = xl ^ p[1];
    }

    /****************************************************************
    *                                                               *
    *                   e n c r y p t _ e c b                       *
    *                                                               *
    ****************************************************************/

    auto blowfish::encrypt_ecb(void const* data, size_t const nbytes) const noexcept
        ->  std::vector<unsigned char>
    {
        if (data == nullptr || nbytes == 0)
            return {};

        // Z wykorzystaniem przysłanego wskaźnika tworzymy wektor bajtów.
        // Jeśli rozmiar danych nie jest wielokrotnością 'bloku' to dodajemy 'padding',
        // aby dane do szyfrowania faktycznie miały rozmiar o wielokrotności 'bloku'.
        auto const ptr = static_cast<unsigned char const*>(data);

        // Bufor z jawnymi danymi.
        std::vector<unsigned char> plain{ptr, ptr + nbytes};
        auto size = plain.size();
        if (size % BLOCK_SIZE) {
            auto const blocks = (size / BLOCK_SIZE) + 1;
            plain.resize(blocks * BLOCK_SIZE, 0);
            plain[size] = 128;      // marker początku 'uzupełnienia'
            size = plain.size();
        }

        // Bufor na zaszyfrowane dane.
        std::vector<u8> cipher(size, 0);

        // Szyfrowanie.
        auto src = reinterpret_cast<u32 const*>(plain.data());
        auto dst = reinterpret_cast<u32*>(cipher.data());
        for (int i = 0; i < (size/BLOCK_SIZE); ++i) {
            encrypt_block(src, dst);
            src += 2;
            dst += 2;
        }

        return std::move(cipher);
    }

    /****************************************************************
    *                                                               *
    *                   d e c r y p t _ e c b                       *
    *                                                               *
    ****************************************************************/

    auto blowfish::decrypt_ecb(void const* const cipher, size_t const nbytes) const noexcept
        ->  std::vector<u8>
    {
        if (cipher == nullptr || nbytes == 0)
           return {};

        // Odszyfrowany tekst ma długość zaszyfrowanego tekstu.
        std::vector<u8> plain(nbytes, 0);

        // Odszyfrowanie
        auto src = static_cast<u32 const*>(cipher);
        auto dst = reinterpret_cast<u32*>(plain.data());
        for (int i = 0; i < (nbytes/BLOCK_SIZE); ++i) {
            decrypt_block(src, dst);
            src += 2;
            dst += 2;
        }

        // Obcięcie 'ogona'.
        if (int const idx = padding_index(plain.data(), static_cast<int>(nbytes)); idx != -1)
            plain.resize(idx);

        return std::move(plain);
    }

    /****************************************************************
    *                                                               *
    *                   e n c r y p t _ c b c                       *
    *                                                               *
    ****************************************************************/

    auto blowfish::encrypt_cbc(void const* const data, size_t const nbytes, void const* const iv) const noexcept
    -> std::vector<u8>
    {
        if (!data || nbytes == 0)
            return {};

        // Z wykorzystaniem przysłanego wskaźnika tworzymy wektor bajtów.
        // Jeśli rozmiar danych nie jest wielokrotnością 'bloku' to dodajemy 'padding',
        // aby dane do szyfrowania faktycznie miały rozmiar o wielokrotności 'bloku'.
        auto const ptr = static_cast<u8 const*>(data);

        // Bufor z jawnymi danymi.
        std::vector<u8> plain{ptr, ptr + nbytes};
        auto size = plain.size();
        if (size % BLOCK_SIZE) {
            auto const blocks = (size / BLOCK_SIZE) + 1;
            plain.resize(blocks * BLOCK_SIZE, 0);
            plain[size] = 128;      // marker początku 'uzupełnienia'
            size = plain.size();
        }

        // Bufor na zaszyfrowane dane (rozmiar zwiększony o IV).
        std::vector<u8> cipher(size + BLOCK_SIZE, 0);
        // Wektor IV jako pierwszy blok szyfrowanych danych.
        if (iv)
            std::memcpy(cipher.data(), iv, BLOCK_SIZE);
        else
            std::memcpy(cipher.data(), box::random_bytes<u8>(BLOCK_SIZE).data(), BLOCK_SIZE);

        // std::cout << std::format("I: {}\n", box::bytes_to_string(cipher, BLOCK_SIZE, true));


        // Szyfrowanie.
        auto src = reinterpret_cast<u32 const*>(plain.data());
        auto dst = reinterpret_cast<u32*>(cipher.data());
        u32 tmp[2];
        for (int i = 0; i < (size/BLOCK_SIZE); ++i) {
            tmp[0] = src[0] ^ dst[0];
            tmp[1] = src[1] ^ dst[1];
            dst += 2;
            encrypt_block(tmp, dst);
            src += 2;
        }
        return std::move(cipher);
    }

    /****************************************************************
    *                                                               *
    *                   d e c r y p t _ c b c                       *
    *                                                               *
    ****************************************************************/

    auto blowfish::decrypt_cbc(void const* const cipher, size_t nbytes) const noexcept
    -> std::vector<u8>
    {
        if (!cipher || nbytes == 0)
            return {};

        // Odszyfrowany tekst jest krótszy o blok IV.
        nbytes -= BLOCK_SIZE;
        std::vector<u8> plain(nbytes, 0);

        // Odszyfrowanie.
        auto src = static_cast<u32 const*>(cipher);
        auto dst = reinterpret_cast<u32*>(plain.data());
        for (int i = 0; i < (nbytes/BLOCK_SIZE); ++i) {
            decrypt_block(src + 2, dst);
            dst[0] = dst[0] ^ src[0];
            dst[1] = dst[1] ^ src[1];
            src += 2;
            dst += 2;
        }

        // Obcięcie 'ogona'.
        if (int const idx = padding_index(plain.data(), static_cast<int>(nbytes)); idx != -1)
            plain.resize(idx);

        return std::move(plain);
    }
}

//
// Created by piotr on 09.01.25.
//

#include "blowfish.h"
#include "data.h"
#include "../crypto.h"
#include <vector>
#include <iostream>

namespace bee::crypto {

    Blowfish::Blowfish(void const* const key_material, size_t const key_size)
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
                if (k >= key_size) {
                    k = 0;
                }
            }
            p[i] = orgp[i] ^ d;
        }


        // P
        u32 data[2] = {0, 0};
        for (int i = 0; i < (ROUND_COUNT + 2); i += 2) {
            encrypt_block(data, data);
            p[i] = data[0];
            p[i+1] = data[1];
        }

        // S
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 256; j += 2) {
                encrypt_block(data, data);
                s[i][j] = data[0];
                s[i][j+1] = data[1];
            }
        }
    }

    Blowfish::~Blowfish() {
        crypto::clear_bytes(p, (ROUND_COUNT+2) * sizeof(u32));
        crypto::clear_bytes(s[0], 256 * sizeof(u32));
        crypto::clear_bytes(s[1], 256 * sizeof(u32));
        crypto::clear_bytes(s[2], 256 * sizeof(u32));
        crypto::clear_bytes(s[3], 256 * sizeof(u32));
    }


    /****************************************************************
    *                                                               *
    *                 e n c r y p t _ b l o c k                     *
    *                                                               *
    ****************************************************************/

    void Blowfish::encrypt_block(u32 const* const src, u32* const dst) const noexcept {
        u32 xl = src[0];
        u32 xr = src[1];

        xl = xl ^ p[0];
        xr = f(xl) ^ xr;
        xr = xr ^ p[1];
        xl = f(xr) ^ xl;

        xl = xl ^ p[2];
        xr = f(xl) ^ xr;
        xr = xr ^ p[3];
        xl = f(xr) ^ xl;

        xl = xl ^ p[4];
        xr = f(xl) ^ xr;
        xr = xr ^ p[5];
        xl = f(xr) ^ xl;

        xl = xl ^ p[6];
        xr = f(xl) ^ xr;
        xr = xr ^ p[7];
        xl = f(xr) ^ xl;

        xl = xl ^ p[8];
        xr = f(xl) ^ xr;
        xr = xr ^ p[9];
        xl = f(xr) ^ xl;

        xl = xl ^ p[10];
        xr = f(xl) ^ xr;
        xr = xr ^ p[11];
        xl = f(xr) ^ xl;

        xl = xl ^ p[12];
        xr = f(xl) ^ xr;
        xr = xr ^ p[13];
        xl = f(xr) ^ xl;

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

    void Blowfish::decrypt_block(u32 const* const src, u32* const dst) const noexcept {
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

    auto Blowfish::encrypt_ecb(void const* data, size_t const nbytes) const noexcept
        ->  std::vector<u8>
    {
        if (data == nullptr || nbytes == 0)
            return {};

        // Z wykorzystaniem przysłanego wskaźnika tworzymy wektor bajtów.
        // Jeśli rozmiar danych nie jest wielokrotnością 'bloku' to dodajemy 'padding',
        // aby dane do szyfrowania faktycznie miały rozmiar o wielokrotności 'bloku'.
        auto const ptr = static_cast<u8 const*>(data);

        // Bufor z jawnymi danymi.
        std::vector<u8> plain{ptr, ptr + nbytes};
        auto size = plain.size();
        if (auto n = size % BLOCK_SIZE) {
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
        return cipher;
    }

    /****************************************************************
    *                                                               *
    *                   d e c r y p t _ e c b                       *
    *                                                               *
    ****************************************************************/

    auto Blowfish::decrypt_ecb(void const* const cipher, size_t const nbytes) const noexcept
        ->  std::vector<u8>
    {
        if (cipher == nullptr || nbytes == 0)
           return {};

        std::vector<u8> plain(nbytes, 0);

        auto src = static_cast<u32 const*>(cipher);
        auto dst = reinterpret_cast<u32*>(plain.data());

        for (int i = 0; i < (nbytes/BLOCK_SIZE); ++i) {
            decrypt_block(src, dst);
            src += 2;
            dst += 2;
        }

        if (int const idx = padding_index(plain.data(), static_cast<int>(nbytes)); idx != -1)
            plain.resize(idx);
        return plain;
    }

}

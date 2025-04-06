//
// Created by piotr on 09.01.25.
//

#include "crypto.h"
#include "../toolbox.h"
#include <cstring>

namespace bee::crypto {
    int padding_index(u8 const* const data, int const nbytes) noexcept {
        for (int i = nbytes - 1; i >= 0; --i) {
            if (data[i] != 0) {
                if (data[i] == 128) {
                    return i;
                }
                break;
            }
        }
        return -1;
    }

    void clear_bytes(void* const data, size_t const nbytes) noexcept {
        // czterokrotnie wypełniamy bufor liczbami losowymi
        for (auto i = 0; i < 4; ++i) {
            const auto rnd = box::random_bytes<u8>(nbytes);
            std::memcpy(data, rnd.data(), nbytes);
        }
        std::memset(data, 0x55, nbytes);
        std::memset(data, 0xaa, nbytes);
        std::memset(data, 0xff, nbytes);
        std::memset(data, 0x00, nbytes);
    }
}

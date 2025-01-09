//
// Created by piotr on 09.01.25.
//

#pragma once
#include "../types.h"
#include "blowfish/blowfish.h"

namespace bee::crypto {
    int padding_index(u8 const*, int) noexcept;
    void clear_bytes(void*, size_t) noexcept;
}

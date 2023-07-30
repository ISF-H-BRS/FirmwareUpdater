// ============================================================================================== //
//                                                                                                //
//   This file is part of the ISF Utilities collection.                                           //
//                                                                                                //
//   Author:                                                                                      //
//   Marcel Hasler <mahasler@gmail.com>                                                           //
//                                                                                                //
//   Copyright (c) 2022 - 2023                                                                    //
//   Bonn-Rhein-Sieg University of Applied Sciences                                               //
//                                                                                                //
//   Redistribution and use in source and binary forms, with or without modification,             //
//   are permitted provided that the following conditions are met:                                //
//                                                                                                //
//   1. Redistributions of source code must retain the above copyright notice,                    //
//      this list of conditions and the following disclaimer.                                     //
//                                                                                                //
//   2. Redistributions in binary form must reproduce the above copyright notice,                 //
//      this list of conditions and the following disclaimer in the documentation                 //
//      and/or other materials provided with the distribution.                                    //
//                                                                                                //
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"                  //
//   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED            //
//   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.           //
//   IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,             //
//   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT           //
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR           //
//   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,            //
//   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)           //
//   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE                   //
//   POSSIBILITY OF SUCH DAMAGE.                                                                  //
//                                                                                                //
// ============================================================================================== //

#include "base64.h"

// ---------------------------------------------------------------------------------------------- //

static const char _base64_map[64] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

// ---------------------------------------------------------------------------------------------- //

static inline
uint8_t _base64_find(char c)
{
    for (uint8_t i = 0; i < 64; ++i)
    {
        if (_base64_map[i] == c)
            return i;
    }

    return 255;
}

// ---------------------------------------------------------------------------------------------- //

int base64_encode(const uint8_t *input, size_t size, char *result)
{
    const size_t padded_size = ((size + 2) / 3) * 3;

    size_t in_position = 0;
    size_t out_position = 0;

    while (in_position < padded_size)
    {
        uint8_t bytes[3] = { 0, 0, 0 };

        for (size_t i = 0; i < sizeof(bytes); ++i)
        {
            if (in_position < size)
                bytes[i] = input[in_position];

            ++in_position;
        }

        uint32_t word = (bytes[0] << 16) | (bytes[1] << 8) | bytes[2];

        result[out_position++] = _base64_map[(word >> 18) & 0x3f];
        result[out_position++] = _base64_map[(word >> 12) & 0x3f];
        result[out_position++] = _base64_map[(word >>  6) & 0x3f];
        result[out_position++] = _base64_map[(word >>  0) & 0x3f];
    }

    for (size_t i = 0; i < padded_size - size; ++i)
        result[out_position - 1 - i] = '=';

    result[out_position] = '\0';

    return 0;
}

// ---------------------------------------------------------------------------------------------- //

int base64_decode(const char *input, uint8_t *result, size_t *size)
{
    size_t position = 0;
    size_t padding = 0;

    while (*input != '\0')
    {
        uint32_t word = 0;

        for (int i = 3; i >= 0; --i)
        {
            const char c = *(input++);

            if (c == '\0')
                return -1;

            if (c == '=')
            {
                ++padding;
                continue;
            }

            const uint8_t index = _base64_find(c);

            if (index >= sizeof(_base64_map))
                return -1;

            word |= (index << (i * 6));
        }

        result[position++] = (uint8_t)(word >> 16);
        result[position++] = (uint8_t)(word >>  8);
        result[position++] = (uint8_t)(word >>  0);
    }

    if (size)
        *size = position - padding;

    return 0;
}

// ---------------------------------------------------------------------------------------------- //

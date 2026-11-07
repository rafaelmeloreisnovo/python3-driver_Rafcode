/* SPDX-License-Identifier: LicenseRef-RAFCODEPHI-Authorial-1.0 */
/* Copyright (c) 2026 Rafael Melo Reis. */
#include "rafcode_cursor.h"

/* Exit code is the first failing case; zero means scoped tests passed. */
int main(void) {
    const unsigned char four[] = { 0x12, 0x34, 0x56, 0x78 };
    raf_reader r;
    raf_reader_init(&r, four, 4);
    raf_u32 value = 0;
    if (raf_read_u32be(&r, &value) != RAF_OK || value != 0x12345678u || r.offset != 4)
        return 1;
    value = 0xABCDu;
    if (raf_read_u32be(&r, &value) != RAF_ERR_BOUNDS || value != 0xABCDu || r.offset != 4)
        return 2;

    unsigned char dest[] = { 7, 7, 7, 7 };
    raf_writer w;
    raf_writer_init(&w, dest, 4);
    if (raf_write_u32be(&w, 0xA0B1C2D3u) != RAF_OK || w.offset != 4
        || dest[0] != 0xA0 || dest[1] != 0xB1 || dest[2] != 0xC2 || dest[3] != 0xD3)
        return 3;
    if (raf_write_u32be(&w, 1) != RAF_ERR_BOUNDS || w.offset != 4)
        return 4;

    const unsigned char frame[] = { 0, 0, 0, 3, 0xAA, 0xBB, 0xCC };
    raf_slice slice = {four, 111};
    raf_reader_init(&r, frame, 7);
    if (raf_take_frame_u32be(&r, 2, &slice) != RAF_ERR_LIMIT
        || r.offset != 0 || slice.data != four || slice.length != 111)
        return 5;
    if (raf_take_frame_u32be(&r, 3, &slice) != RAF_OK
        || r.offset != 7 || slice.length != 3 || slice.data[0] != 0xAA
        || slice.data[1] != 0xBB || slice.data[2] != 0xCC)
        return 6;

    raf_reader_init(&r, frame, 6);
    slice.data = four;
    slice.length = 111;
    if (raf_take_frame_u32be(&r, 3, &slice) != RAF_ERR_BOUNDS
        || r.offset != 0 || slice.data != four || slice.length != 111)
        return 7;
    raf_reader_init(&r, frame, 3);
    if (raf_take_frame_u32be(&r, 3, &slice) != RAF_ERR_BOUNDS || r.offset != 0)
        return 8;

    raf_reader_init(&r, (const void *)0, 0);
    if (raf_take(&r, 0, &slice) != RAF_OK || slice.data != (void *)0
        || slice.length != 0 || r.offset != 0)
        return 9;
    raf_reader_init(&r, (const void *)0, 1);
    if (raf_take(&r, 0, &slice) != RAF_ERR_ARGUMENT || r.offset != 0)
        return 10;

    raf_reader_init(&r, four, 4);
    r.offset = 5;
    if (raf_read_u32be(&r, &value) != RAF_ERR_BOUNDS || r.offset != 5)
        return 11;

    raf_reader_init(&r, four, 4);
    if (raf_read_u32be(&r, (void *)0) != RAF_ERR_ARGUMENT || r.offset != 0)
        return 12;
    if (raf_take(&r, 5, &slice) != RAF_ERR_BOUNDS || r.offset != 0)
        return 13;
    if (raf_take(&r, 4, &slice) != RAF_OK || r.offset != 4 || slice.length != 4)
        return 14;

    unsigned char small[] = { 9, 9, 9 };
    raf_writer_init(&w, small, 3);
    if (raf_write_u32be(&w, 0xFFFFFFFFu) != RAF_ERR_BOUNDS
        || w.offset != 0 || small[0] != 9 || small[1] != 9 || small[2] != 9)
        return 15;
    raf_writer_init(&w, (void *)0, 4);
    if (raf_write_u32be(&w, 0) != RAF_ERR_ARGUMENT || w.offset != 0)
        return 16;
    return 0;
}

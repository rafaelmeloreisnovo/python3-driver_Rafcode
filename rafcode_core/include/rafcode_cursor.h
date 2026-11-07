/* SPDX-License-Identifier: LicenseRef-RAFCODEPHI-Authorial-1.0 */
/* Copyright (c) 2026 Rafael Melo Reis. See LICENSES/RAFCODEPHI-AUTHORIAL-1.0.txt. */
#ifndef RAFCODE_CURSOR_H
#define RAFCODE_CURSOR_H

/* Compiler-provided integer types: no hosted headers or runtime required. */
#if !defined(__UINT32_TYPE__) || !defined(__SIZE_TYPE__) || !defined(__CHAR_BIT__)
#error "RAFCODE core requires GCC/Clang-compatible type macros"
#endif
typedef __UINT32_TYPE__ raf_u32;
typedef __SIZE_TYPE__ raf_size;
_Static_assert(__CHAR_BIT__ == 8, "RAFCODE requires 8-bit bytes");
_Static_assert(sizeof(raf_u32) == 4, "RAFCODE requires exact 32-bit integers");
_Static_assert(sizeof(raf_size) >= sizeof(raf_u32), "RAFCODE requires size >= 32 bits");

typedef enum {
    RAF_OK = 0,
    RAF_ERR_ARGUMENT = 1,
    RAF_ERR_BOUNDS = 2,
    RAF_ERR_LIMIT = 3
} raf_result;

typedef struct {
    const unsigned char *data;
    raf_size length;
    raf_size offset;
} raf_reader;

typedef struct {
    unsigned char *data;
    raf_size length;
    raf_size offset;
} raf_writer;

typedef struct {
    const unsigned char *data;
    raf_size length;
} raf_slice;

/* No allocation, I/O, syscalls, libc, global mutable state or hidden buffers. */
void raf_reader_init(raf_reader *r, const void *data, raf_size length);
void raf_writer_init(raf_writer *w, void *data, raf_size length);

/* All failure paths leave cursor offsets and output arguments unchanged. */
raf_result raf_read_u32be(raf_reader *r, raf_u32 *out);
raf_result raf_write_u32be(raf_writer *w, raf_u32 value);
raf_result raf_take(raf_reader *r, raf_size count, raf_slice *out);

/* Generic length-prefixed record, not a claim of Firebird wire compatibility. */
raf_result raf_take_frame_u32be(raf_reader *r, raf_u32 max_payload, raf_slice *out);

#endif

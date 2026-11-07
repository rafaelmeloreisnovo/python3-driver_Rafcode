/* SPDX-License-Identifier: LicenseRef-RAFCODEPHI-Authorial-1.0 */
/* Copyright (c) 2026 Rafael Melo Reis. */
#include "rafcode_cursor.h"

static raf_u32 load_be32(const unsigned char *p) {
    return ((raf_u32)p[0] << 24) | ((raf_u32)p[1] << 16)
         | ((raf_u32)p[2] << 8) | (raf_u32)p[3];
}

void raf_reader_init(raf_reader *r, const void *data, raf_size length) {
    if (r != (void *)0) {
        r->data = (const unsigned char *)data;
        r->length = length;
        r->offset = 0;
    }
}

void raf_writer_init(raf_writer *w, void *data, raf_size length) {
    if (w != (void *)0) {
        w->data = (unsigned char *)data;
        w->length = length;
        w->offset = 0;
    }
}

static raf_result valid_reader(const raf_reader *r) {
    if (r == (void *)0 || (r->data == (void *)0 && r->length != 0))
        return RAF_ERR_ARGUMENT;
    if (r->offset > r->length)
        return RAF_ERR_BOUNDS;
    return RAF_OK;
}

static raf_result valid_writer(const raf_writer *w) {
    if (w == (void *)0 || (w->data == (void *)0 && w->length != 0))
        return RAF_ERR_ARGUMENT;
    if (w->offset > w->length)
        return RAF_ERR_BOUNDS;
    return RAF_OK;
}

raf_result raf_read_u32be(raf_reader *r, raf_u32 *out) {
    raf_result status = valid_reader(r);
    if (status != RAF_OK || out == (void *)0)
        return status != RAF_OK ? status : RAF_ERR_ARGUMENT;
    if (r->length - r->offset < 4)
        return RAF_ERR_BOUNDS;
    raf_u32 value = load_be32(r->data + r->offset);
    *out = value;
    r->offset += 4;
    return RAF_OK;
}

raf_result raf_write_u32be(raf_writer *w, raf_u32 value) {
    raf_result status = valid_writer(w);
    if (status != RAF_OK)
        return status;
    if (w->length - w->offset < 4)
        return RAF_ERR_BOUNDS;
    unsigned char *p = w->data + w->offset;
    p[0] = (unsigned char)(value >> 24);
    p[1] = (unsigned char)(value >> 16);
    p[2] = (unsigned char)(value >> 8);
    p[3] = (unsigned char)value;
    w->offset += 4;
    return RAF_OK;
}

raf_result raf_take(raf_reader *r, raf_size count, raf_slice *out) {
    raf_result status = valid_reader(r);
    if (status != RAF_OK || out == (void *)0)
        return status != RAF_OK ? status : RAF_ERR_ARGUMENT;
    if (count > r->length - r->offset)
        return RAF_ERR_BOUNDS;
    const unsigned char *start = r->data != (void *)0
                               ? r->data + r->offset : (const unsigned char *)0;
    out->data = start;
    out->length = count;
    r->offset += count;
    return RAF_OK;
}

raf_result raf_take_frame_u32be(raf_reader *r, raf_u32 max_payload, raf_slice *out) {
    raf_result status = valid_reader(r);
    if (status != RAF_OK || out == (void *)0)
        return status != RAF_OK ? status : RAF_ERR_ARGUMENT;
    raf_size remaining = r->length - r->offset;
    if (remaining < 4)
        return RAF_ERR_BOUNDS;
    raf_u32 payload_length = load_be32(r->data + r->offset);
    if (payload_length > max_payload)
        return RAF_ERR_LIMIT;
    if ((raf_size)payload_length > remaining - 4)
        return RAF_ERR_BOUNDS;
    out->data = r->data + r->offset + 4;
    out->length = (raf_size)payload_length;
    r->offset += 4 + (raf_size)payload_length;
    return RAF_OK;
}

/*
 * PLEASE LICENSE 11/2020, Michael Clark <michaeljclark@mac.com>
 *
 * All rights to this work are granted for all purposes, with exception of
 * author's implied right of copyright to defend the free use of this work.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

#include <cstring>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <alloca.h>
#include <poll.h>
#include <termios.h>
#include <unistd.h>

/*
 * base64.c : base-64 / MIME encode/decode
 * PUBLIC DOMAIN - Jon Mayo - November 13, 2003
 * $Id: base64.c 156 2007-07-12 23:29:10Z orange $
 */

namespace kitty {

const uint8_t base64enc_tab[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64_encode(size_t in_len, const uint8_t *in, size_t out_len, char *out);

size_t kitty_send_rgba(char cmd, uint32_t id, uint32_t compression,
                       const uint8_t *color_pixels, uint32_t width,
                       uint32_t height);

struct line {
  size_t r;
  char buf[256];
};
struct kdata {
  int iid;
  size_t offset;
  struct line data;
};
struct pos {
  int x, y;
};

typedef struct line line;
typedef struct kdata kdata;
typedef struct pos pos;

line kitty_recv_term(int timeout);

line kitty_send_term(const char *s);

void kitty_set_position(int x, int y);

pos kitty_get_position();

void kitty_hide_cursor();

void kitty_show_cursor();

kdata kitty_parse_response(line l);

void kitty_flip_buffer_y(uint32_t *buffer, uint32_t width, uint32_t height);

typedef void (*key_cb)(int k);

key_cb *_get_key_callback();

void kitty_key_callback(key_cb cb);

void kitty_poll_events(int millis);

struct termios *_get_termios_backup();

struct termios *_get_termios_raw();

void kitty_setup_termios();

void kitty_restore_termios();

} // namespace kitty

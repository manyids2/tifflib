#include "kitty_util.h"

size_t kitty_send_rgba(char cmd, uint32_t id, uint32_t compression,
                       const uint8_t *color_pixels, uint32_t width,
                       uint32_t height) {
  const size_t chunk_limit = 4096;

  size_t pixel_count = width * height;
  size_t total_size = pixel_count << 2;
  const uint8_t *encode_data;
  size_t encode_size;

#define COMPRESSION_STRING ""

  encode_data = color_pixels;
  encode_size = total_size;

  size_t base64_size = ((encode_size + 2) / 3) * 4;
  uint8_t *base64_pixels = (uint8_t *)alloca(base64_size + 1);

  /* base64 encode the data */
  int ret = base64_encode(encode_size, encode_data, base64_size + 1,
                          (char *)base64_pixels);
  if (ret < 0) {
    fprintf(stderr, "error: base64_encode failed: ret=%d\n", ret);
    exit(1);
  }

  /*
   * write kitty protocol RGBA image in chunks no greater than 4096 bytes
   *
   * <ESC>_Gf=32,s=<w>,v=<h>,m=1;<encoded pixel data first chunk><ESC>\
   * <ESC>_Gm=1;<encoded pixel data second chunk><ESC>\
   * <ESC>_Gm=0;<encoded pixel data last chunk><ESC>\
   */

  size_t sent_bytes = 0;
  while (sent_bytes < base64_size) {
    size_t chunk_size = base64_size - sent_bytes < chunk_limit
                            ? base64_size - sent_bytes
                            : chunk_limit;
    int cont = !!(sent_bytes + chunk_size < base64_size);
    if (sent_bytes == 0) {
      fprintf(stdout, "\x1B_Gf=32,a=%c,i=%u,s=%d,v=%d,m=%d%s;", cmd, id, width,
              height, cont, COMPRESSION_STRING);
    } else {
      fprintf(stdout, "\x1B_Gm=%d;", cont);
    }
    fwrite(base64_pixels + sent_bytes, chunk_size, 1, stdout);
    fprintf(stdout, "\x1B\\");
    sent_bytes += chunk_size;
  }
  fflush(stdout);

  return encode_size;
}
int base64_encode(size_t in_len, const uint8_t *in, size_t out_len, char *out) {
  uint ii, io;
  uint_least32_t v;
  uint rem;

  for (io = 0, ii = 0, v = 0, rem = 0; ii < in_len; ii++) {
    uint8_t ch;
    ch = in[ii];
    v = (v << 8) | ch;
    rem += 8;
    while (rem >= 6) {
      rem -= 6;
      if (io >= out_len)
        return -1; /* truncation is failure */
      out[io++] = base64enc_tab[(v >> rem) & 63];
    }
  }
  if (rem) {
    v <<= (6 - rem);
    if (io >= out_len)
      return -1; /* truncation is failure */
    out[io++] = base64enc_tab[v & 63];
  }
  while (io & 3) {
    if (io >= out_len)
      return -1; /* truncation is failure */
    out[io++] = '=';
  }
  if (io >= out_len)
    return -1; /* no room for null terminator */
  out[io] = 0;
  return io;
}

line kitty_recv_term(int timeout) {
  line l = {0, {0}};
  int r;
  struct pollfd fds[1];

  memset(fds, 0, sizeof(fds));
  fds[0].fd = fileno(stdin);
  fds[0].events = POLLIN;

  if ((r = poll(fds, 1, timeout)) < 0) {
    return l;
  }
  if ((fds[0].revents & POLLIN) == 0) {
    return l;
  }

  l.r = read(0, l.buf, sizeof(l.buf) - 1);
  if (l.r > 0) {
    l.buf[l.r] = '\0';
  }

  return l;
}

line kitty_send_term(const char *s) {
  fputs(s, stdout);
  fflush(stdout);
  return kitty_recv_term(-1);
}

void kitty_set_position(int x, int y) {
  printf("\x1B[%d;%dH", y, x);
  fflush(stdout);
}

pos kitty_get_position() {
  pos p;
  line l = kitty_send_term("\x1B[6n");
  // FIXME: changed a thing here
  /* int r = sscanf(l.buf + 1, "[%d;%dR", &p.y, &p.x); */
  sscanf(l.buf + 1, "[%d;%dR", &p.y, &p.x);
  return p;
}

void kitty_hide_cursor() { puts("\x1B[?25l"); }

void kitty_show_cursor() { puts("\x1B[?25h"); }

kdata kitty_parse_response(line l) {
  /*
   * parse kitty response of the form: "\x1B_Gi=<image_id>;OK\x1B\\"
   *
   * NOTE: a keypress can be present before or after the data
   */
  if (l.r < 1) {
    return (kdata){-1, 0, l};
  }
  char *esc = strchr(l.buf, '\x1B');
  if (!esc) {
    return (kdata){-1, 0, l};
  }
  ptrdiff_t offset = (esc - l.buf) + 1;
  int iid = 0;
  int r = sscanf(l.buf + offset, "_Gi=%d;OK", &iid);
  if (r != 1) {
    return (kdata){-1, 0, l};
  }
  // FIXME: changed a thing here
  /* offset -> static_cast... ; */
  return (kdata){iid, static_cast<size_t>(offset), l};
}

void kitty_flip_buffer_y(uint32_t *buffer, uint32_t width, uint32_t height) {
  /* Iterate only half the buffer to get a full flip */
  size_t rows = height >> 1;
  size_t line_size = width * sizeof(uint32_t);
  uint32_t *scan_line = (uint32_t *)alloca(line_size);

  for (uint32_t rowIndex = 0; rowIndex < rows; rowIndex++) {
    size_t l1 = rowIndex * width;
    size_t l2 = (height - rowIndex - 1) * width;
    memcpy(scan_line, buffer + l1, line_size);
    memcpy(buffer + l1, buffer + l2, line_size);
    memcpy(buffer + l2, scan_line, line_size);
  }
}

void kitty_key_callback(key_cb cb) { *_get_key_callback() = cb; }

void kitty_poll_events(int millis) {
  kdata k;

  /*
   * loop until we see the image id from kitty acknowledging
   * the image upload. keypresses can arrive on their own,
   * or before or after the image id response from kitty.
   */
  do {
    k = kitty_parse_response(kitty_recv_term(millis));
    /* handle keypress present at the beginning of the response */
    if (k.offset == 2) {
      (*_get_key_callback())(k.data.buf[0]);
    }
    /* handle keypress present at the end of the response */
    if (k.offset == 1) {
      char *ok = strstr(k.data.buf + k.offset, ";OK\x1B\\");
      ptrdiff_t ko = (ok != NULL) ? ((ok - k.data.buf) + 5) : 0;
      // FIXME: changed a thing here
      /* ko -> static_cast... ; */
      if (k.data.r > static_cast<size_t>(ko)) {
        (*_get_key_callback())(k.data.buf[ko]);
      }
    }
    /* handle keypress on its own */
    if (k.offset == 0 && k.data.r == 1) {
      (*_get_key_callback())(k.data.buf[0]);
    }
    /* loop once more for a keypress, if we got our image id */
  } while (k.iid > 0);
}

key_cb *_get_key_callback() {
  static key_cb cb;
  return &cb;
}

struct termios *_get_termios_backup() {
  static struct termios backup;
  return &backup;
}

struct termios *_get_termios_raw() {
  static struct termios raw;
  cfmakeraw(&raw);
  return &raw;
}

void kitty_setup_termios() {
  /* save termios and set to raw */
  tcgetattr(0, _get_termios_backup());
  tcsetattr(0, TCSANOW, _get_termios_raw());
}

void kitty_restore_termios() {
  /* restore termios */
  tcsetattr(0, TCSANOW, _get_termios_backup());
}

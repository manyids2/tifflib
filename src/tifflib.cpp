// To display version number
#include "window.h"
#include <cxxopts.hpp>
#include <iterator>
#include <numeric>

static uint width = 256, height = 256;
static uint millis = 10;
static uint compression = 0;
static size_t bytes_rendered = 0;
static size_t bytes_transferred = 0;
int running = 1;

static void keystroke(int key) {
  switch (key) {
  case 'q':
    running = 0;
    break;
  default:
    return;
  }
}

static void reshape(int width, int height) {
  glViewport(0, 0, (GLint)width, (GLint)height);
}

static void draw_on_buffer(uint8_t *buffer, int width, int height, int a) {
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      buffer[((x * a) * height) + (y * a)] = 255;
    }
  }
}

int main(int argc, char **argv) {
  cxxopts::Options options("tifflib", "A brief description");

  // clang-format off
  options.add_options()
    ( "f,slidepath", "Path to slide",
      cxxopts::value<std::string>()->default_value("/data/slides/aperio/CMU-1.svs"))
    ( "m,maskpath", "Path to mask",
      cxxopts::value<std::string>()->default_value("/data/slides/dlup/slide1.itiff"))
    ("h,help", "Print usage");
  // clang-format on

  auto result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  std::string slidepath;
  slidepath = result["slidepath"].as<std::string>();
  std::cout << slidepath << std::endl;

  /* SlideProps slide = get_slideprops(slidepath); */
  /* print_slideprops(slide); */

  std::string maskpath;
  maskpath = result["maskpath"].as<std::string>();
  std::cout << maskpath << std::endl;

  OSMesaContext ctx;
  uint8_t *buffer;
  uint lh = height / 18;
  uint frame;
  size_t len;
  pos p;

  /* Create an RGBA-mode context */
  if (!(ctx = OSMesaCreateContextExt(OSMESA_RGBA, 16, 0, 0, NULL))) {
    fprintf(stderr, "OSMesaCreateContext failed!\n");
    return 0;
  }

  /* Allocate the image buffer */
  if (!(buffer = (uint8_t *)malloc(width * height * sizeof(uint)))) {
    fprintf(stderr, "Alloc image buffer failed!\n");
    return 0;
  }

  /* Bind the buffer to the context and make it current */
  if (!OSMesaMakeCurrent(ctx, buffer, GL_UNSIGNED_BYTE, width, height)) {
    fprintf(stderr, "OSMesaMakeCurrent failed!\n");
    return 0;
  }

  kitty_key_callback(keystroke);
  draw_on_buffer(buffer, width, height, 0);
  draw_on_buffer(buffer, width, height, 3);

  for (uint i = 0; i < lh; i++)
    printf("\n");

  kitty_setup_termios();
  p = kitty_get_position();
  kitty_hide_cursor();

  /* loop displaying frames */
  static uint count = 1000;
  running = 1;
  for (frame = 0; frame < count && running; frame++) {
    /* glClearColor(0.0, 0.0, 0.0, 0.0); */
    /* glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); */
    /* glFlush(); */

    /* flip buffer and output to kitty as base64 RGBA data*/
    uint iid = 2 + (frame & 1);
    kitty_set_position(p.x, p.y - lh);
    /* kitty_flip_buffer_y((uint *)buffer, width, height); */
    len = kitty_send_rgba('T', iid, compression, buffer, width, height);

    bytes_rendered += (width * height) << 2;
    bytes_transferred += len;

    kitty_poll_events(millis);
  }

  /* drain kitty responses */
  kitty_poll_events(millis);

  /* restore cursor position then show cursor */
  kitty_show_cursor();
  kitty_set_position(p.x, p.y);
  kitty_restore_termios();
  printf("\n");

  /* release memory and exit */
  OSMesaDestroyContext(ctx);
  free(buffer);
  exit(EXIT_SUCCESS);

  return 0;
}

// To display version number
#include "window.h"
#include <cxxopts.hpp>

int running = 1;
static size_t bytes_rendered = 0;
static size_t bytes_transferred = 0;

static void keystroke(int key) {
  switch (key) {
  case 'q':
    running = 0;
    break;
  default:
    return;
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

  SlideProps slide = get_slideprops(slidepath);
  print_slideprops(slide);

  std::string maskpath;
  maskpath = result["maskpath"].as<std::string>();
  std::cout << maskpath << std::endl;

  Window win(180, 180);

  kitty_key_callback(keystroke);

  for (uint i = 0; i < win.lh; i++)
    printf("\n");

  kitty_setup_termios();
  win.p = kitty_get_position();
  kitty_hide_cursor();

  uint count = 1000;
  uint frame;
  for (frame = 0; frame < count && running; frame++) {
    /* win.clear(); */
    /* glClearColor(1.f, 0.f, 0.f, 1.f); */
    /* glClear(GL_COLOR_BUFFER_BIT); */
    /* glFlush(); */

    /* flip buffer and output to kitty as base64 RGBA data*/
    /* uint iid = 2 + (frame & 1); */
    /* kitty_set_position(win.p.x, win.p.y - win.lh); */
    /* kitty_flip_buffer_y((uint *)win.buffer, win.width, win.height); */
    /* win.len = kitty_send_rgba('T', iid, win.compression, win.buffer,
     * win.width,
     */
    /*                           win.height); */
    /**/
    /* bytes_rendered += (win.width * win.height) << 2; */
    /* bytes_transferred += win.len; */
    /**/
    kitty_poll_events(win.millis);
  }

  /* drain kitty responses */
  kitty_poll_events(win.millis);

  /* restore cursor position then show cursor */
  kitty_show_cursor();
  kitty_set_position(win.p.x, win.p.y);
  kitty_restore_termios();
  printf("\n");

  return 0;
}

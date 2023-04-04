// To display version number
#include "window.h"
#include <cxxopts.hpp>
#include <iostream>

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

  Window win(1000, 1000);
  win.setSlideprops(slide);
  win.setShowing(true);

  // MAIN LOOP
  while (win.getShowing()) {
    win.clear();
    win.update();
    win.render();
  }

  return 0;
}

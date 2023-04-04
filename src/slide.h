#include <iostream>
#include <openslide/openslide.h>
#include <vector>

using namespace std;

// Get the props we need into a struct
// Doesn't get called often, so size, caching is not an issue, hopefully
struct SlideProps {
  // Refs
  std::string slidepath;
  std::string vendor;
  std::string comment;

  // Props
  float mpp;
  int height;
  int width;
  int level_count;
  std::vector<float> level_downsamples;
  std::vector<int> level_heights;
  std::vector<int> level_widths;

  // TextureID for thumbnail
  unsigned int thumbnail;

  openslide_t *wsi;
};

SlideProps get_slideprops(std::string slidepath);
void print_slideprops(SlideProps slideprops);
void initThumnailTexture(SlideProps slideprops);

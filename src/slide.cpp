#include "slide.h"
#include <cstdio>
#include <cstring>
#include <openslide/openslide.h>

void print_slideprops(SlideProps slideprops) {
  printf("slidepath : %s\n", slideprops.slidepath.c_str());
  printf("vendor    : %s\n", slideprops.vendor.c_str());
  printf("mpp       : %f\n", slideprops.mpp);
  printf("height    : %d\n", slideprops.height);
  printf("width     : %d\n", slideprops.width);
  for (int i = 0; i < slideprops.level_count; i++) {
    printf("level[%d]  : %8.2f (%7d, %7d)\n", i,
           slideprops.level_downsamples[i], slideprops.level_widths[i],
           slideprops.level_heights[i]);
  }
}

SlideProps get_slideprops(std::string slidepath) {
  // Open the slide
  openslide_t *wsi = openslide_open(slidepath.c_str());

  // Refs
  SlideProps slideprops;
  slideprops.slidepath = slidepath;
  slideprops.vendor = openslide_get_property_value(wsi, "openslide.vendor");
  slideprops.comment = openslide_get_property_value(wsi, "openslide.comment");

  // Props
  slideprops.mpp = stof(openslide_get_property_value(wsi, "openslide.mpp-x"));
  slideprops.height =
      stoi(openslide_get_property_value(wsi, "openslide.level[0].height"));
  slideprops.width =
      stoi(openslide_get_property_value(wsi, "openslide.level[0].width"));
  slideprops.level_count =
      stoi(openslide_get_property_value(wsi, "openslide.level-count"));

  // Get sizes and scale for all levels
  for (int i = 0; i < slideprops.level_count; i++) {
    char name[256];

    // downsample
    float downsample;
    sprintf(name, "openslide.level[%d].downsample", i);
    downsample = stof(openslide_get_property_value(wsi, name));
    slideprops.level_downsamples.push_back(downsample);

    // height
    int height;
    sprintf(name, "openslide.level[%d].height", i);
    height = stoi(openslide_get_property_value(wsi, name));
    slideprops.level_heights.push_back(height);

    // width
    int width;
    sprintf(name, "openslide.level[%d].width", i);
    width = stoi(openslide_get_property_value(wsi, name));
    slideprops.level_widths.push_back(width);
  }

  slideprops.wsi = wsi;
  return slideprops;
}

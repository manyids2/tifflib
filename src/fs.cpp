#include "fs.h"
#include <array>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

FileSystem::FileSystem(std::string base_dir) {
  this->base_dir = fs::path(base_dir);
  printf("Initializing in: %s\n", this->base_dir.c_str());
  if (!fs::exists(this->base_dir)) {
    printf("Dir does not exist : %s\nExiting...\n", this->base_dir.c_str());
    exit(0);
  }

  // Compile list of dirs
  std::vector<fs::path> groups;
  for (auto const &file : std::filesystem::directory_iterator{this->base_dir}) {
    if (fs::is_directory(file)) {
      groups.push_back(file.path());
    }
  }
  this->groups = groups;

  // Compile list of slides
  std::map<std::string, std::map<std::string, fs::path>> slides;
  for (auto const &group : this->groups) {
    for (auto const &file : std::filesystem::directory_iterator{group}) {
      fs::path manifest = file.path() / "MANIFEST.yml";
      fs::path slidepath = file.path() / "image.wsi";
      if (fs::is_directory(file) &&
          (fs::exists(manifest) || fs::exists(slidepath))) {
        slides[group.stem()][file.path().stem()] = file;
      }
    }
  }
}

void FileSystem::print() {
  printf("FileSystem:\n  base_dir: %s\n", this->base_dir.c_str());

  for (auto const &group : this->groups) {
    printf("    Group: %s\n", group.c_str());
    for (auto const &slidename : this->slides[group.stem()]) {
      printf("      Slide: %s - %s\n", slidename.first.c_str(),
             slidename.second.c_str());
    }
  }
}

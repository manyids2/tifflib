#include <cstdio>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class FileSystem {

public:
  FileSystem(std::string base_dir);
  fs::path base_dir;
  std::vector<fs::path> groups;
  std::map<std::string, std::map<std::string, fs::path>> slides;

  void print();

private:
};

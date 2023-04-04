cmake_minimum_required(VERSION 3.10)

project(
  tifflib
  VERSION 0.0.1
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(FindPkgConfig)
include(GNUInstallDirs)

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
find_package(X11 REQUIRED)
find_package(glfw3 REQUIRED)
pkg_check_modules(OPENSLIDE IMPORTED_TARGET REQUIRED openslide)
pkg_check_modules(CXXOPTS IMPORTED_TARGET REQUIRED cxxopts)

find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})
link_directories(${GLM_LIBRARY_DIRS})
add_definitions(${GLM_DEFINITIONS})

add_subdirectory(src)

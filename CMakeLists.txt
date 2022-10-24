cmake_minimum_required(VERSION 3.10)

project(tifflib VERSION 0.0.1 LANGUAGES CXX)

option(WITH_OPENSLIDE_SUPPORT "Enables support to scientific OpenSlide formats" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(FindPkgConfig)
include(GNUInstallDirs)

# if(WITH_OPENSLIDE_SUPPORT)
  pkg_check_modules(OPENSLIDE IMPORTED_TARGET REQUIRED openslide)
  pkg_check_modules(AVUTIL REQUIRED IMPORTED_TARGET libavutil)
  pkg_check_modules(SWSCALE IMPORTED_TARGET REQUIRED libswscale)
  pkg_check_modules(CXXOPTS IMPORTED_TARGET REQUIRED cxxopts)
# endif()

find_package(Threads)

add_subdirectory(src)
# add_subdirectory(man)

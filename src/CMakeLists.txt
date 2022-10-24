set(CMAKE_CXX_FLAGS "-W -Wall -Wextra -Wno-unused-parameter -O3 ${CMAKE_CXX_FLAGS}")

add_executable(tifflib tifflib.cpp)
target_sources(tifflib PRIVATE
  # kitty-canvas.h    kitty-canvas.cc
)

target_link_libraries(tifflib Threads::Threads)
target_link_libraries(tifflib PkgConfig::CXXOPTS)
target_include_directories(tifflib PRIVATE ${CMAKE_BINARY_DIR}/src)

target_compile_features(tifflib PRIVATE cxx_std_14)

if(WITH_OPENSLIDE_SUPPORT)
  target_sources(tifflib PUBLIC openslide-source.h openslide-source.cc)
  target_compile_definitions(tifflib PUBLIC WITH_TIMG_OPENSLIDE_SUPPORT)
  target_link_libraries(tifflib PkgConfig::OPENSLIDE
    PkgConfig::AVUTIL
    PkgConfig::SWSCALE)
endif()

install(TARGETS tifflib
  RUNTIME DESTINATION bin
)

set(CMAKE_CXX_FLAGS
    "-W -Wall -Wextra -Wno-unused-parameter -O3 ${CMAKE_CXX_FLAGS}")

# Actual executable
message("-- Adding: tifflib")
set(sources tifflib.cpp kitty_util.h kitty_util.cpp gl_util.h gl_util.cpp)
add_executable(tifflib ${sources})

# Linking libraries
target_link_libraries(
  tifflib
  ${CXXOPTS}
  ${X11}
  glfw
  OpenGL
  GLEW
  openslide)

# Dont know why
target_link_libraries(tifflib ${CMAKE_DL_LIBS})

# copy font file to the build directory
add_custom_command(
  TARGET ${CMAKE_PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/verdana.ttf
          ${CMAKE_CURRENT_BINARY_DIR}/verdana.ttf)

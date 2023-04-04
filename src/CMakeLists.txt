set(CMAKE_CXX_FLAGS
    "-W -Wall -Wextra -Wno-unused-parameter -O3 ${CMAKE_CXX_FLAGS}")

add_library("glad" "../include/glad/glad.cpp")
set_target_properties("glad" PROPERTIES LINKER_LANGUAGE CXX)
include_directories("../include/glad")

# Actual executable
message("-- Adding: tifflib")
set(sources
    tifflib.cpp
    slide.h
    slide.cpp
    window.h
    window.cpp
    fs.h
    fs.cpp
    gl_util.h
    gl_util.cpp
    kitty_util.h
    kitty_util.cpp
    lib.h
    shader.h
    ../include/glad/glad.h
    ../include/glad/glad.cpp)
add_executable(tifflib ${sources})

# Linking libraries
target_link_libraries(
  tifflib
  ${CXXOPTS}
  ${X11}
  OSMesa
  "glad"
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

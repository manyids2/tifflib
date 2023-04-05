// To display version number
#include "window.h"
#include <cstdio>
#include <cxxopts.hpp>
#include <iterator>
#include <numeric>

static uint width = 256, height = 256;
static uint millis = 10;
static uint compression = 0;
static size_t bytes_rendered = 0;
static size_t bytes_transferred = 0;
int running = 1;

static const char *frag_shader_filename = "../include/shaders/gears.fsh";
static const char *vert_shader_filename = "../include/shaders/gears.vsh";

static GLuint program;
static GLuint vao[3], vbo[3], ibo[3];
static vertex_buffer vb[3];
static index_buffer ib[3];

#define vertex_3f(x, y, z, u, v)                                               \
  vertex_buffer_add(vb, (vertex){{x, y, z}, norm, {u, v}, col})
#define normal_3f(x, y, z)                                                     \
  norm = (vec3f) { x, y, z }

static inline void normalize2f(float v[2]) {
  float len = sqrtf(v[0] * v[0] + v[1] * v[1]);
  v[0] /= len;
  v[1] /= len;
}

static void keystroke(int key) {
  switch (key) {
  case 'q':
    running = 0;
    break;
  default:
    return;
  }
}

static void reshape(int width, int height) {
  glViewport(0, 0, (GLint)width, (GLint)height);
}

static void draw_on_buffer(uint8_t *buffer, int width, int height, int a) {
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      buffer[((x * a) * height) + (y * a)] = 255;
    }
  }
}

static void gear(vertex_buffer *vb, index_buffer *ib, float inner_radius,
                 float outer_radius, float width, int teeth, float tooth_depth,
                 vec4f col) {
  int i;
  float r0, r1, r2, nr0, nr1, nr2;
  float angle, da, ca0, sa0, ca1, sa1, ca2, sa2, ca3, sa3, ca4, sa4;
  float u, v, len;
  float tmp[2];
  vec3f norm;
  vec2f uv;
  uint idx;

  r0 = inner_radius;
  r1 = outer_radius - tooth_depth / 2.f;
  r2 = outer_radius + tooth_depth / 2.f;
  nr0 = r0 / r2;
  nr1 = r1 / r2;
  nr2 = 1.f;
  da = 2.f * (float)M_PI / teeth / 4.f;
  uv = (vec2f){0, 0};

  normal_3f(0.f, 0.f, 1.f);

  /* draw front face */
  idx = vertex_buffer_count(vb);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.f * (float)M_PI / teeth;
    ca0 = cosf(angle);
    sa0 = sinf(angle);
    ca3 = cosf(angle + 3 * da);
    sa3 = sinf(angle + 3 * da);
    vertex_3f(r0 * ca0, r0 * sa0, width * 0.5f, nr0 * ca0, nr0 * sa0);
    vertex_3f(r1 * ca0, r1 * sa0, width * 0.5f, nr1 * ca0, nr1 * sa0);
    if (i < teeth) {
      vertex_3f(r0 * ca0, r0 * sa0, width * 0.5f, nr0 * ca0, nr0 * sa0);
      vertex_3f(r1 * ca3, r1 * sa3, width * 0.5f, nr1 * ca3, nr1 * sa3);
    }
  }
  index_buffer_add_primitves(ib, primitive_topology_quad_strip, teeth * 2, idx);

  /* draw front sides of teeth */
  da = 2.f * (float)M_PI / teeth / 4.f;
  idx = vertex_buffer_count(vb);
  for (i = 0; i < teeth; i++) {
    angle = i * 2.f * (float)M_PI / teeth;
    ca0 = cosf(angle);
    sa0 = sinf(angle);
    ca1 = cosf(angle + 1 * da);
    sa1 = sinf(angle + 1 * da);
    ca2 = cosf(angle + 2 * da);
    sa2 = sinf(angle + 2 * da);
    ca3 = cosf(angle + 3 * da);
    sa3 = sinf(angle + 3 * da);
    vertex_3f(r1 * ca0, r1 * sa0, width * 0.5f, nr1 * ca0, nr1 * sa0);
    vertex_3f(r2 * ca1, r2 * sa1, width * 0.5f, nr2 * ca1, nr2 * sa1);
    vertex_3f(r2 * ca2, r2 * sa2, width * 0.5f, nr2 * ca2, nr2 * sa2);
    vertex_3f(r1 * ca3, r1 * sa3, width * 0.5f, nr1 * ca3, nr1 * sa3);
  }
  index_buffer_add_primitves(ib, primitive_topology_quads, teeth, idx);

  normal_3f(0.f, 0.f, -1.f);

  /* draw back face */
  idx = vertex_buffer_count(vb);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.f * (float)M_PI / teeth;
    ca0 = cosf(angle);
    sa0 = sinf(angle);
    ca3 = cosf(angle + 3 * da);
    sa3 = sinf(angle + 3 * da);
    vertex_3f(r1 * ca0, r1 * sa0, -width * 0.5f, nr1 * ca0, nr1 * sa0);
    vertex_3f(r0 * ca0, r0 * sa0, -width * 0.5f, nr0 * ca0, nr0 * sa0);
    if (i < teeth) {
      vertex_3f(r1 * ca3, r1 * sa3, -width * 0.5f, nr1 * ca3, nr1 * sa3);
      vertex_3f(r0 * ca0, r0 * sa0, -width * 0.5f, nr0 * ca0, nr0 * sa0);
    }
  }
  index_buffer_add_primitves(ib, primitive_topology_quad_strip, teeth * 2, idx);

  /* draw back sides of teeth */
  da = 2.f * (float)M_PI / teeth / 4.f;
  idx = vertex_buffer_count(vb);
  for (i = 0; i < teeth; i++) {
    angle = i * 2.f * (float)M_PI / teeth;
    ca0 = cosf(angle);
    sa0 = sinf(angle);
    ca1 = cosf(angle + 1 * da);
    sa1 = sinf(angle + 1 * da);
    ca2 = cosf(angle + 2 * da);
    sa2 = sinf(angle + 2 * da);
    ca3 = cosf(angle + 3 * da);
    sa3 = sinf(angle + 3 * da);
    vertex_3f(r1 * ca3, r1 * sa3, -width * 0.5f, nr1 * ca3, nr1 * sa3);
    vertex_3f(r2 * ca2, r2 * sa2, -width * 0.5f, nr2 * ca2, nr2 * sa2);
    vertex_3f(r2 * ca1, r2 * sa1, -width * 0.5f, nr2 * ca1, nr2 * sa1);
    vertex_3f(r1 * ca0, r1 * sa0, -width * 0.5f, nr1 * ca0, nr1 * sa0);
  }
  index_buffer_add_primitves(ib, primitive_topology_quads, teeth, idx);

  /* draw outward faces of teeth */
  idx = vertex_buffer_count(vb);
  for (i = 0; i < teeth; i++) {
    angle = i * 2.f * (float)M_PI / teeth;
    ca0 = cosf(angle);
    sa0 = sinf(angle);
    ca1 = cosf(angle + 1 * da);
    sa1 = sinf(angle + 1 * da);
    ca2 = cosf(angle + 2 * da);
    sa2 = sinf(angle + 2 * da);
    ca3 = cosf(angle + 3 * da);
    sa3 = sinf(angle + 3 * da);
    ca4 = cosf(angle + 4 * da);
    sa4 = sinf(angle + 4 * da);
    tmp[0] = r2 * ca1 - r1 * ca0;
    tmp[1] = r2 * sa1 - r1 * sa0;
    normalize2f(tmp);
    normal_3f(tmp[1], -tmp[0], 0.f);
    vertex_3f(r1 * ca0, r1 * sa0, width * 0.5f, nr1 * ca0, nr1 * sa0);
    vertex_3f(r1 * ca0, r1 * sa0, -width * 0.5f, nr1 * ca0, nr1 * sa0);
    vertex_3f(r2 * ca1, r2 * sa1, -width * 0.5f, nr2 * ca1, nr2 * sa1);
    vertex_3f(r2 * ca1, r2 * sa1, width * 0.5f, nr2 * ca1, nr2 * sa1);
    normal_3f(ca0, sa0, 0.f);
    vertex_3f(r2 * ca1, r2 * sa1, width * 0.5f, nr2 * ca1, nr2 * sa1);
    vertex_3f(r2 * ca1, r2 * sa1, -width * 0.5f, nr2 * ca1, nr2 * sa1);
    vertex_3f(r2 * ca2, r2 * sa2, -width * 0.5f, nr2 * ca2, nr2 * sa2);
    vertex_3f(r2 * ca2, r2 * sa2, width * 0.5f, nr2 * ca2, nr2 * sa2);
    tmp[0] = r1 * ca3 - r2 * ca2;
    tmp[1] = r1 * sa3 - r2 * sa2;
    normalize2f(tmp);
    normal_3f(tmp[1], -tmp[0], 0.f);
    vertex_3f(r2 * ca2, r2 * sa2, width * 0.5f, nr2 * ca2, nr2 * sa2);
    vertex_3f(r2 * ca2, r2 * sa2, -width * 0.5f, nr2 * ca2, nr2 * sa2);
    vertex_3f(r1 * ca3, r1 * sa3, -width * 0.5f, nr1 * ca3, nr1 * sa3);
    vertex_3f(r1 * ca3, r1 * sa3, width * 0.5f, nr1 * ca3, nr1 * sa3);
    normal_3f(ca0, sa0, 0.f);
    vertex_3f(r1 * ca3, r1 * sa3, width * 0.5f, nr1 * ca3, nr1 * sa3);
    vertex_3f(r1 * ca3, r1 * sa3, -width * 0.5f, nr1 * ca3, nr1 * sa3);
    vertex_3f(r1 * ca4, r1 * sa4, -width * 0.5f, nr1 * ca4, nr1 * sa4);
    vertex_3f(r1 * ca4, r1 * sa4, width * 0.5f, nr1 * ca4, nr1 * sa4);
  }
  index_buffer_add_primitves(ib, primitive_topology_quads, teeth * 4, idx);

  /* draw inside radius cylinder */
  idx = vertex_buffer_count(vb);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.f * (float)M_PI / teeth;
    ca0 = cosf(angle);
    sa0 = sinf(angle);
    normal_3f(-ca0, -sa0, 0.f);
    vertex_3f(r0 * ca0, r0 * sa0, -width * 0.5f, nr0 * ca0, nr0 * sa0);
    vertex_3f(r0 * ca0, r0 * sa0, width * 0.5f, nr0 * ca0, nr0 * sa0);
  }
  index_buffer_add_primitves(ib, primitive_topology_quad_strip, teeth, idx);
}

static void init(void) {
  printf("Starting init\n");
  GLuint shaders[2];
  printf("GLuint shaders[2]\n");

  /* shader program */
  shaders[0] = compile_shader(GL_VERTEX_SHADER, vert_shader_filename);
  printf("compiled shaders[0]\n");
  shaders[1] = compile_shader(GL_FRAGMENT_SHADER, frag_shader_filename);
  printf("compiled shaders[1]\n");
  program = link_program(shaders, 2, NULL);
  printf("linked program\n");

  /* create gear vertex and index buffers */
  for (size_t i = 0; i < 3; i++) {
    vertex_buffer_init(&vb[i]);
    index_buffer_init(&ib[i]);
  }
  gear(&vb[0], &ib[0], 1.f, 4.f, 1.f, 20, 0.7f, (vec4f){0.8f, 0.1f, 0.f, 1.f});
  gear(&vb[1], &ib[1], 0.5f, 2.f, 2.f, 10, 0.7f, (vec4f){0.f, 0.8f, 0.2f, 1.f});
  gear(&vb[2], &ib[2], 1.3f, 2.f, 0.5f, 10, 0.7f,
       (vec4f){0.2f, 0.2f, 1.f, 1.f});

  /* create vertex array, vertex buffer and index buffer objects */
  for (size_t i = 0; i < 3; i++) {
    glGenVertexArrays(1, &vao[i]);
    glBindVertexArray(vao[i]);
    vertex_buffer_create(&vbo[i], GL_ARRAY_BUFFER, vb[i].data,
                         vb[i].count * sizeof(vertex));
    vertex_buffer_create(&ibo[i], GL_ELEMENT_ARRAY_BUFFER, ib[i].data,
                         ib[i].count * sizeof(uint));
    vertex_array_pointer("a_pos", 3, GL_FLOAT, 0, sizeof(vertex),
                         offsetof(vertex, pos));
    vertex_array_pointer("a_normal", 3, GL_FLOAT, 0, sizeof(vertex),
                         offsetof(vertex, norm));
    vertex_array_pointer("a_uv", 2, GL_FLOAT, 0, sizeof(vertex),
                         offsetof(vertex, uv));
    vertex_array_pointer("a_color", 4, GL_FLOAT, 0, sizeof(vertex),
                         offsetof(vertex, col));
  }

  /* set light position uniform */
  glUseProgram(program);
  uniform_3f("u_lightpos", 5.f, 5.f, 10.f);

  /* enable OpenGL capabilities */
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

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

  /* SlideProps slide = get_slideprops(slidepath); */
  /* print_slideprops(slide); */

  std::string maskpath;
  maskpath = result["maskpath"].as<std::string>();
  std::cout << maskpath << std::endl;

  OSMesaContext ctx;
  uint8_t *buffer;
  uint lh = height / 18;
  uint frame;
  size_t len;
  pos p;

  /* Create an RGBA-mode context */
  if (!(ctx = OSMesaCreateContextExt(OSMESA_RGBA, 16, 0, 0, NULL))) {
    fprintf(stderr, "OSMesaCreateContext failed!\n");
    return 0;
  }

  /* Allocate the image buffer */
  if (!(buffer = (uint8_t *)malloc(width * height * sizeof(uint)))) {
    fprintf(stderr, "Alloc image buffer failed!\n");
    return 0;
  }

  /* Bind the buffer to the context and make it current */
  if (!OSMesaMakeCurrent(ctx, buffer, GL_UNSIGNED_BYTE, width, height)) {
    fprintf(stderr, "OSMesaMakeCurrent failed!\n");
    return 0;
  }

  kitty_key_callback(keystroke);
  draw_on_buffer(buffer, width, height, 0);
  draw_on_buffer(buffer, width, height, 3);

  init();
  exit(1);
  /* reshape(width, height); */

  for (uint i = 0; i < lh; i++)
    printf("\n");

  kitty_setup_termios();
  p = kitty_get_position();
  kitty_hide_cursor();

  /* loop displaying frames */
  static uint count = 1000;
  running = 1;
  for (frame = 0; frame < count && running; frame++) {
    /* glClearColor(0.0, 0.0, 0.0, 0.0); */
    /* glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); */
    /* glFlush(); */

    /* flip buffer and output to kitty as base64 RGBA data*/
    uint iid = 2 + (frame & 1);
    kitty_set_position(p.x, p.y - lh);
    /* kitty_flip_buffer_y((uint *)buffer, width, height); */
    len = kitty_send_rgba('T', iid, compression, buffer, width, height);

    bytes_rendered += (width * height) << 2;
    bytes_transferred += len;

    kitty_poll_events(millis);
  }

  /* drain kitty responses */
  kitty_poll_events(millis);

  /* restore cursor position then show cursor */
  kitty_show_cursor();
  kitty_set_position(p.x, p.y);
  kitty_restore_termios();
  printf("\n");

  /* release memory and exit */
  OSMesaDestroyContext(ctx);
  free(buffer);
  exit(EXIT_SUCCESS);

  return 0;
}

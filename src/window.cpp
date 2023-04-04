#include "window.h"
#include <cstdlib>

int getBoxGridVerticesIndices(float *gridVertices, unsigned int *gridIndices,
                              int width, int height) {
  /* gridVertices = {0, 0, 0, height, width, height, width, 0}; */
  gridVertices[0] = 0;
  gridVertices[1] = 0;
  gridVertices[2] = 0;
  gridVertices[3] = 0;

  gridVertices[4] = 0;
  gridVertices[5] = height;
  gridVertices[6] = 0;
  gridVertices[7] = 1;

  gridVertices[8] = width;
  gridVertices[9] = height;
  gridVertices[10] = 1;
  gridVertices[11] = 1;

  gridVertices[12] = width;
  gridVertices[13] = 0;
  gridVertices[14] = 1;
  gridVertices[15] = 0;

  /* gridIndices = {0, 1, 2, 2, 3, 0}; */
  gridIndices[0] = 0;
  gridIndices[1] = 1;
  gridIndices[2] = 2;
  gridIndices[3] = 2;
  gridIndices[4] = 3;
  gridIndices[5] = 0;
  return 4;
}

// GLFW stuff
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

Window::Window(const int width, const int height)
    : width(width), height(height) {
  // Init variables
  this->window = nullptr;
  this->framebufferWidth = width;
  this->framebufferHeight = height;

  this->posX = 0.0;
  this->posY = 0.0;
  this->scale = 1.0;
  this->level = 1;
  this->dirty = true;

  // mesa stuff
  this->lh = height / 18;

  /* Create an RGBA-mode context */
  if (!(this->ctx = OSMesaCreateContextExt(OSMESA_RGBA, 16, 0, 0, NULL))) {
    fprintf(stderr, "OSMesaCreateContext failed!\n");
    exit(1);
  }

  /* Allocate the image buffer */
  if (!(buffer = (uint8_t *)malloc(width * height * sizeof(uint)))) {
    fprintf(stderr, "Alloc image buffer failed!\n");
    exit(1);
  }

  /* Bind the buffer to the context and make it current */
  if (!OSMesaMakeCurrent(this->ctx, this->buffer, GL_UNSIGNED_BYTE, width,
                         height)) {
    fprintf(stderr, "OSMesaMakeCurrent failed!\n");
    exit(1);
  }

  // Open the window
  /* fprintf(stderr, "Initializing GLFW\n"); */
  /* this->initGLFW(); */
  /* fprintf(stderr, "Inited GLFW\n"); */

  /* fprintf(stderr, "Initializing Window\n"); */
  /* this->initWindow(); */
  /* fprintf(stderr, "Inited Window\n"); */
  /* this->showing = false; */
  /**/
  /* // Shaders */
  /* fprintf(stderr, "Initializing Shaders\n"); */
  /* this->initShaders(); */
  /* fprintf(stderr, "Inited Shaders\n"); */
  /**/
  /* // Initialize with window params */
  /* fprintf(stderr, "Initializing Vertices\n"); */
  /* this->initVertices(this->width, this->height); */
  /* fprintf(stderr, "Inited Vertices\n"); */
}

Window::~Window() {
  /* glDeleteVertexArrays(1, &this->VAO); */
  /* glDeleteBuffers(1, &this->VBO); */
  /* glDeleteBuffers(1, &this->EBO); */
  /* glDeleteProgram(this->shaderProgram); */
  /**/
  /* glfwDestroyWindow(this->window); */
  /* glfwTerminate(); */

  OSMesaDestroyContext(this->ctx);
  free(this->buffer);
}

void Window::initGLFW() {
  if (glfwInit() == GLFW_FALSE) {
    std::cout << "ERROR::GLFW_INIT_FAILED" << std::endl;
    glfwTerminate();
  }
}

void Window::initShaders() {
  // build and compile our shader program
  std::cout << "Initing ::Shader" << std::endl;
  Shader ourShader("../include/shaders/viewShader.vs",
                   "../include/shaders/viewShader.fs");
  this->shaderProgram = ourShader.ID;
  std::cout << "Using ::Shader" << std::endl;
  ourShader.use();
}

void Window::initVertices(int width, int height) {
  // set up vertex data to match grid
  // ------------------------------------------------------------------
  std::cout << "Initing ::Vertices" << std::endl;
  float gridVertices[16] = {0.0};
  unsigned int gridIndices[6] = {0};

  // Put the thumbnail dimensions
  int gridCount =
      getBoxGridVerticesIndices(gridVertices, gridIndices, width, height);
  this->gridCount = gridCount;

  // Vertex, indices arrays
  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &this->VBO);
  glGenBuffers(1, &this->EBO);

  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and
  // then configure vertex attributes(s).
  // 1. bind Vertex Array Object
  glBindVertexArray(this->VAO);
  // 2. copy our vertices array in a vertex buffer for OpenGL to use
  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(gridVertices), gridVertices,
               GL_STATIC_DRAW);
  // 3. copy our index array in a element buffer for OpenGL to use
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gridIndices), gridIndices,
               GL_STATIC_DRAW);
  // 4. then set the vertex attributes pointers
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO
  // as the vertex attribute's bound vertex buffer object so afterwards we can
  // safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // You can unbind the VAO afterwards so other VAO calls won't accidentally
  // modify this VAO, but this rarely happens. Modifying other VAOs requires a
  // call to glBindVertexArray anyways so we generally don't unbind VAOs (nor
  // VBOs) when it's not directly necessary.
  glBindVertexArray(0);

  // uncomment this call to draw in wireframe polygons.
  /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */
  std::cout << "Inited  ::Vertices" << std::endl;
}

void Window::initWindow() {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // glfw window creation
  this->window =
      glfwCreateWindow(this->width, this->height, "glfw-window", NULL, NULL);
  if (this->window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  }

  /* // Support for resize */
  /* glfwGetFramebufferSize(this->window, &this->framebufferWidth, */
  /*                        &this->framebufferHeight); */
  /* glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); */

  // IMPORTANT !!! apparently
  glfwMakeContextCurrent(this->window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
  }
}

// Functions
void Window::updateDt() {
  this->curTime = static_cast<float>(glfwGetTime());
  this->dt = this->curTime - this->lastTime;
  this->lastTime = this->curTime;
}

void Window::updateKeyboardInput() {
  if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(this->window, true);
    this->showing = false;
    this->dirty = true;
  }
}

void Window::update() {
  glfwPollEvents();
  this->updateDt();
  this->updateKeyboardInput();
}

void Window::clear() {
  glClearColor(1.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void Window::render() {
  if (this->dirty) {
    glfwSwapBuffers(window);
  }
}

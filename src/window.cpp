#include "window.h"

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

Window::Window(const int WINDOW_WIDTH, const int WINDOW_HEIGHT)
    : WINDOW_WIDTH(WINDOW_WIDTH), WINDOW_HEIGHT(WINDOW_HEIGHT) {
  // Init variables
  this->window = nullptr;
  this->framebufferWidth = this->WINDOW_WIDTH;
  this->framebufferHeight = this->WINDOW_HEIGHT;

  this->lastMouseX = 0.0;
  this->lastMouseY = 0.0;
  this->mouseX = 0.0;
  this->mouseY = 0.0;
  this->panning = false;

  this->posX = 0.0;
  this->posY = 0.0;
  this->scale = 1.0;
  this->level = 1;
  this->dirty = true;

  // Open the window
  this->initGLFW();
  this->initWindow();
  this->showing = true;

  // Shaders
  this->initShaders();

  // Initialize with window params
  this->initVertices(this->WINDOW_WIDTH, this->WINDOW_HEIGHT);

  // Initialize our state properly
  glfwGetCursorPos(this->window, &this->mouseX, &this->mouseY);
  glfwGetCursorPos(this->window, &this->lastMouseX, &this->lastMouseY);
}

Window::~Window() {
  glDeleteVertexArrays(1, &this->VAO);
  glDeleteBuffers(1, &this->VBO);
  glDeleteBuffers(1, &this->EBO);
  glDeleteProgram(this->shaderProgram);

  glfwDestroyWindow(this->window);
  glfwTerminate();
}

void Window::initGLFW() {
  if (glfwInit() == GLFW_FALSE) {
    std::cout << "ERROR::GLFW_INIT_FAILED" << std::endl;
    glfwTerminate();
  }
}

void Window::initShaders() {
  // build and compile our shader program
  Shader ourShader("../include/internal/shaders/viewShader.vs",
                   "../include/internal/shaders/viewShader.fs");
  this->shaderProgram = ourShader.ID;
  ourShader.use();
}

void Window::initVertices(int width, int height) {
  // set up vertex data to match grid
  // ------------------------------------------------------------------
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
}

void Window::initWindow() {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // glfw window creation
  this->window = glfwCreateWindow(this->WINDOW_WIDTH, this->WINDOW_HEIGHT,
                                  "glfw-window", NULL, NULL);
  if (this->window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  }

  // Support for resize
  glfwGetFramebufferSize(this->window, &this->framebufferWidth,
                         &this->framebufferHeight);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

void Window::handlePanning() {
  this->posX += (float)(this->mouseX - this->lastMouseX);
  this->posY += (float)(this->mouseY - this->lastMouseY);
}

void Window::handleZooming() {
  this->posX =
      (this->posX - this->mouseX) * (this->scale / lastScale) + this->mouseX;
  this->posY =
      (this->posY - this->mouseY) * (this->scale / lastScale) + this->mouseY;
}

// Just trigger stuff, let callback handle it
void Window::updateMouseInput() {
  this->panning = false;
  glfwGetCursorPos(this->window, &this->mouseX, &this->mouseY);
  if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
    // Update state
    switch (this->mode) {
    case 0:
      this->panning = true;
      this->handlePanning();
      break;
    case 1:
      this->panning = true;
      this->handlePanning();
      break;
    default:
      break;
    }
    this->dirty = true;
  }
  this->lastMouseX = this->mouseX;
  this->lastMouseY = this->mouseY;
}

void Window::updateKeyboardInput() {
  if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(this->window, true);
    this->showing = false;
    this->dirty = true;
  }

  // TODO: refactor to just change scale and calc later
  this->lastScale = this->scale;

  // W - zoom in
  if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS) {
    this->scale = this->scale * 1.05;
    this->handleZooming();
    this->dirty = true;
  }

  // S - zoom out
  if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS) {
    this->scale = this->scale / 1.05;
    this->handleZooming();
    this->dirty = true;
  }

  // P - switch mode
  if (glfwGetKey(this->window, GLFW_KEY_P) == GLFW_PRESS) {
    if (this->lastTime - this->debounce > 0.2) {
      this->mode = (this->mode + 1) % this->numModes;
      this->debounce = this->lastTime;
    }
  }
}

// Just trigger stuff, let callback handle it
void Window::updateTiles() {
  if (this->dirty) {
    this->level = (int)this->scale;
  }
}

void Window::update() {
  glfwPollEvents();
  this->updateDt();
  this->updateMouseInput();
  this->updateKeyboardInput();
  this->updateTiles();
}

void Window::clear() {
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);
  this->drawThumbnail();
}

void Window::drawThumbnail() {
  // bind textures on corresponding texture units
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, this->thumbnail);

  int windowSizeLocation =
      glGetUniformLocation(this->shaderProgram, "windowSize");

  int viewStateLocation =
      glGetUniformLocation(this->shaderProgram, "viewState");

  // draw our first triangle
  glUseProgram(this->shaderProgram);
  glUniform2f(windowSizeLocation, (float)this->WINDOW_WIDTH,
              (float)this->WINDOW_HEIGHT);
  glUniform3f(viewStateLocation, this->posX, this->posY, this->scale);

  glBindVertexArray(this->VAO); // seeing as we only have a single VAO there's
                                // no need to bind it every time, but we'll do
                                // so to keep things a bit more organized
  glDrawElements(GL_TRIANGLES, 6 * this->gridCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Window::render() {
  if (this->dirty) {
    glfwSwapBuffers(window);
  }
}

void Window::initThumbnail() {
  // Open the slide and get smallest layer
  openslide_t *wsi = openslide_open(this->slide.slidepath.c_str());
  int level = this->slide.level_count - 1;
  /* int level = 1; */
  int smallestHeight = this->slide.level_heights[level];
  int smallestWidth = this->slide.level_widths[level];

  // Reinit vertex array objects
  this->initVertices(smallestWidth, smallestHeight);

  // Allocate buffer
  uint32_t *buffer;
  buffer = (uint32_t *)malloc(smallestHeight * smallestWidth * 4);
  openslide_read_region(wsi, buffer, 0, 0, level, smallestWidth,
                        smallestHeight);

  // Initialize texture
  glGenTextures(1, &this->thumbnail);
  glBindTexture(GL_TEXTURE_2D, this->thumbnail);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, smallestWidth, smallestHeight, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, buffer);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Free buffer and wsi connection
  free(buffer);
  openslide_close(wsi);
}

void Window::setSlideprops(SlideProps slide) {
  this->slide = slide;
  this->initThumbnail();
}

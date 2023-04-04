#include "lib.h" // glad, GLFW, etc.
#include "slide.h"

class Window {
private:
  // Position and scale (x, y, s) to define viewport
  float posX;
  float posY;
  float scale;
  int level;
  bool dirty;
  bool showing = false;

  // View / Draw
  int mode = 0;
  int numModes = 2;

  // Delta time
  float dt;
  float curTime;
  float lastTime;
  float debounce = 0;

  // Mouse Input
  double lastMouseX;
  double lastMouseY;
  double lastScale;
  double mouseX;
  double mouseY;
  bool panning;

  // Variables
  const int WINDOW_WIDTH;
  const int WINDOW_HEIGHT;
  int framebufferWidth;
  int framebufferHeight;

  // Tiling
  int tileSize = 256;
  int gridCount = 0;

  // Shaders
  unsigned int shaderProgram;
  unsigned int VAO, VBO, EBO;

  // Textures
  unsigned int thumbnail;

  // As struct
  SlideProps slide;
  bool validSlideprops = false;

  // Private functions
  void initGLFW();
  void initWindow();
  void initShaders();
  void initVertices(int width, int height);
  void initThumbnail();

public:
  GLFWwindow *window;

  // Constructors / Destructors
  Window(const int WINDOW_WIDTH, const int WINDOW_HEIGHT);
  virtual ~Window();

  void setSlideprops(SlideProps slide);

  void updateDt();

  void handlePanning();
  void handleZooming();
  void updateMouseInput();
  void updateTiles();

  void updateKeyboardInput();

  void drawThumbnail();
  void clear();
  void update();
  void render();

  // Accessors
  inline bool getShowing() { return this->showing; };
  inline void setShowing(bool showing) { this->showing = showing; };

  // State
  inline void getState(float *x, float *y, float *s, int *l) {
    *x = this->posX;
    *y = this->posY;
    *s = this->scale;
    *l = this->level;
  };
};

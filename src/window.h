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

public:
  GLFWwindow *window;

  // Variables
  const int width = 500;
  const int height = 500;
  int framebufferWidth = 500;
  int framebufferHeight = 500;
  uint compression = 0;
  uint millis = 10;

  // Open mesa shite
  OSMesaContext ctx;
  uint8_t *buffer;
  uint lh = height / 18;
  uint frame;
  size_t len;
  pos p;

  // Constructors / Destructors
  Window(const int width, const int height);
  virtual ~Window();

  void updateDt();
  void updateKeyboardInput();
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

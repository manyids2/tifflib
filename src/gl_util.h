/*
 * 3-D gear wheels.  This program is in the public domain.
 *
 * === History ===
 *
 * Michael Clark
 *   - Conversion from fixed function to programmable shaders
 *   - Conversion to OSMesa and kitty
 *
 * Marcus Geelnard:
 *   - Conversion to GLFW
 *   - Time based rendering (frame rate independent)
 *   - Slightly modified camera that should work better for stereo viewing
 *
 * Camilla Löwy:
 *   - Removed FPS counter (this is not a benchmark)
 *   - Added a few comments
 *   - Enabled vsync
 *
 * Brian Paul
 *   - Orignal version
 */

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static uint running = 1;

/*
 * keyboard dispatch
 */
static void keystroke(int key) {
  switch (key) {
  case 'q':
    running = 0;
    break;
  default:
    return;
  }
}

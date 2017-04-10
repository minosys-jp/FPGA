#include "ap_cint.h"

#define SCREEN_WIDTH	(640)
#define abs(x)		(((x) >= 0) ? (x) : (-x))
#define sgn(x, y)		(((x) > 0) ? (y) : ((x) < 0) ? (-y) : 0)

void simpleLine(volatile unsigned short *vram, short xpos, short ypos, short dx, short dy, unsigned short count, unsigned short color) {
  unsigned short c;
  short delta = 0;
  unsigned short adx = abs(dx);
  unsigned short ady = abs(dy);
  int sdx = sgn(dx, 1);
  int sdy = sgn(dy, SCREEN_WIDTH);
  short a, b;
  int sx, sy;

  vram = vram + ypos * SCREEN_WIDTH + xpos;
  if (adx >= ady) {
    a = adx;
    b = ady;
    sx = sdx;
    sy = sdy;
  } else {
    a = ady;
    b = adx;
    sx = sdy;
    sy = sdx;
  }

  loop_count: for (c = 0; c < count; ++c) {
    *vram = color;
    vram = vram + sx;
    delta = delta + (b << 1);
    vram = vram + ((delta >= a) ? sy : 0);
    delta = delta - ((delta >= a) ? (a << 1) : 0);
  }
}


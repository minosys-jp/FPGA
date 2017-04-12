#include <stdio.h>

#define SCREEN_WIDTH	(640)
#define SCREEN_HEIGHT	(480)
#define VGACOLOR(c, s)  ((c) >> s) & 0xf

void drawFlatTriangle(
  volatile unsigned short *vram,
  unsigned short x1, unsigned short y1,
  unsigned short x2, unsigned short y2,
  unsigned short x3,
  unsigned short c1, unsigned short c2, unsigned short c3
);

int main() {
  static unsigned short vram[SCREEN_WIDTH * SCREEN_HEIGHT];
  FILE *f;
  int i;

  // normal operation
  drawFlatTriangle(vram, 50, 50, 0, 0, 100, 0xf00, 0x0f0, 0x00f);
  drawFlatTriangle(vram, 50, 50, 0, 100, 100, 0xf00, 0x0f0, 0x00f);
  drawFlatTriangle(vram, 150, 50, 200, 0, 300, 0xf00, 0x0f0, 0x00f);
  drawFlatTriangle(vram, 150, 50, 200, 200, 300, 0xf00, 0x0f0, 0x00f);

  // triangles on boundary
  drawFlatTriangle(vram, -10, 250, 50, 200, 100, 0xf00, 0x0f0, 0x00f);
  drawFlatTriangle(vram, -10, 250, 50, 300, 100, 0xf00, 0x0f0, 0x00f);
  drawFlatTriangle(vram, 660, 250, 600, 200, 550, 0xf00, 0x0f0, 0x00f);
  drawFlatTriangle(vram, 660, 250, 600, 300, 550, 0xf00, 0x0f0, 0x00f);
  drawFlatTriangle(vram, 320, 600, 300, 690, 340, 0xf00, 0x0f0, 0x00f);

  // 90 degree angle
  drawFlatTriangle(vram, 300, 200, 300, 280, 340, 0xf00, 0x0f0, 0x00f);
  drawFlatTriangle(vram, 300, 200, 300, 120, 340, 0xf00, 0x0f0, 0x00f);

  // write out the result as ppm
  f = fopen("localram.ppm", "wb");
  fprintf(f, "P6\n640 480\n15\n");
  for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
    fprintf(f, "%c", VGACOLOR(vram[i], 8));
    fprintf(f, "%c", VGACOLOR(vram[i], 4));
    fprintf(f, "%c", VGACOLOR(vram[i], 0));
  }
  fclose(f);
}


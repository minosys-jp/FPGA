#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH	(640)
#define SCREEN_HEIGHT	(480)
#define LINE_LENGTH		(100.0)
#define INNER_LENGTH	(10.0)
#define VGACOLOR(c,p)	((c >> p) & 0xf)

static void line(volatile unsigned short *vram, short xpos, short ypos, short dx, short dy, unsigned short count, unsigned short color) {
  simpleLine(vram, xpos, ypos, dx, dy, count, color);
}

static void drawLine(volatile unsigned short *vram, short xs, short ys, short xe, short ye, unsigned color) {
	short dx = xe - xs;
	short dy = ye - ys;
	unsigned short count = (abs(dx) >= abs(dy)) ? abs(dx): abs(dy);
	line(vram, xs, ys, dx, dy, count, color);
}

static void clearScreen(volatile unsigned short *vram) {
  int pos;
  for (pos = 0; pos < SCREEN_WIDTH * SCREEN_HEIGHT; ++pos) {
    vram[pos] = 0;
  }
}

int main() {
  static unsigned short v[SCREEN_WIDTH * SCREEN_HEIGHT];
  static unsigned short color[] = {
    0x00f, 0x0f0, 0xf00, 0xf0f, 0x0ff, 0xf0f, 0xfff,
    0x008, 0x080, 0x800, 0x808, 0x088, 0x808, 0x888
  };

  int i, j;
  int xh = SCREEN_WIDTH / 2;
  int yh = SCREEN_HEIGHT / 2;
  FILE *f;

  clearScreen(&v[0]);

  for (i = 0, j = 0; i < 360; i = i + 30, j = j + 1) {
    double c = cos(i * M_PI / 180.0);
    double s = sin(i * M_PI / 180.0);
    int x = xh + INNER_LENGTH * c;
    int y = yh + INNER_LENGTH * s;
    line(&v[0], x, y, (short)(c * 10000.0), (short)(s * 10000.0), LINE_LENGTH, color[j]);
  }
  drawLine(&v[0], 0, 0, 639, 20, 0xfff);
  drawLine(&v[0], 0, 20, 639, 0, 0xf00);
  drawLine(&v[0], 0, 0, 20, 479, 0x0f0);
  drawLine(&v[0], 20, 0, 0, 479, 0x00f);

  f = fopen("localram.ppm", "wb");
  fprintf(f, "P6\n640 480\n15\n");
  for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
    fprintf(f, "%c", VGACOLOR(v[i], 8));
    fprintf(f, "%c", VGACOLOR(v[i], 4));
    fprintf(f, "%c", VGACOLOR(v[i], 0));
  }
  fclose(f);
  return 0;
}


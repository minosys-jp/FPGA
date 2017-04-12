#include "ap_cint.h"

#define SCREEN_WIDTH	(640)
#define SCREEN_HEIGHT	(480)

#define abs(x)	(((x) >= 0) ? (x) : (-x))
#define sgn(x, w) (((x) >= 0) ? (w) : ((x) < 0) ? (-w) : 0)
#define VGACOLOR(c, s) (((c) >> s) & 0xf)
#define COLOR(r, g, b) (((unsigned short)r) << 8) | (((unsigned short)g) << 4) | (unsigned short)(b)

// draw X direction
static inline void drawXAxis(volatile unsigned short *vram,
  short y,
  short x2, short x3,
  uint4 c2r, uint4 c2g, uint4 c2b,
  uint4 c3r, uint4 c3g, uint4 c3b
) {
  short x32 = x3 - x2;
  unsigned short adx = abs(x32);
  short sdx = sgn(x32, 1);
  short crdx = c3r - c2r;
  short cgdx = c3g - c2g;
  short cbdx = c3b - c2b;
  unsigned short acrdx = abs(crdx);
  short scrdx = sgn(crdx, 1);
  unsigned short acgdx = abs(cgdx);
  short scgdx = sgn(cgdx, 1);
  unsigned short acbdx = abs(cbdx);
  short scbdx = sgn(cbdx, 1);
  int4 dcr = (scrdx == sdx) ? sdx : -sdx;
  int4 dcg = (scgdx == sdx) ? sdx : -sdx;
  int4 dcb = (scbdx == sdx) ? sdx : -sdx;
  unsigned short delta_r = 0;
  unsigned short delta_g = 0;
  unsigned short delta_b = 0;
  short x = x2;
  uint4 r = c2r;
  uint4 g = c2g;
  uint4 b = c3b;
  unsigned short pos;

  if (adx == 0) {
	// for single point
	if (x >= 0 && x < SCREEN_WIDTH) {
		vram[y * SCREEN_WIDTH + x] = COLOR(r, g, b);
	}
	return;
  }

  loop_x: for (pos = 0; pos <= adx; x += sdx, ++pos) {
    // draw operation
    if (x >= 0 && x < SCREEN_WIDTH) {
      vram[y * SCREEN_WIDTH + x] = COLOR(r, g, b);
    }

    // update modifiers
    delta_r += acrdx;
    loop_r: while (delta_r >= adx) {
      r += dcr;
      delta_r -= adx;
    }
    delta_g += acgdx;
    loop_g: while (delta_g >= adx) {
      g += dcg;
      delta_g -= adx;
    }
    delta_b += acbdx;
    loop_b: while (delta_b >= adx) {
      b += dcb;
      delta_b -= adx;
    }
  }
}

// top function for the FPGA logic sythesis
// move Y direction and draw X direction
// assume VGA 4bit depth color
// assume Y axis of triangle is the same for P2 and P3
void drawFlatTriangle(
  volatile unsigned short *vram,
  short x1, short y1,
  short x2, short y2,
  short x3,
  unsigned short c1, unsigned short c2, unsigned short c3
) {
  short y12 = y1 - y2;
  short ady = abs(y12);
  short sdy = sgn(y12, 1);
  short x12 = x1 - x2;
  short ada = abs(x12);
  short sda = sgn(x12, 1);
  short x13 = x1 - x3;
  short adb = abs(x13);
  short sdb = sgn(x13, 1);
  short crda = VGACOLOR(c1, 8) - VGACOLOR(c2, 8);
  unsigned short adcrda = abs(crda);
  short sdcrda = sgn(crda, 1);
  short cgda = VGACOLOR(c1, 4) - VGACOLOR(c2, 4);
  unsigned short adcgda = abs(cgda);
  short sdcgda = sgn(cgda, 1);
  short cbda = VGACOLOR(c1, 0) - VGACOLOR(c2, 0);
  unsigned short adcbda = abs(cbda);
  short sdcbda = sgn(cbda, 1);
  short crdb = VGACOLOR(c1, 8) - VGACOLOR(c3, 8);
  unsigned short adcrdb = abs(crdb);
  short sdcrdb = sgn(crdb, 1);
  short cgdb = VGACOLOR(c1, 4) - VGACOLOR(c3, 4);
  unsigned short adcgdb = abs(cgdb);
  short sdcgdb = sgn(cgdb, 1);
  short cbdb = VGACOLOR(c1, 0) - VGACOLOR(c3, 0);
  unsigned short adcbdb = abs(cbdb);
  short sdcbdb = sgn(cbdb, 1);
  short dxa = (sda == sdy) ? sdy : -sdy;
  short dxb = (sdb == sdy) ? sdy : -sdy;
  int4 dcra = (sdcrda == sdy) ? sdy : -sdy;
  int4 dcga = (sdcgda == sdy) ? sdy : -sdy;
  int4 dcba = (sdcbda == sdy) ? sdy : -sdy;
  int4 dcrb = (sdcrdb == sdy) ? sdy : -sdy;
  int4 dcgb = (sdcgdb == sdy) ? sdy : -sdy;
  int4 dcbb = (sdcbdb == sdy) ? sdy : -sdy;
  short delta_xa = 0;
  short delta_xb = 0;
  unsigned short delta_cra = 0;
  unsigned short delta_cga = 0;
  unsigned short delta_cba = 0;
  unsigned short delta_crb = 0;
  unsigned short delta_cgb = 0;
  unsigned short delta_cbb = 0;
  short y = y2;
  short xa = x2;
  short xb = x3;
  uint4 cra = VGACOLOR(c2, 8);
  uint4 cga = VGACOLOR(c2, 4);
  uint4 cba = VGACOLOR(c2, 0);
  uint4 crb = VGACOLOR(c3, 8);
  uint4 cgb = VGACOLOR(c3, 4);
  uint4 cbb = VGACOLOR(c3, 0);
  unsigned short pos;
 
  for (pos = 0; pos <= ady; y += sdy, ++pos) {
    // draw X direction
	if (y >= 0 && y < SCREEN_HEIGHT) {
      drawXAxis(vram, y, xa, xb, cra, cga, cba, crb, cgb, cbb);
    }

    // update modifiers
    delta_xa += ada;
    while (delta_xa >= ady) {
      xa += dxa;
      delta_xa -= ady;
    }
    delta_xb += adb;
    while (delta_xb >= ady) {
      xb += dxb;
      delta_xb -= ady;
    }
    delta_cra += adcrda;
    while (delta_cra >= ady) {
      cra += dcra;
      delta_cra -= ady;
    }
    delta_cga += adcgda;
    while (delta_cga >= ady) {
      cga += dcga;
      delta_cga -= ady;
    }
    delta_cba += adcbda;
    while (delta_cba >= ady) {
      cba += dcba;
      delta_cba -= ady;
    }
    delta_crb += adcrdb;
    while (delta_crb >= ady) {
      crb += dcrb;
      delta_crb -= ady;
    }
    delta_cgb += adcgdb;
    while (delta_cgb >= ady) {
      cgb += dcgb;
      delta_cgb -= ady;
    }
    delta_cbb += adcbda;
    while (delta_cbb >= ady) {
      cbb += dcbb;
      delta_cbb -= ady;
    }
  }
}


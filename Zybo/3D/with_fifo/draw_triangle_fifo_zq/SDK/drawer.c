/*
 * drawer.c
 *
 *  Created on: 2017/04/11
 *      Author: minoru
 */
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "xparameters.h"
#include "xgpio.h"
#include "xil_cache.h"
#include "xdrawflattriangle.h"
#include "drawer.h"


// draw_fifo stuff
#define DRAWFIFO_BASE	(volatile unsigned int *)(XPAR_DRAW_FIFO_V1_0_0_BASEADDR)
#define DRAWFIFO_RSTN	(volatile unsigned int *)(XPAR_DRAW_FIFO_V1_0_0_BASEADDR + 4)
#define DRAWFIFO_X1Y1	(volatile unsigned int *)(XPAR_DRAW_FIFO_V1_0_0_BASEADDR + 8)
#define DRAWFIFO_FULL	0x2
#define DRAWFIFO_NEXT	0x1

// GPIO ports
#define GPIO_DISPADDR	(1)
#define GPIO_DISPON		(2)
#define GPIO_VBLANK		(1)
#define GPIO_CLRVBLANK	(2)

// screen parameters or definitions
#define SCREEN_WIDTH	(640)
#define SCREEN_HEIGHT	(480)
#define VRAM_START		(0x10000000)
#define VGACOLOR(x, p)	(((x) >> p) & 0xf)
#define COLOR(r, g, b)	(((r) << 8) | ((g) << 4) | (b))
#define INTERPOLATE(v1, v3)	(float)(y2 - y3) * (v1) / (float)(y1 - y3) + (float)(y2 - y1) * (v3) / (float)(y3 - y1)
#define VRAM0			(0)
#define VRAM1			(1 << 20)
#define min(x,y)		(((x) < (y)) ? (x) : (y))
#define swap(x,y)		{ short tmp = x; x = y; y = tmp; }

// globals
unsigned short *gVram[2];
int gCurrentFlip;
int gEngineStart = 0;
float gViewPointZ = 240.0f;
float gViewMag = 240.0f;
float gViewTransX = 320.0f;
float gViewTransY = 240.0f;
float *gWorkVertex;
XGpio gpioDisp, gpioVblank;
XDrawflattriangle gFTR;

/**
 * initialize the system
 */
int initDrawer() {
	// initialize GPIOs
	if (XGpio_Initialize(&gpioDisp, XPAR_AXI_GPIO_0_DEVICE_ID) != XST_SUCCESS) {
		return XST_FAILURE;
	}
	if (XGpio_Initialize(&gpioVblank, XPAR_AXI_GPIO_1_DEVICE_ID) != XST_SUCCESS) {
		return XST_FAILURE;
	}
	XGpio_SetDataDirection(&gpioDisp, GPIO_DISPADDR, 0);
	XGpio_SetDataDirection(&gpioDisp, GPIO_DISPON, 0);
	XGpio_SetDataDirection(&gpioVblank, GPIO_VBLANK, 1);
	XGpio_SetDataDirection(&gpioVblank, GPIO_CLRVBLANK, 0);

	// release reset signal for FIFO
	*DRAWFIFO_RSTN = 0x80000000;
	// wait a moment
	for (int i = 0; i < 1000; ++i);

	// initialize flat triangle drawer
	if (XDrawflattriangle_Initialize(&gFTR, XPAR_DRAWFLATTRIANGLE_0_DEVICE_ID) != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// initialize VRAM address
	gVram[0] = (unsigned short *)(VRAM_START + VRAM0);
	gVram[1] = (unsigned short *)(VRAM_START + VRAM1);
	gCurrentFlip = 0;
	XGpio_DiscreteWrite(&gpioDisp, GPIO_DISPADDR, (u32)gVram[gCurrentFlip]);
	XDrawflattriangle_Set_vram(&gFTR, (u32)gVram[1 - gCurrentFlip]);
	return XST_SUCCESS;
}

/**
 * start display
 */
void startDisplay() {
	XGpio_DiscreteWrite(&gpioDisp, GPIO_DISPON, 1);
}

/**
 * stop display
 */
void stopDisplay() {
	XGpio_DiscreteWrite(&gpioDisp, GPIO_DISPON, 0);
}

/**
 * swap display VRAM and drawing VRAM
 */
void swapDisplay() {
	gCurrentFlip = 1 - gCurrentFlip;
	XGpio_DiscreteWrite(&gpioDisp, GPIO_DISPADDR, (u32)gVram[gCurrentFlip]);
	XDrawflattriangle_Set_vram(&gFTR, (u32)gVram[1 - gCurrentFlip]);
}

/**
 * clear the drawing screen
 */
void clearDisplay() {
	volatile unsigned short *v = gVram[1 - gCurrentFlip];
	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i, ++v) {
		*v = 0xfff;
	}
	Xil_DCacheFlush();
}

/**
 * wait until vertical blank period
 */
void waitVblank() {
	XGpio_DiscreteWrite(&gpioVblank, GPIO_CLRVBLANK, 1);
	XGpio_DiscreteWrite(&gpioVblank, GPIO_CLRVBLANK, 0);
	while (XGpio_DiscreteRead(&gpioVblank, GPIO_VBLANK) == 0);
}

/**
 * set Z axis position of the view point
 * view pyramid is fixed to (-320, -240) - (320, 240)
 */
void setViewPointZ(float z) {
	gViewPointZ = z;
}

/**
 * rotate a single polygons by angle theta, phi
 */
Polygons *rotatePolygons(Polygons *dst, Polygons *src, float theta, float phi) {
	float *srcV = src->vertexes, *dstV = dst->vertexes;
	for (int i = 0; i < src->nVertex; ++i, srcV += 3) {
		float s[3] = { srcV[0], srcV[1], srcV[2] };
		float d[3];
		float cphi = cosf(phi);
		float sphi = sinf(phi);
		float ctheta = cosf(theta);
		float stheta = sinf(theta);

		// rotate on x-z plane
		d[0] = s[0] * cphi + s[2] * sphi;
		d[1] = s[1];
		d[2] = -s[0] * sphi + s[2] * cphi;

		// rotate on x-y plane
		s[0] = d[0] * ctheta + d[1] * stheta;
		s[1] = -d[0] * stheta + d[1] * ctheta;
		s[2] = d[2];

		// save result
		*(dstV++) = s[0];
		*(dstV++) = s[1];
		*(dstV++) = s[2];
	}
	memcpy(dst->surfaces, src->surfaces, sizeof(Surface) * dst->nSurface);
	return dst;
}

/**
 * calculate quantity of vertex buffers
 */
int calcTmpVertexQuant(Polygons *src, int nPolygons) {
	int q = 0;
	for (int i = 0; i < nPolygons; ++i) {
		q += src->nVertex;
	}
	return q;
}

/**
 * calculate quantity of surface buffers
 */
int calcTmpSurfaceQuant(Polygons *src, int nPolygons) {
	int q = 0;
	for (int i = 0; i < nPolygons; ++i) {
		q += src->nSurface;
	}
	return q;
}

/**
 * calculate most farthest vertex from the view point
 */
static float farZ(float *v, const int s[3]) {
	float z1 = v[s[0] * 3 + 2];
	z1 = min(z1, v[s[1] * 3 + 2]);
	z1 = min(z1, v[s[2] * 3 + 2]);
	return z1;
}

/**
 * compare function for z sort
 */
static int compareZ(const void *x1, const void *x2) {
	const Surface *s1 = (const Surface *)x1;
	const Surface *s2 = (const Surface *)x2;
	float z = farZ(gWorkVertex, s2->v) - farZ(gWorkVertex, s1->v);
	return (z > 0.0f) ? -1 : (z < 0.0f) ? 1 : 0;
}

/**
 * wait until flat triangle drawer finishes
 */
static void waitXDrawflattriangle() {
	if (gEngineStart) {
		while ((*DRAWFIFO_BASE & DRAWFIFO_FULL) == DRAWFIFO_FULL);
		gEngineStart = 0;
	}
}

#define SHIFT(x, a)		((((unsigned int)x) & 0xffff) << a)
#define PACK(x, y)		(SHIFT(x, 16) | SHIFT(y, 0))

/**
 * draw a flat triangle
 */
static void drawFlatTriangle(
		short x1, short y1,
		short x2, short y2,
		short x3,
		unsigned short c1, unsigned short c2, unsigned short c3) {

	// set parameters
	unsigned int expected[4] = { PACK(y1, x1), PACK(y2, x2), PACK(c1, x3), PACK(c3, c2) };
	unsigned int state;
	static int count = 0;

	// wait previous drawing
	state = *DRAWFIFO_BASE & 7;
	if (state != 0 && state != 4) {
		xil_printf("illegal state(1)=%d\r\n", state);
	}
	waitXDrawflattriangle();
	*DRAWFIFO_BASE = expected[0];

	waitXDrawflattriangle();
	*DRAWFIFO_BASE = expected[1];

	waitXDrawflattriangle();
	*DRAWFIFO_BASE = expected[2];

	waitXDrawflattriangle();
	*DRAWFIFO_BASE = expected[3];
	state = *DRAWFIFO_BASE & 7;
	if (state != 4 && state != 0) {
		xil_printf("illegal state(2)=%d\r\n", state);
	}
	if (++count % 8 == 0) {
		xil_printf("count=%d\r\n", count);
	}
	// start the draw engine
	gEngineStart = 1;
}

/**
 * draw a general trianlge
 */
static void drawTriangle(
		short x1, short y1,
		short x2, short y2,
		short x3, short y3,
		unsigned short c1, unsigned short c2, unsigned short c3) {
	// sort by y axis
	if (y1 > y2) {
		swap(x1, x2);
		swap(y1, y2);
		swap(c1, c2);
	}
	if (y1 > y3) {
		swap(x1, x3);
		swap(y1, y3);
		swap(c1, c3);
	}
	if (y2 > y3) {
		swap(x2, x3);
		swap(y2, y3);
		swap(c2, c3);
	}

	// exclude eccentric cases
	if (y1 == y3) {
		return;
	}
	if (x1 == x2 && x2 == x3) {
		return;
	}

	if (y1 == y2) {
		drawFlatTriangle(x3, y3, x1, y1, x2, c3, c1, c2);
	} else if (y2 == y3) {
		drawFlatTriangle(x1, y1, x2, y2, x3, c1, c2, c3);
	} else {
		short xa = INTERPOLATE(x1, x3);
		unsigned short r1 = VGACOLOR(c1, 8);
		unsigned short g1 = VGACOLOR(c1, 4);
		unsigned short b1 = VGACOLOR(c1, 0);
		unsigned short r3 = VGACOLOR(c3, 8);
		unsigned short g3 = VGACOLOR(c3, 4);
		unsigned short b3 = VGACOLOR(c3, 0);
		unsigned short ra = INTERPOLATE(r1, r3);
		unsigned short ga = INTERPOLATE(g1, g3);
		unsigned short ba = INTERPOLATE(b1, b3);
		unsigned short ca = COLOR(ra, ga, ba);
		drawFlatTriangle(x1, y1, xa, y2, x2, c1, ca, c2);
		drawFlatTriangle(x3, y3, x2, y2, xa, c3, c2, ca);
	}
}

/**
 * draw a set of polygons on the VGA display
 */
void drawPolygons(Polygons *src, int nPolygons, Polygons *work1, float *work2) {
	int w;
	Surface *dstSurface;
	float *v1, *v2;

	// check work area is enough
	if (calcTmpVertexQuant(src, nPolygons) != work1->nVertex) {
		return;
	}
	if (calcTmpSurfaceQuant(src, nPolygons) != work1->nSurface) {
		return;
	}

	// copy a set of polygons into the flat array
	dstSurface = work1->surfaces;
	w = 0;
	for (int i = 0; i < nPolygons; ++i) {
		Surface *srcSurface = src[i].surfaces;
		memcpy(work1->vertexes, src[i].vertexes, sizeof(float) * src[i].nVertex * 3);
		for (int j = 0; j < src[i].nSurface; srcSurface++, dstSurface++, j++) {
			dstSurface->v[0] = srcSurface->v[0] + w;
			dstSurface->v[1] = srcSurface->v[1] + w;
			dstSurface->v[2] = srcSurface->v[2] + w;
			dstSurface->c[0] = srcSurface->c[0];
			dstSurface->c[1] = srcSurface->c[1];
			dstSurface->c[2] = srcSurface->c[2];
		}
		w += src[i].nSurface;
	}

	// sort surfaces from -z to z
	gWorkVertex = work1->vertexes;
	qsort(work1->surfaces, work1->nSurface, sizeof(Surface), compareZ);

	// make a view transformation
	v1 = work1->vertexes;
	v2 = work2;
	for (int i = 0; i < work1->nVertex; ++i, v1 += 3, v2 += 3) {
		if (v1[2] < gViewPointZ - 1.0f) {
			v2[0] = v1[0] / (gViewPointZ - v1[2]) * gViewMag + gViewTransX;
			v2[1] = v1[1] / (gViewPointZ - v1[2]) * -gViewMag + gViewTransY;
		}
		v2[2] = gViewPointZ - v1[2];
	}

	// for each surface (from farthest to nearest)
	dstSurface = work1->surfaces;
	for (int i = 0; i < work1->nSurface; ++i, dstSurface++) {
		// z axis of the vertex must be in the view pyramid
		if (work2[dstSurface->v[0] * 3 + 2] < 0.0f
				|| work2[dstSurface->v[1] * 3 + 2] < 0.0f
				|| work2[dstSurface->v[2] * 3 + 2] < 0.0f) {
			continue;
		}

		// z axis of normal vector
		float vx1 = work1->vertexes[dstSurface->v[0] * 3 + 0];
		float vy1 = work1->vertexes[dstSurface->v[0] * 3 + 1];
		float vx2 = work1->vertexes[dstSurface->v[1] * 3 + 0];
		float vy2 = work1->vertexes[dstSurface->v[1] * 3 + 1];
		float vx3 = work1->vertexes[dstSurface->v[2] * 3 + 0];
		float vy3 = work1->vertexes[dstSurface->v[2] * 3 + 1];
		vx2 = vx2 - vx1;
		vy2 = vy2 - vy1;
		vx3 = vx3 - vx1;
		vy3 = vy3 - vy1;
		if (vx2 * vy3 - vx3 * vy2 <= 0.0f) {
			// surface is reversed
			continue;
		}

		// draw a triangle
		vx1 = work2[dstSurface->v[0] * 3];
		vy1 = work2[dstSurface->v[0] * 3 + 1];
		vx2 = work2[dstSurface->v[1] * 3];
		vy2 = work2[dstSurface->v[1] * 3 + 1];
		vx3 = work2[dstSurface->v[2] * 3];
		vy3 = work2[dstSurface->v[2] * 3 + 1];
		drawTriangle(vx1, vy1, vx2, vy2, vx3, vy3,
				dstSurface->c[0], dstSurface->c[1], dstSurface->c[2]);
	}

	// wait until the engine stops
	while ((*DRAWFIFO_BASE & DRAWFIFO_NEXT) == 0);
}

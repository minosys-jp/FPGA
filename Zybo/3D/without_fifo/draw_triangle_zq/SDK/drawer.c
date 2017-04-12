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
#include "xdrawflattriangle.h"
#include "drawer.h"

#define GPIO_DISPADDR	(1)
#define GPIO_DISPON		(2)
#define GPIO_VBLANK		(1)
#define GPIO_CLRVBLANK	(2)
#define SCREEN_WIDTH	(640)
#define SCREEN_HEIGHT	(480)
#define VRAM_START		(0x10000000)
#define VRAM0			(0)
#define VRAM1			(1 << 20)
#define min(x,y)		(((x) < (y)) ? (x) : (y))
#define swap(x,y)		{ short tmp = x; x = y; y = tmp; }

unsigned short *gVram[2];
int gCurrentFlip;
float gViewPointZ = 240.0f;
float gViewMag = -240.0f;
float gViewTransX = 320.f;
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

	// initialize flat triangle drawer
	if (XDrawflattriangle_Initialize(&gFTR, XPAR_DRAWFLATTRIANGLE_0_DEVICE_ID) != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// initialize VRAM address
	gVram[0] = (unsigned short *)(VRAM_START + VRAM0);
	gVram[1] = (unsigned short *)(VRAM_START + VRAM1);
	gCurrentFlip = 0;
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
	unsigned short *v = gVram[1 - gCurrentFlip];
	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i, ++v) {
		*v = 0;
	}
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
	for (int i = 0; i < src->nVertex; ++i) {
		float s[3] = { *(srcV++), *(srcV++), *(srcV++) };
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
	return dst;
}

/**
 * calculate quantity of vertex buffers
 */
int calcTmpVertexQuant(Polygons *src, int nPolygons) {
	int q = 0;
	for (int i = 0; i < nPolygons; ++i) {
		q += src->nVertex * 3;
	}
	return q;
}

/**
 * calculate quantity of surface buffers
 */
int calcTmpSurfaceQuant(Polygons *src, int nPolygons) {
	int q = 0;
	for (int i = 0; i < nPolygons; ++i) {
		q += src->nSurface * 3;
	}
	return q;
}

/**
 * calculate most farthest vertex from the view point
 */
static float farZ(float *v, const int *s) {
	float z1 = v[s[0] * 3 + 2];
	z1 = min(z1, v[s[1] * 3 + 2]);
	z1 = min(z1, v[s[2] * 3 + 2]);
	return z1;
}

/**
 * compare function for z sort
 */
static int compareZ(const void *x1, const void *x2) {
	const int *s1 = (const int *)x1;
	const int *s2 = (const int *)x2;
	float z = farZ(gWorkVertex, s2) - farZ(gWorkVertex, s1);
	return (z > 0.0f) ? 1 : (z < 0.0f) ? -1 : 0;
}

/**
 * wait until flat triangle drawer finishes
 */
static void waitXDrawflattriangle() {
	while (XDrawflattriangle_IsDone(&gFTR) == 0);
}

/**
 * draw a flat triangle
 */
static void drawFlatTriangle(
		short x1, short y1,
		short x2, short y2,
		short x3,
		unsigned short c1, unsigned short c2, unsigned short c3) {
	// wait previous drawing
	waitXDrawflattriangle();

	// set parameters
	XDrawflattriangle_Set_x1(&gFTR, x1);
	XDrawflattriangle_Set_y1(&gFTR, y1);
	XDrawflattriangle_Set_x2(&gFTR, x2);
	XDrawflattriangle_Set_y2(&gFTR, y2);
	XDrawflattriangle_Set_x3(&gFTR, x3);
	XDrawflattriangle_Set_c1(&gFTR, c1);
	XDrawflattriangle_Set_c2(&gFTR, c2);
	XDrawflattriangle_Set_c3(&gFTR, c3);

	// start the draw engine
	XDrawflattriangle_Start(&gFTR);
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
		short xa = (float)(y2 - y3) * x1 / (float)(y1 - y3) + (float)(y2 - y1) * x2 / (float)(y3 - y1);
		unsigned short ca = (float)(y2 - y3) * c1 / (float)(y1 - y3) + (float)(y2 - y1) * c2 / (float)(y3 - y1);
		drawFlatTriangle(x1, y1, xa, y2, x2, c1, ca, c2);
		drawFlatTriangle(x3, y3, x2, y2, xa, c3, c2, ca);
	}
}

/**
 * draw a set of polygons on the VGA display
 */
void drawPolygons(Polygons *src, int nPolygons, Polygons *work1, float *work2) {
	int w;
	int *dstSurface;
	float *v1, *v2;
	unsigned short *color;

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
		int *srcSurface = src[i].surfaces;
		memcpy(work1->vertexes, src[i].vertexes, sizeof(float) * src[i].nVertex * 3);
		memcpy(work1->colors, src[i].colors, sizeof(unsigned short) * src[i].nSurface * 3);
		for (int j = 0; j < src[i].nSurface; ++i) {
			*(dstSurface++) = *(srcSurface) + w;
		}
		w += src[i].nSurface;
	}

	// sort surfaces from -z to z
	gWorkVertex = work1->vertexes;
	qsort(work1->surfaces, work1->nSurface, sizeof(int) * 3, compareZ);

	// make a view transformation
	v1 = work1->vertexes;
	v2 = work2;
	for (int i = 0; i < work1->nVertex; ++i, v1 += 3, v2 += 3) {
		if (v1[2] < gViewPointZ - 1.0f) {
			v2[0] = v1[0] / (gViewPointZ - v1[2]) * gViewMag + gViewTransX;
			v2[1] = v1[1] / (gViewPointZ - v1[2]) * gViewMag + gViewTransY;
		}
		v2[2] = v1[2] - gViewPointZ;
	}

	// for each surface (from farthest to nearest)
	dstSurface = work1->surfaces;
	color = work1->colors;
	for (int i = 0; i < work1->nSurface; ++i, dstSurface += 3, color += 3) {
		// z axis of the vertex must be in the view pyramid
		if (work2[dstSurface[0] * 3 + 2] < 0.0f
				|| work2[dstSurface[1] * 3 + 2] < 0.0f
				|| work2[dstSurface[2] * 3 + 2] < 0.0f) {
			continue;
		}

		// z axis of normal vector
		float vx1 = work1->vertexes[dstSurface[0] * 3];
		float vy1 = work1->vertexes[dstSurface[0] * 3 + 1];
		float vx2 = work1->vertexes[dstSurface[1] * 3];
		float vy2 = work1->vertexes[dstSurface[1] * 3 + 1];
		float vx3 = work1->vertexes[dstSurface[2] * 3];
		float vy3 = work1->vertexes[dstSurface[2] * 3 + 1];
		vx2 = vx2 - vx1;
		vy2 = vy2 - vy1;
		vx3 = vx3 - vx1;
		vy3 = vy3 - vy1;
		if (vx2 * vy3 - vx3 * vy2 <= 0.0f) {
			// surface is reversed
			continue;
		}

		// draw a triangle
		vx1 = work2[dstSurface[0] * 3];
		vy1 = work2[dstSurface[0] * 3 + 1];
		vx2 = work2[dstSurface[1] * 3];
		vy2 = work2[dstSurface[1] * 3 + 1];
		vx3 = work2[dstSurface[2] * 3];
		vy3 = work2[dstSurface[2] * 3 + 1];
		drawTriangle(vx1, vy1, vx2, vy2, vx3, vy3, color[0], color[1], color[2]);
	}

	waitXDrawflattriangle();
}

/*
 * draw_test.c
 *
 *  Created on: 2017/04/11
 *      Author: minoru
 */

#include <math.h>
#include "drawer.h"
#include "xil_printf.h"
#include "xstatus.h"

float vertex[] = { -50.0f, -50.0f, 50.0f,	// 0
		50.0f, -50.0f, 50.0f,				// 1
		50.0f, 50.0f, 50.0f,				// 2
		-50.0f, 50.0f, 50.0f,				// 3
		-50.0f, -50.0f, -50.0f,			// 4
		50.0f, -50.f, -50.f,				// 5
		50.0f, 50.0f, -50.f,				// 6
		-50.0f, 50.0f, -50.0f			// 7
};
Surface surface[] = {
		{ {0, 1, 3}, {0x000, 0xf00, 0x000} },
		{ {1, 2, 3}, {0xf00, 0x000, 0x000} },
		{ {4, 6, 5}, {0x000, 0x000, 0x0f0} },
		{ {4, 7, 6}, {0x000, 0x0f0, 0x000} },
		{ {0, 4, 1}, {0x000, 0x000, 0x00f} },
		{ {4, 5, 1}, {0x000, 0x00f, 0x000} },
		{ {3 ,2, 7}, {0xf0f, 0x000, 0x000} },
		{ {2, 6, 7}, {0x000, 0x000, 0xf0f} },
		{ {0, 3, 4}, {0x000, 0x0ff, 0x000} },
		{ {3, 7, 4}, {0x0ff, 0x000, 0x000} },
		{ {1, 5, 6}, {0x000, 0xff0, 0x000} },
		{ {1, 6, 2}, {0x000, 0x000, 0xff0} }
};

#define NVERTEX		(sizeof(vertex) / (sizeof(vertex[0]) * 3))
#define NSURFACE	(sizeof(surface) / (sizeof(surface[0])))

Polygons poly[] = {
		{ NVERTEX, NSURFACE, &vertex[0], &surface[0] }
};

float vwork1[NVERTEX * 3], vwork2[NSURFACE * 3];
Surface swork1[NSURFACE];
Polygons work1[] = {
		{ NVERTEX, NSURFACE, &vwork1[0], &swork1[0] }
};

float vrot[NVERTEX * 3];
Surface srot[NSURFACE];
Polygons rot[] = {
		{ NVERTEX, NSURFACE, &vrot[0], &srot[0] }
};

void waitMoment() {
	waitVblank();
	waitVblank();
	waitVblank();
	waitVblank();
	waitVblank();
}

int main() {
	float theta = 0.0f, phi = 0.0f * M_PI / 180.0f;
	float dtheta = 1.0f * M_PI / 180.0f;
	float dphi = 2.0f * M_PI / 180.0f;

	// initialize the system
	if (initDrawer() != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// start display
	startDisplay();
	clearDisplay();
	swapDisplay();

	for (int i = 0; i < 360; i += 2) {
		// clear the draw screen
		clearDisplay();

		// rotate the polygons
		rotatePolygons(&rot[0], &poly[0], theta, phi);

		// draw the polygons
		drawPolygons(&rot[0], 1, &work1[0], &vwork2[0]);

		// wait until VBLANK
		waitVblank();

		// swap the screen buffers
		swapDisplay();
		if (i % 10 == 0) {
			xil_printf("theta=%d, phi=%d\r\n", (int)(theta * 180.0f / M_PI), (int)(phi * 180.0f / M_PI));
		}

		// wait a moment
		waitMoment();

		// update the rotation parameters
		theta += dtheta;
		phi += dphi;
	}

	// stop display
	stopDisplay();
	return 0;
}

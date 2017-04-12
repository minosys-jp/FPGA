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

float vertex[] = { -100.0f, -100.0f, 0.0f, 100.0f, -100.0f, 0.0f, 100.0f, 100.0f, 0.0f, -100.0f, 100.0f, 0.0f };
int surface[] = { 0, 1, 3, 1, 2, 3 };
unsigned short color[] = { 0xf00, 0x0f0, 0x00f, 0x0f0, 0x0ff, 0x00f };

Polygons poly[] = {
		{ 4, 2, &vertex[0], &surface[0], &color[0] }
};

float vwork1[4 * 3], vwork2[4 * 3];
int swork1[2 * 3];
unsigned short cwork1[2 * 3];
Polygons work1[] = {
		{ 4, 2, &vwork1[0], &swork1[0], &cwork1[0] }
};

float vrot[4 * 3];
int srot[2 * 3];
unsigned short crot[2 * 3];
Polygons rot[] = {
		{ 4, 2, &vrot[0], &srot[0], &crot[0] }
};

void waitMoment() {
	waitVblank();
	waitVblank();
	waitVblank();
	waitVblank();
	waitVblank();
}

int main() {
	float theta = 0.0f, phi = 0.0f;
	float dtheta = 1.0f * M_PI / 180.0f;
	float dphi = 0.0f;

	// initialize the system
	if (initDrawer() != XST_SUCCESS) {
		return XST_FAILURE;
	}

	for (int i = 0; i < 360; ++i) {
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
	return 0;
}

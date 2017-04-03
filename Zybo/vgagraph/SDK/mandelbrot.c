/*
 * mandelbrot.c
 *
 *  Created on: 2017/04/02
 *      Author: minoru
 */

#include <stdio.h>
#include <math.h>

static int isDiverge(float x, float y) {
	return x * x + y * y > 2.0f * 2.0f;
}

static int divergeCount(float cx, float cy, int repeat) {
	float x = 0.0f, y = 0.0f;
	int count;

	for (count = 0; count < repeat; ++count) {
		float zx = x * x - y * y + cx;
		float zy = 2.0f * x * y + cy;
		if (isDiverge(zx, zy)) {
			return count;
		}
		x = zx;
		y = zy;
	}
	return count;
}

static unsigned short count2color(int count, int repeat) {
	if (count == repeat) {
		return 0;
	} else {
		unsigned char r = 15.0f * count / repeat;
		unsigned char g = 15.0f * count / repeat * count / repeat;
		unsigned char b = (count > repeat / 2) ? (15.0f * (count - repeat / 2) / repeat) : 15.0f * 2 * count / repeat;
		return (r << 8) | (g << 4) | b;
	}
}

// マンデルブロ集合の描画
void drawMandelbrot(volatile unsigned short *vram, int w, int h, int repeat) {
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			float cx = (float)x / (w / 3.0f) - 2.0f;
			float cy = (float)y / (h / 2.0f) - 1.0f;
			int count = divergeCount(cx, cy, repeat);
			vram[x + y * w] = count2color(count, repeat);
		}
	}
}

/*
 * vgagraph_test.c
 *
 *  Created on: 2017/04/02
 *      Author: minoru
 */

#include <stdio.h>
#include <string.h>
#include "xparameters.h"
#include "xgpio.h"
#include "xil_cache.h"

// GPIO チャンネル
#define GPIO_DISPADDR	1
#define GPIO_DISPON		2
#define GPIO_VBLANK		1
#define GPIO_CLRVBLANK	2

#define WIDTH	640
#define HEIGHT	480
#define VRAMDELTA	(0x00100000)
#define VRAM0	((volatile unsigned short *)0x10000000)
#define VRAM1	((volatile unsigned short *)(0x10000000 + VRAMDELTA))

void drawMandelbrot(volatile unsigned short *, int, int, int);
void drawFillbox(volatile unsigned short *vram, int xpos, int ypos, int width, int height, unsigned short color);

// GPIO インスタンス
XGpio GpioAddr, GpioBlank;

// 次の VBLANK まで待機する関数
static void waitVblank() {
	XGpio_DiscreteWrite(&GpioBlank, GPIO_CLRVBLANK, 1);
	XGpio_DiscreteWrite(&GpioBlank, GPIO_CLRVBLANK, 0);
	while (XGpio_DiscreteRead(&GpioBlank, GPIO_VBLANK) == 0);
}

// 塗りつぶし長方形を描画する
#if 0
static void drawFilledRectangle(int xstart, int ystart, int w, int h, unsigned short color) {
	for (int j = 0; j < h; ++j) {
		int offset = (ystart + j) * WIDTH + xstart;
		for (int i = 0; i < w; ++i, ++offset) {
			VRAM0[offset] = color;
		}
	}
}
#else
static void drawFilledRectangle(int xstart, int ystart, int w, int h, unsigned short color) {
	drawFillbox(VRAM0, xstart, ystart, w, h, color);
}
#endif

// デバイスの初期化
static int init_device() {
	int status;

	// GPIO_0 の初期化
	status = XGpio_Initialize(&GpioAddr, XPAR_AXI_GPIO_0_DEVICE_ID);
	if (status != XST_SUCCESS) return status;
	XGpio_SetDataDirection(&GpioAddr, GPIO_DISPADDR, 0);
	XGpio_SetDataDirection(&GpioAddr, GPIO_DISPON, 0);

	// GPIO_1 の初期化
	status = XGpio_Initialize(&GpioBlank, XPAR_AXI_GPIO_1_DEVICE_ID);
	if (status != XST_SUCCESS) return status;
	XGpio_SetDataDirection(&GpioBlank, GPIO_VBLANK, 1);
	XGpio_SetDataDirection(&GpioBlank, GPIO_CLRVBLANK, 0);
	return XST_SUCCESS;
}

// 画面クリア
static void clearScreen() {
	drawFilledRectangle(0, 0, WIDTH, HEIGHT, 0);
}

const static unsigned short col[] = { 0x000, 0x00f, 0x0f0, 0xf00, 0x0ff, 0xf0f, 0xff0, 0xfff };

int main() {
	if (init_device() != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// VRAM ウィンドウ設定
	XGpio_DiscreteWrite(&GpioAddr, GPIO_DISPADDR, 0);	// 0x10000000 が設定される
	waitVblank();
	XGpio_DiscreteWrite(&GpioAddr, GPIO_DISPON, 1);		// 表示開始

	// VRAM テスト
	clearScreen();
	drawFilledRectangle(0, 0, WIDTH, HEIGHT, 0xff0);
	for (int i = 0; i < 10; ++i) {
		int x = i * 64;
		drawFilledRectangle(x, 0, 64, 16, col[i % 8]);
	}
	// 整列していないアドレス
	drawFilledRectangle(65, 16, 2, 8, 0x00f);
	drawFilledRectangle(65, 24, 4, 8, 0x0f0);
	drawFilledRectangle(65, 32, 6, 8, 0xf00);
	drawFilledRectangle(65, 40, 10, 8, 0x000);
	drawFilledRectangle(65, 48, 18, 8, 0x0ff);
	drawFilledRectangle(65, 56, 34, 8, 0xf0f);
	drawFilledRectangle(1, 100, 64, 4, 0xfff);
	drawFilledRectangle(63, 110, 64, 4, 0xf00);

	Xil_DCacheFlush();

	// マンデルブロ集合の定義
	printf("start to compute Mandelbrot set...\r\n");
	drawMandelbrot(VRAM1, WIDTH, HEIGHT, 20);
	Xil_DCacheFlush();
	printf("end to compute Mandelbrot set.!\r\n");
	XGpio_DiscreteWrite(&GpioAddr, GPIO_DISPADDR, (unsigned long int)VRAM1);	// 表示開始位置を移動する
	waitVblank();
	Xil_DCacheFlush();

	// 終了処理: 次のデバイスロードでハングアップしないようにする
	waitVblank();
	XGpio_DiscreteWrite(&GpioAddr, GPIO_DISPON, 0);		// 表示終了

	return 0;
}

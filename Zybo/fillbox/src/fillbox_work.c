/*
 * fillbox_work.c
 *
 *  Created on: 2017/04/09
 *      Author: minoru
 */

#include "xparameters.h"
#include "xil_printf.h"
#include <stdio.h>

#define REG_VRAM	((volatile unsigned long int *)XPAR_FILLBOX_V1_0_0_BASEADDR)
#define REG_WIDTH	((volatile int *)(XPAR_FILLBOX_V1_0_0_BASEADDR + 4))
#define REG_HEIGHT	((volatile int *)(XPAR_FILLBOX_V1_0_0_BASEADDR + 8))
#define REG_COLOR	((volatile unsigned short *)(XPAR_FILLBOX_V1_0_0_BASEADDR + 12))
#define TRIG_START	((volatile int *)(XPAR_FILLBOX_V1_0_0_BASEADDR + 16))
#define FLG_DONE	((volatile int *)(XPAR_FILLBOX_V1_0_0_BASEADDR + 16))

#define SCREEN_WIDTH	640

// 矩形描画関数
void drawFillbox(volatile unsigned short *vram, int xpos, int ypos, int width, int height, unsigned short color) {
	// 前回の終了まで待つ
	int flag, count = 0;
	while ((flag = *FLG_DONE) == 0) {
		if (count++ % 100 == 0) {
			xil_printf("vram=%x, w=%d, h=%d, c=%03x, flag=%x\r\n",
					*REG_VRAM, *REG_WIDTH, *REG_HEIGHT, *REG_COLOR, flag);
		}
	}

	// パラメータを設定
	*REG_VRAM = ((unsigned long int)vram) + ypos * SCREEN_WIDTH * 2 + xpos * 2;
	*REG_WIDTH = width;
	*REG_HEIGHT = height;
	*REG_COLOR = color;

	// 描画開始
	*TRIG_START = 1;

	// 終了まで待つ
	count = 0;
	while ((flag = *FLG_DONE) == 0) {
		if (count++ % 100 == 0) {
			xil_printf("vram=%x, w=%d, h=%d, c=%03x, flag=%x\r\n",
					*REG_VRAM, *REG_WIDTH, *REG_HEIGHT, *REG_COLOR, flag);
		}
	}
}

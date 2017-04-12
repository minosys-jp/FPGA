/*
 * drawer.h
 *
 *  Created on: 2017/04/11
 *      Author: minoru
 */

#ifndef SRC_DRAWER_H_
#define SRC_DRAWER_H_

typedef struct Polygons {
	int nVertex, nSurface;	// number of vertexes, number of surfaces
	float *vertexes;		// (n * 3: x, n * 3 + 1: y, n * 3 + 2: z)
	int *surfaces;			// (n * 3: vertex1, n * 3 + 1: vertex2, n * 3 + 2: vertex3)
	unsigned short *colors;	// (n * 3: color1, n * 3 + 1: color2, n * 3 + 2: color3)
} Polygons;

extern unsigned short *gVram[2];
extern int gCurrentFlip;

// prototypes
int initDrawer();
void startDisplay();
void stopDisplay();
void swapDisplay();
void waitVblank();
void clearDisplay();
Polygons *rotatePolygons(Polygons *dst, Polygons *src, float theta, float phi);
int calcTmpVertexQuant(Polygons *src, int nPolygons);
int calcTmpSurfaceQuant(Polygons *src, int nPolygons);
void setViewPointZ(float z);
void drawPolygons(Polygons *src, int nPolygons, Polygons *work1, float *work2);

#endif /* SRC_DRAWER_H_ */

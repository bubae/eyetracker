#ifndef _CORNER_H_
#define _CORNER_H_

#include <stdlib.h>
#include <math.h>

#include "Segment.h"

#define CORNER_SIGMA 2.0
#define CORNER_THRESHOLD 4.0
#define CORNER_DELTA 0.001
#define CORNER_MIN_ENTROPY 1.9
#define CORNER_ANGLE_THRESHOLD 160.0
#define CORNER_LINEFIT_THRESHOLD 0.25
#define CORNER_KERNEL_SIZE 3
#define CORNER_MIN_LENGTH 25

using namespace std;

int *getCorners(Line line, int *numOfCorners);
//double getEstimatedLineFit(Line line, int start, int end, int x);
//double leastSquareLineFit(Line line, int start, int end);
double getAngleAtCorner(Line line, int *candidateCornerIndexes, int index);
double getAngleBetweenVectors(GPixel p, GPixel c, GPixel n);
double getKernelValue(int index);
double gaussian(double mean, double stddev, double x);
double getDervatedGaussian(double mean, double stddev, double x);
int isTooShort(Line line);
Line **getSubArcs(Line line, int numOfCorner, int *cornerIndexes, int *count);

#endif

#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <math.h>

#define SEGMENT_NUM_DIRECTION 8
#define SEGMENT_MIN_ENTROPY 2.6
#define SEGMENT_MAX_DISTANCE 15
#define SEGMENT_MIN_CIRCULAR_LENGTH 150

using namespace cv;

typedef struct GPixel {
	int r;
	int c;
	double gradient;
} GPixel;

typedef struct Line {
	int length;
	GPixel *pixels;
} Line;

Line *makeLine(vector<Point> &segment);
Line *makeLine(Line line, int start, int end);
double getGradient(Line line, int index);
int isClosedLine(Line line);
int isClosedLine(Line line, int start, int end);
double getMaxEntropy(Line line, int *start, int *end);
double getEntropy(Line line);
double getEntropy(Line line, int start, int end);
int getDirection(double gradient);


#endif

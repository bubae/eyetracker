#ifndef _CANDIDATE_H_
#define _CANDIDATE_H_

#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <math.h>

#include "Segment.h"
#include "Ellipse.h"
#include "ImageProcess.h"

//#define CANDIDATE_MAX_COMBINATION 10
#define CANDIDATE_MIN_RADIUS 30
#define CANDIDATE_MAX_RADIUS 200
#define CANDIDATE_TEST 1

using namespace cv;

CvBox2D findPupil(IplImage* cvImg);
double averageRadius(CvBox2D ellipse);
double largeRadius(CvBox2D ellipse);
double scoreOfEllipse(Line *line, CvBox2D ellipse);
double getEpsilon(Line *line, CvBox2D ellipse);
double getTau(CvBox2D ellipse);
double getPhi(Line *line, CvBox2D ellipse);
double getCircumference(CvBox2D ellipse);

#endif

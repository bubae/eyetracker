#ifndef _IMAGEPROCESS_H_
#define _IMAGEPROCESS_H_

#include <cv.h>
#include <highgui.h>
#include <math.h>

void drawEllipses(IplImage *img, int numOfEllipses, CvBox2D ellipses[]);
void drawEllipse(CvBox2D box, IplImage *img);

#endif

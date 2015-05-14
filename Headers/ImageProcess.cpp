#include "ImageProcess.h"

void drawEllipse(CvBox2D box, IplImage *img) {
	CvPoint center;
        CvSize size;
        center = cvPointFrom32f(box.center);
        size.width = cvRound(box.size.width*0.5);
        size.height = cvRound(box.size.height*0.5);

	cvCircle(img, center, 1, CV_RGB(255,255,255));

        cvEllipse(img, center, size, box.angle, 0, 360, CV_RGB(255,255,255), 1, CV_AA, 0);
}

void drawEllipses(IplImage *img, int numOfEllipses, CvBox2D ellipses[]) {
	int i;

	for (i=0; i<numOfEllipses; i++) {
		drawEllipse(ellipses[i], img);
	}
}

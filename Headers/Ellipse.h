#ifndef _ELLIPSE_H_
#define _ELLIPSE_H_

//OpenCV Headers
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <cvblob.h>

#include "Segment.h"
#include "Corner.h"

#define ELLIPSE_BLOB_AREA_MIN 400
#define ELLIPSE_BLOB_AREA_MAX 3000

#define ELLIPSE_BLOB_THRESHOLD 70
#define ELLIPSE_MAX_DISTANCE 40
#define ELLIPSE_MAX_NUM 7

#define CANNY_MIN 100
#define CANNY_MAX 105
#define MIN_CONTOUR_LENGTH 30

#define ROI_SIZE 100

#define IMAGE_MODE 2

#define PI 3.141592653589

using namespace cvb;
using namespace cv;
using namespace std;

void findEllipsesFromImage(IplImage* cvImg, int *numOfEllipses, CvBox2D ellipses[ELLIPSE_MAX_NUM], Line *ellipseLines[ELLIPSE_MAX_NUM], CvPoint2D32f *centroid);
Line **findContourLines(IplImage *img, CvPoint2D32f centroid, int *numOfLines);
CvBox2D getEllipseFromLine(Line *line);
CvPoint2D32f centerOfLargestBlob(IplImage* src);
void filterByCircularity(CvBlobs& blobs);
void filterByDistance(CvBlobs &blobs);
int myContains(CvPoint2D32f centroid, CvBox2D ellipse);
double myDistance(CvPoint c1, CvPoint c2);
int isTooFar(CvPoint2D32f centroid, CvBox2D ellipse);
int addEllipse(Line *line, int *numOfEllipses, CvBox2D ellipses[ELLIPSE_MAX_NUM], Line *ellipseLines[ELLIPSE_MAX_NUM], CvPoint2D32f centroid);
void insert(Line *line, CvBox2D box, int *numOfEllipses, CvBox2D ellipses[ELLIPSE_MAX_NUM], Line *ellipseLines[ELLIPSE_MAX_NUM], CvPoint2D32f centroid);

#endif

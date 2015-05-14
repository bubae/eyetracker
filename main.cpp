#include <stdio.h>
#include "Headers/Candidate.h"

//NameSpaces
using namespace std;

int main(int argc, char *argv[])
{
	if (argc == 2) {
		VideoCapture video;
		video.set(CV_CAP_PROP_FOURCC, CV_FOURCC('D','I','V','4'));
		video.open(argv[1]);
		video.set(CV_CAP_PROP_FPS, 30);

		if (!video.isOpened())
		{
			cout<<"Failed to open file!!!"<<endl;
			return -1;
		}
	
		Mat frame;
		while(video.read(frame))
		{
			IplImage oriImg = frame;

			IplImage *cvImg = cvCreateImage(cvGetSize(&oriImg), IPL_DEPTH_8U, 1);

			cvCvtColor(&oriImg, cvImg, CV_RGB2GRAY);

			CvBox2D pupil = findPupil(cvImg);

			// exception check
			if (pupil.size.width > 0) {
				drawEllipse(pupil, &oriImg);
			}
			else {
				printf("No pupil detected!!\n");
			}

			cvShowImage("Canny", cvImg);

			cvShowImage("Cam", &oriImg);

			if (cvWaitKey(10) >= 0) break;

			cvReleaseImage(&cvImg);
		}
	}
	else
	{
		CvCapture *capture = cvCaptureFromCAM(-1);
		if (!capture) {
			printf("No camera\n");
			return -1;
		}
		IplImage *frame;

		while(1){
			frame = cvQueryFrame(capture);

			if(!frame) {
				printf("Cannot read cam\n");
				cvReleaseCapture(&capture);
			
				return -1;
			}

			IplImage *cvImg = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);

			cvCvtColor(frame, cvImg, CV_RGB2GRAY);

			CvBox2D pupil = findPupil(cvImg);

			// exception
			if (pupil.size.width > 0) {
				drawEllipse(pupil, frame);
			}
			else {
				printf("No pupil detected!!\n");
			}

			printf("Pupil size : %f\n", pupil.size.height);

			cvShowImage("Canny", cvImg);

			cvShowImage("Cam", frame);

			if (cvWaitKey(10) >= 0) break;

			cvReleaseImage(&cvImg);
		}

		cvReleaseImage(&frame);
		cvReleaseCapture(&capture);
	}
	return 0;
}



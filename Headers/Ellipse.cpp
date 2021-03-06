#include "Ellipse.h"

void findEllipsesFromImage(IplImage* cvImg, int *numOfEllipses, CvBox2D ellipses[ELLIPSE_MAX_NUM], Line *ellipseLines[ELLIPSE_MAX_NUM], CvPoint2D32f *centroid) {
	int i;
	int numOfNearCircular = 0;

	*centroid = centerOfLargestBlob(cvImg);

	CvPoint2D32f dummy_centroid = cvPoint2D32f(ROI_SIZE, ROI_SIZE);

	int numOfSegments;
	Line **lines = findContourLines(cvImg, dummy_centroid, &numOfSegments);

	for (i=0; i<numOfSegments; i++){
		int j;
		int numOfCorner;
		int *cornerIndexes;
		Line **sub_lines;
		int count;

		if (lines[i]->length < SEGMENT_MIN_CIRCULAR_LENGTH) continue;
		if (getEntropy(*lines[i]) < SEGMENT_MIN_ENTROPY) continue;

		cornerIndexes = getCorners(*lines[i], &numOfCorner);

		sub_lines = getSubArcs(*lines[i], numOfCorner, cornerIndexes, &count);
		numOfNearCircular += count;

		for (j=0; j<count; j++) {
			if (addEllipse(sub_lines[j], numOfEllipses, ellipses, ellipseLines, dummy_centroid) != 0) {
				free(sub_lines[j]->pixels);
				free(sub_lines[j]);
			}
			cvCircle(cvImg, cvPoint(lines[i]->pixels[cornerIndexes[j]].c, lines[i]->pixels[cornerIndexes[j]].r), 3, CV_RGB(255,255,255));
			int k;
			for (k=0; k<sub_lines[j]->length; k++) cvCircle(cvImg, cvPoint(sub_lines[j]->pixels[k].c, sub_lines[j]->pixels[k].r), 0.5, CV_RGB(255,255,255));
		}
		printf("# of corner : %d\n", count);

		free(sub_lines);
		free (cornerIndexes);

	} //end-for

	for (i=0; i<numOfSegments; i++) {
		free(lines[i]->pixels);
		free(lines[i]);
	}

	free(lines);
}

Line **findContourLines(IplImage *img, CvPoint2D32f centroid, int *numOfLines) {
	//	cvSetImageROI(img, cvRect(centroid.x - CANDIDATE_MAX_RADIUS, centroid.y - CANDIDATE_MAX_RADIUS, 2*CANDIDATE_MAX_RADIUS, 2*CANDIDATE_MAX_RADIUS));

	cvCanny(img, img, CANNY_MIN, CANNY_MAX, 3);

	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq *contours = cvCreateSeq(0, sizeof(CvSeq),sizeof(vector<CvPoint>), storage);
	int count = cvFindContours(img, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_NONE);

	if (count < 1) {
		printf("Error : No contour\n");
	}

	vector<vector<Point> > temp_contours;
	temp_contours.resize(count);
	CvSeq *temp = contours;

	cvSet(img, cvScalar(0,0,0));

	for(int i=0; i<count && temp; i++, temp=temp->h_next)
	{
		if (temp->total < MIN_CONTOUR_LENGTH) continue;

		Point* parray = new Point[temp->total];
		cvCvtSeqToArray(temp, parray, CV_WHOLE_SEQ);
		temp_contours[i].resize(temp->total);
		memcpy(temp_contours[i].data(), parray, sizeof(Point)*temp->total);
		for (int j=0; j<temp->total; j++) {
//			cvCircle(img, parray[j], 0.3, CV_RGB(255,255,255));
		}
	}

	cvReleaseMemStorage(&storage);

	cvCircle(img, cvPoint((int)centroid.x, (int)centroid.y), 3, CV_RGB(255,255,255));
	
	Line **lines = (Line **) malloc (sizeof(Line *) * count);
	for (int i=0; i<count; i++) {
		lines[i] = makeLine(temp_contours[i]);
		temp_contours[i].clear();
	}
	temp_contours.clear();

	*numOfLines = count;

	return lines;
}

CvPoint2D32f centerOfLargestBlob(IplImage* src) {
	unsigned int result = 0;
	CvPoint2D32f centroid;
	IplImage *labelImg;
	IplImage *gray;
	CvBlobs blobs;

	if (IMAGE_MODE == 1) {
		gray = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

		cvSmooth(src, gray, CV_GAUSSIAN, 5, 5, 5, 5);
		cvThreshold(gray, gray, ELLIPSE_BLOB_THRESHOLD, 255, CV_THRESH_BINARY_INV);

		labelImg = cvCreateImage(cvGetSize(gray), IPL_DEPTH_LABEL, 1);

		result = cvLabel(gray, labelImg, blobs);
	}
	else if (IMAGE_MODE == 2) {
		cvSmooth(src, src, CV_GAUSSIAN, 5, 5, 5, 5);
		cvThreshold(src, src, ELLIPSE_BLOB_THRESHOLD, 255, CV_THRESH_BINARY_INV);

		labelImg = cvCreateImage(cvGetSize(src), IPL_DEPTH_LABEL, 1);

		result = cvLabel(src, labelImg, blobs);
	}

	if(blobs.size() == 0)
	{
		printf("blob is zero\n");
		return centroid;
	}

	if(result > 0){
//		cvFilterByArea(blobs, ELLIPSE_BLOB_AREA_MIN, ELLIPSE_BLOB_AREA_MAX);
//		filterByDistance(blobs);
//		filterByCircularity(blobs);
	}

	unsigned int maxArea = 0;

	for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
	{
		CvBlob *blob = (*it).second;

		if (blob->area > maxArea)
		{
			centroid.x = blob->centroid.x;
			centroid.y = blob->centroid.y;
			maxArea = blob->area;
		}
	}

	cvSetImageROI(src, cvRect(centroid.x - ROI_SIZE, centroid.y - ROI_SIZE, 2*ROI_SIZE, 2*ROI_SIZE));

	cvReleaseBlobs(blobs);

	cvReleaseImage(&labelImg);

	if (IMAGE_MODE == 1) {
		cvReleaseImage(&gray);
	}
//	cvRenderBlobs(eye_label_image,blobs,eyeImg, tempImg,CV_BLOB_RENDER_BOUNDING_BOX);

	return centroid;
}


void filterByCircularity(CvBlobs& blobs)
{
    if(blobs.size()==0)
        return;
    CvBlobs::iterator it=blobs.begin();

    while(it!=blobs.end())
    {
        CvBlob* blob=(*it).second;
        double perimeter = cvContourChainCodePerimeter(&(blob->contour));
        double circularity = (4 * (PI)*blob->area)/std::pow(perimeter,2);
	
//	printf("filter : %lf, ", circularity);
        if(circularity > 1.0 || circularity < 0.2)
        {
            cvReleaseBlob(blob);
            CvBlobs::iterator tmp=it++;
            blobs.erase(tmp);
        }
        else it++;
     }
}

void filterByDistance(CvBlobs &blobs)
{
    if(blobs.size()==0)
        return;
    CvBlobs::iterator it=blobs.begin();
    while(it!=blobs.end())
    {
        CvBlob* blob=(*it).second;
        double d=sqrt(pow(blob->centroid.x, 2) + pow(blob->centroid.y, 2));

//	printf("%lf \n", d);
        if( d < 300.0)
        {
            cvReleaseBlob(blob);
            CvBlobs::iterator tmp=it;
            it++;
            blobs.erase(tmp);
        }
        else
            it++;
    }
}

CvBox2D getEllipseFromLine(Line *line) {
	CvMemStorage* storage = cvCreateMemStorage(0); 
        CvSeq* seq = cvCreateSeq(CV_32FC2, sizeof(CvSeq), sizeof(CvPoint2D32f), storage);  

	for (int i=0; i<line->length; i++) {       
		CvPoint2D32f p;
		p.x = (float)line->pixels[i].c;
		p.y = (float)line->pixels[i].r;
		cvSeqPush(seq, &p);
	}

        CvBox2D box = cvFitEllipse2(seq);
	
        cvClearSeq(seq);
        cvReleaseMemStorage(&storage);

	return box;
}

int myContains(CvPoint2D32f centroid, CvBox2D ellipse) {
	struct Point {
		double x;
		double y;
	} left, right, top, bottom;
	double dx, dy;
	double dleft, dright, dtop, dbottom;

	dx = centroid.x - ellipse.center.x;
	dy = centroid.y - ellipse.center.y;

	double radAngle = ellipse.angle > 90.0 ? (ellipse.angle-180.0)/180.0*M_PI : ellipse.angle/180.0*M_PI;

	left.x = ellipse.center.x - (ellipse.size.width*0.5)*cos(radAngle);
	left.y = ellipse.center.y - (ellipse.size.width*0.5)*sin(radAngle);

	right.x = ellipse.center.x + (ellipse.size.width*0.5)*cos(radAngle);
	right.y = ellipse.center.y + (ellipse.size.width*0.5)*sin(radAngle);

	top.x = ellipse.center.x - (ellipse.size.height*0.5)*sin(radAngle);
	top.y = ellipse.center.y - (ellipse.size.height*0.5)*cos(radAngle);

	bottom.x = ellipse.center.x + (ellipse.size.height*0.5)*sin(radAngle);
	bottom.y = ellipse.center.y + (ellipse.size.height*0.5)*cos(radAngle);

	double gradient = tan(radAngle);

	//printf("angle : %f, gradient : %f\n", ellipse.angle, gradient);
	if (gradient == 0) gradient = 0.00001;

	dleft = centroid.y - (left.y + (-1/gradient) * (centroid.x - left.x));
	dright = centroid.y - (right.y + (-1/gradient) * (centroid.x - right.x));
	dtop = centroid.y - (top.y + gradient * (centroid.x - top.x));
	dbottom = centroid.y - (bottom.y + gradient * (centroid.x - bottom.x));

	return (sqrt(dx*dx + dy*dy) < ELLIPSE_MAX_DISTANCE && dleft*dright < 0 && dtop*dbottom < 0);
}

double myDistance(CvPoint2D32f c1, CvPoint2D32f c2) {
	double dx, dy;

	dx = c1.x - c2.x;
	dy = c1.y - c2.y;

	return sqrt(dx*dx + dy*dy);
}

int isTooFar(CvPoint2D32f centroid, CvBox2D ellipse) {
	return (myDistance(centroid, ellipse.center) > ELLIPSE_MAX_DISTANCE);// && !myContains(centroid, ellipse);
}

int addEllipse(Line *line, int *numOfEllipses, CvBox2D ellipses[ELLIPSE_MAX_NUM], Line *ellipseLines[ELLIPSE_MAX_NUM], CvPoint2D32f centroid) {
	CvBox2D box = getEllipseFromLine(line);

//	if (!isTooFar(centroid, box)) {
		insert(line, box, numOfEllipses, ellipses, ellipseLines, centroid);	
		return 0;
//	}

//	return 1;
}

void insert(Line *line, CvBox2D box, int *numOfEllipses, CvBox2D ellipses[ELLIPSE_MAX_NUM], Line *ellipseLines[ELLIPSE_MAX_NUM], CvPoint2D32f centroid) {
	double current_distance = myDistance(centroid, box.center);
	int index;
	for (index=0; index<*numOfEllipses; index++) {
		if (current_distance < myDistance(centroid, ellipses[index].center)) 
			break;
	}
	for (int i=*numOfEllipses; i>index; i--) {
		if (i == ELLIPSE_MAX_NUM) continue;
		ellipses[i] = ellipses[i-1];
		ellipseLines[i] = ellipseLines[i-1];
	}
	if (index != ELLIPSE_MAX_NUM) {
		ellipses[index] = box;
		ellipseLines[index] = line;		
	}

	if (*numOfEllipses < ELLIPSE_MAX_NUM) {
		(*numOfEllipses)++;
	}
}

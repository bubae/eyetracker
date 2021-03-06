#include "Corner.h"

int *getCorners(Line line, int *numOfCorners) {
	int i;
	double *scaledGradient = NULL;
	double *convolvedGradient = NULL;
        int numOfCandidateCorners = 0;
	int *candidateCornerIndexes = NULL;
	int *cornerIndexes = NULL;

	scaledGradient = (double *) malloc (sizeof(double) * line.length);
	convolvedGradient = (double *) malloc (sizeof(double) * line.length);
	candidateCornerIndexes = (int *) malloc (sizeof(int) * line.length);

	scaledGradient[0] = fabs(line.pixels[0].gradient);
	for (i=1; i<line.length; i++) {
		double theta = fabs(line.pixels[i].gradient - line.pixels[i-1].gradient);

                for (; theta>180.0; theta -= 180.0) ;
		scaledGradient[i] = scaledGradient[i-1] + theta > 90.0 ? theta : 180.0 - theta;
	}

	for (i=0; i<line.length; i++) convolvedGradient[i] = 0.0;
	
	for (i=0; i<line.length; i++) {
		int j;
		for (j=i-CORNER_KERNEL_SIZE; j<=i+CORNER_KERNEL_SIZE; j++) {
                        if (j >= 0 && j < line.length)
        			convolvedGradient[j] += scaledGradient[i] * getKernelValue(j-i);
		}
	}

        numOfCandidateCorners = 0;
        candidateCornerIndexes[numOfCandidateCorners++] = 0;

	for (i=0; i<line.length; i++) {
		if (convolvedGradient[i] > CORNER_THRESHOLD) {
                        // local maxima
                        if ((i == 0 || convolvedGradient[i] > convolvedGradient[i-1])
                         && (i == line.length-1 || convolvedGradient[i] > convolvedGradient[i+1])) {
                		candidateCornerIndexes[numOfCandidateCorners++] = i;
                        }
		}
	}
        candidateCornerIndexes[numOfCandidateCorners++] = line.length-1;

	free(scaledGradient);
	free(convolvedGradient);

       	cornerIndexes = (int *) malloc (sizeof(int) * numOfCandidateCorners);
        (*numOfCorners) = candidateCornerIndexes[0];
        cornerIndexes[(*numOfCorners)++] = candidateCornerIndexes[0];
        for (i=1; i<numOfCandidateCorners-1; i++) {
               if (getAngleAtCorner(line, candidateCornerIndexes, i) < CORNER_ANGLE_THRESHOLD) {
                        cornerIndexes[(*numOfCorners)++] = candidateCornerIndexes[i];
                }
        }
        cornerIndexes[(*numOfCorners)++] = candidateCornerIndexes[i];

        free(candidateCornerIndexes);
	return cornerIndexes;
}

double getEstimatedLineFit(Line line, int start, int end, int x) {
        int dx, dy;
        double gradient;

        dx = line.pixels[end].c - line.pixels[start].c;
        dy = line.pixels[end].r - line.pixels[start].r;

        gradient = (double)dy/(double)dx;

        return line.pixels[start].r + (x - line.pixels[start].c) * gradient;
}

double leastSquareLineFit(Line line, int start, int end) {
        double sqSum = 0.0;
        int i;
        for (i=start; i<=end; i++) {
                double delta = line.pixels[i].r - getEstimatedLineFit(line, start, end, i);
                sqSum += delta*delta;
        }

        return sqrt(sqSum/(end-start+1));
}

double getAngleAtCorner(Line line, int *candidateCornerIndexes, int index){
        int pp, pn;

        if (candidateCornerIndexes[index] > 0 && candidateCornerIndexes[index] < line.length-1) {
                for (pp=candidateCornerIndexes[index]-1; pp > 0 && leastSquareLineFit(line, pp, candidateCornerIndexes[index]) < CORNER_LINEFIT_THRESHOLD; pp--);
                for (pn=candidateCornerIndexes[index]+1; pn < line.length-1 && leastSquareLineFit(line, candidateCornerIndexes[index], pn) < CORNER_LINEFIT_THRESHOLD; pn++);
                return getAngleBetweenVectors(line.pixels[pp],
                                         line.pixels[(candidateCornerIndexes[index])],
                                         line.pixels[pn]);
        }
        return 0.0;
}

double getAngleBetweenVectors(GPixel p, GPixel c, GPixel n) {
	struct Point {
		int x;
		int y;
	} v1, v2;

	v1.x = c.c - p.c;
	v1.y = c.r - p.r;

	v2.x = c.c - n.c;
	v2.y = c.r - n.r;

	return acos( (v1.x*v2.x + v1.y*v2.y) / (sqrt(v1.x*v1.x + v1.y*v1.y) * sqrt(v2.x*v2.x + v2.y*v2.y)) )*180.0/M_PI;
}

double getKernelValue(int index) {
        return getDervatedGaussian(0, CORNER_SIGMA, index);
}

double gaussian(double mean, double stddev, double x)
{ 
	double variance2 = stddev*stddev*2.0;
	double term = x-mean; 
	return (double)exp(-(term*term)/variance2)/(double)sqrt(M_PI*variance2);
}

double getDervatedGaussian(double mean, double stddev, double x) {
	return (gaussian(mean, stddev, x+CORNER_DELTA/2.0)-gaussian(mean, stddev, x-CORNER_DELTA/2.0)) / CORNER_DELTA;
}

int isTooShort(Line line) {
	return line.length < CORNER_MIN_LENGTH;
}

Line **getSubArcs(Line line, int numOfCorner, int *cornerIndexes, int *count) {
        int i;
	Line **sub_lines = (Line **) malloc (sizeof(Line *) * (numOfCorner-1));

	for (i=0, (*count)=0; i<(numOfCorner-1); i++) {
		sub_lines[(*count)] = makeLine(line, cornerIndexes[i], cornerIndexes[i+1]);

		if (isTooShort(*(sub_lines[(*count)])) || 
                        getEntropy(*(sub_lines[(*count)])) < CORNER_MIN_ENTROPY) {

			free(sub_lines[(*count)]->pixels);
			free(sub_lines[(*count)]);
                        continue;
                }
                
		(*count)++;
	}

	if (*count == 0) {
		free(sub_lines);
		return NULL;
	}
        return sub_lines;
}


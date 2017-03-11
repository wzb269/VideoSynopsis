#include "stdafx.h"
#include "detectMovingObjects.h"

MovingObjects::MovingObjects()
{
	setLearningRate(-1);
	setMinArea(100);
	bgsubtractor = createBackgroundSubtractorKNN(500, 400, false);
}

MovingObjects::~MovingObjects()
{
}

void MovingObjects::setLearningRate(double learningRate)
{
	learningRate = learningRate;
}

void MovingObjects::setMinArea(double area)
{
	minArea = area;
}

vector<Rect> MovingObjects::refineObjects(const vector<vector<Point>>& contours)
{
	vector<Rect> boundRect(contours.size());
	Rect tempRect;
	vector<Point> contours_poly;

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly, 3, true);
		tempRect = boundingRect(Mat(contours_poly));
		if (tempRect.area() >= minArea)
		{
			boundRect.push_back(tempRect);
		}
	}

	//相互包含或接触取最大面积的
	return boundRect;
}

vector<CvObject> MovingObjects::refineSegments(const Mat& img, Mat& mask, Mat& dst)
{
	vector<CvObject> cvObjects;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat temp;

	//open close
	morphologyEx(mask, temp, MORPH_OPEN, Mat(), Point(-1, -1), 1);
	morphologyEx(temp, temp, MORPH_CLOSE, Mat(), Point(-1, -1), 1);

	//find contours
	findContours(temp, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	dst = Mat::zeros(img.size(), CV_8UC3);
	if (contours.size() == 0)
		return cvObjects;

	/*
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
	}

	//rectangle
	RNG rng(12345);
	for (int i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(dst, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		rectangle(dst, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
	}

	// iterate through all the top-level contours,
	// draw each connected component with its own random color
	int idx = 0, largestComp = 0;
	double maxArea = 0;
	for (; idx >= 0; idx = hierarchy[idx][0])
	{
		const vector<Point>& c = contours[idx];
		double area = fabs(contourArea(Mat(c)));
		if (area > maxArea)
		{
			maxArea = area;
			largestComp = idx;
		}
	}
	Scalar color(0, 0, 255);
	drawContours(dst, contours, largestComp, color, FILLED, LINE_8, hierarchy);
	*/

	//rectangle
	vector<Rect> boundRect = refineObjects(contours);
	RNG rng(12345);
	for (int i = 0; i< boundRect.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		rectangle(dst, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
	}

	return cvObjects;
}

double MovingObjects::calThreshold(const Mat& img)
{
	return 100;
}

vector<CvObject> MovingObjects::detectMovingObjects(Mat frame)
{
	vector<CvObject> cvObjects;
	if (frame.empty())
	{
		printf("can not read data from the video source\n");
		return cvObjects;
	}
	
	Mat out_frame;
	bgsubtractor->setDist2Threshold(calThreshold(frame)); //otsu
	bgsubtractor->apply(frame, bgmask, learningRate);
	cvObjects = refineSegments(frame, bgmask, out_frame);

	imshow("video", frame);
	imshow("detect moving objects", out_frame);

	return cvObjects;
}
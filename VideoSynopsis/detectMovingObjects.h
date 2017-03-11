#pragma once
#include "stdafx.h"
#include "sharedInterface.h"

class MovingObjects
{
public:
	MovingObjects();
	~MovingObjects();
	vector<CvObject> detectMovingObjects(Mat frame);
	void setLearningRate(double learningRate);
	void setMinArea(double area);
private:
	static double calThreshold(const Mat& img);
	vector<CvObject> refineSegments(const Mat& img, Mat& mask, Mat& dst);
	vector<Rect> refineObjects(const vector<vector<Point>>&);

	double learningRate;
	double minArea;
	Mat bgmask;
	Ptr<BackgroundSubtractorKNN> bgsubtractor;
};
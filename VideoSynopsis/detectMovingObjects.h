#pragma once
#include "stdafx.h"
#include "sharedInterface.h"

class MovingObjects
{
public:
	MovingObjects();
	~MovingObjects();
	vector<Rect> detectMovingObjects(Mat frame);
	void setLearningRate(double learningRate);
	void setMinArea(int area);
private:
	static double calThreshold(const Mat& img);
	static void postProcessVideo(const Mat& img);
	static void abolishInsidedBox(const vector<Rect>&, vector<Rect>&);
	void refineSegments(const Mat& img, Mat& mask, Mat& dst, vector<Rect>& boundRect);
	void refineObjects(const vector<vector<Point>>&, vector<Rect>& boundRect);

	double learningRate;
	int minArea;
	Mat bgmask;
	Ptr<BackgroundSubtractorCNT> bgsubtractor;
};
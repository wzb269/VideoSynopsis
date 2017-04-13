#pragma once
#include "stdafx.h"
#include "sharedInterface.h"
#include "random.h"

const int INITIAL_TEMP = 1000;
const double SPEED = 0.98;
const double MIN_TEMP = 0.001;
const double alpha = 0.1;
const double beta = 0.2;

class MergeMovingObjects
{
public:
	MergeMovingObjects();
	~MergeMovingObjects();
	void run(map<int, vector<CvObject>> &objects, VideoCapture &sourceVideoCapture);

private:
	void merge(map<int, vector<CvObject>> &objects, VideoCapture &sourceVideoCapture);
	void preDefinedLength(map<int, vector<CvObject>> &objects);
	void filterFrames();
	void sa(map<int, vector<CvObject>> &objects, VideoCapture &sourceVideoCapture);
	double calEnergy(map<int, vector<CvObject>> &objects, VideoCapture &sourceVideoCapture);
	double calActivityLoss(map<int, vector<CvObject>> &objects, VideoCapture &sourceVideoCapture);
	double calOcclusions(map<int, vector<CvObject>> &objects, VideoCapture &sourceVideoCapture);
	double getRGB(int x, int y, int t, VideoCapture &sourceVideoCapture);
	void copyROI(Mat &src, Mat &dst, Rect &rect);
	bool overlapBoundingBox(Rect boundingBox1, Rect boundingBox2, int x1, int y1, int x2, int y2);

	int length; //the length of video synopsis
	set<int> removeObjects;
};
#pragma once
#include "stdafx.h"

class MovingObjects
{
public:
	MovingObjects();
	~MovingObjects();
	int detectMovingObjects(VideoCapture preprocessVideoCapture);
private:
	static void refineSegments(const Mat& img, Mat& mask, Mat& dst);
};
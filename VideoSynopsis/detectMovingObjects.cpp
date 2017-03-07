#include "stdafx.h"
#include "detectMovingObjects.h"

MovingObjects::MovingObjects()
{
}

MovingObjects::~MovingObjects()
{
}

void MovingObjects::refineSegments(const Mat& img, Mat& mask, Mat& dst)
{
	int niters = 3;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat temp;
	dilate(mask, temp, Mat(), Point(-1, -1), niters);
	erode(temp, temp, Mat(), Point(-1, -1), niters * 2);
	dilate(temp, temp, Mat(), Point(-1, -1), niters);
	findContours(temp, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	dst = Mat::zeros(img.size(), CV_8UC3);
	if (contours.size() == 0)
		return;
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
}

int MovingObjects::detectMovingObjects(VideoCapture preprocessVideoCapture)
{
	if (!preprocessVideoCapture.isOpened())
	{
		printf("\nCan not open camera or video file\n");
		return -1;
	}
	bool update_bg_model = true;
	Mat tmp_frame, bgmask, out_frame;
	preprocessVideoCapture >> tmp_frame;
	if (tmp_frame.empty())
	{
		printf("can not read data from the video source\n");
		return -1;
	}
	namedWindow("video", 1);
	namedWindow("segmented", 1);
	Ptr<BackgroundSubtractorKNN> bgsubtractor = createBackgroundSubtractorKNN(500, 400, false);
	bgsubtractor->setDist2Threshold(100);
	for (;;)
	{
		preprocessVideoCapture >> tmp_frame;
		if (tmp_frame.empty())
			break;
		bgsubtractor->apply(tmp_frame, bgmask, update_bg_model ? -1 : 0);
		refineSegments(tmp_frame, bgmask, out_frame);
		imshow("video", tmp_frame);
		imshow("segmented", out_frame);
		char keycode = (char)waitKey(30);
		if (keycode == 27)
			break;
	}
	destroyWindow("video");
	destroyWindow("segmented");
	return 0;
}
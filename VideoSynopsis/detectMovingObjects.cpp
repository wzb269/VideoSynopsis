#include "stdafx.h"
#include "detectMovingObjects.h"

bool comp(Rect a, Rect b)
{
	return a.area() < b.area();
}

MovingObjects::MovingObjects()
{
	bgsubtractor = createBackgroundSubtractorCNT();
	setLearningRate(-1);
	setMinArea(500);
}

MovingObjects::~MovingObjects()
{
}

void MovingObjects::setLearningRate(double learningRate)
{
	learningRate = learningRate;
}

void MovingObjects::postProcessVideo(const Mat& fore)
{
	erode(fore, fore, Mat());
	erode(fore, fore, Mat());
	dilate(fore, fore, Mat());
	dilate(fore, fore, Mat());
	dilate(fore, fore, Mat());
	dilate(fore, fore, Mat());
	dilate(fore, fore, Mat());
	dilate(fore, fore, Mat());
	dilate(fore, fore, Mat());

	cv::normalize(fore, fore, 0, 1., cv::NORM_MINMAX);
	cv::threshold(fore, fore, .5, 1., CV_THRESH_BINARY);
}

void MovingObjects::setMinArea(int area)
{
	minArea = area;
}

void MovingObjects::abolishInsidedBox(const vector<Rect>& srcBoundRect, vector<Rect>& dstBoundRect)
{
	for (int i = 0; i < srcBoundRect.size(); i++)
	{
		bool flag = true;
		for (int j = i + 1; j < srcBoundRect.size(); j++)
		{
			if (srcBoundRect[j].x <= srcBoundRect[i].x &&
				srcBoundRect[j].y <= srcBoundRect[i].y &&
				srcBoundRect[j].x + srcBoundRect[j].width >= srcBoundRect[i].x + srcBoundRect[i].width &&
				srcBoundRect[j].y + srcBoundRect[j].height >= srcBoundRect[i].y + srcBoundRect[i].height)
			{
				flag = false;
				break;
			}
		}
		if (flag)
			dstBoundRect.push_back(srcBoundRect[i]);
	}
}

void MovingObjects::refineObjects(const vector<vector<Point>>& contours, vector<Rect>& dstBoundRect)
{
	vector<Rect> srcBoundRect;
	Rect tempRect;
	vector<Point> contours_poly;

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly, 3, true);
		tempRect = boundingRect(Mat(contours_poly));
		if (tempRect.area() >= minArea)
		{
			srcBoundRect.push_back(tempRect);
			//printf("%d: %d\n", i, tempRect.area());
		}
	}

	//abolis te rect inside another rect.
	sort(srcBoundRect.begin(), srcBoundRect.end(), comp);
	abolishInsidedBox(srcBoundRect, dstBoundRect);
	printf("%d\n", (int)dstBoundRect.size());
}

void MovingObjects::refineSegments(const Mat& img, Mat& mask, Mat& dst, vector<Rect>& boundRect)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	//open close
	//morphologyEx(mask, temp, MORPH_OPEN, Mat(), Point(-1, -1), 1);
	//morphologyEx(temp, temp, MORPH_CLOSE, Mat(), Point(-1, -1), 1);
	postProcessVideo(mask);

	//find contours
	findContours(mask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	dst = Mat::zeros(img.size(), CV_8UC3);
	if (contours.size() > 0)
	{
		//rectangle
		refineObjects(contours, boundRect);
		RNG rng(12345);
		for (int i = 0; i< boundRect.size(); i++)
		{
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			rectangle(dst, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		}
	}
}

double MovingObjects::calThreshold(const Mat& img)
{
	return 100;
}

vector<Rect> MovingObjects::detectMovingObjects(Mat tmpFrame)
{
	vector<Rect> boundRect;
	if (tmpFrame.empty())
	{
		printf("can not read data from the video source\n");
		return boundRect;
	}

	Mat frame = tmpFrame;
	
	int scn = CV_MAT_CN(tmpFrame.type());
	CV_Assert(scn == 3 || scn == 4);
	cvtColor(tmpFrame, frame, CV_RGB2GRAY, 1);
	
	Mat out_frame;
	bgsubtractor->apply(frame, bgmask, learningRate);
	refineSegments(frame, bgmask, out_frame, boundRect);

	//imshow("video", frame);
	//imshow("detect moving objects", out_frame);

	return boundRect;
}
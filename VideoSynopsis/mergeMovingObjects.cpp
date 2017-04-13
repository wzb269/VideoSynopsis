#include "stdafx.h"
#include "mergeMovingObjects.h"

MergeMovingObjects::MergeMovingObjects()
{
	rand_seed((int)time(NULL));
}

MergeMovingObjects::~MergeMovingObjects()
{
}

void MergeMovingObjects::preDefinedLength(map<int, vector<CvObject>> &objects)
{
	int maxLen = 0;
	map<int, vector<CvObject>>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		maxLen = max(maxLen, (int)it->second.size());
	}
	this->length = maxLen;
}

void MergeMovingObjects::filterFrames()
{

}

bool MergeMovingObjects::overlapBoundingBox(Rect boundingBox1, Rect boundingBox2, int x1, int y1, int x2, int y2)
{
	CvPoint p1, p2;
	p1.x = std::max(boundingBox1.x, boundingBox2.x);
	p1.y = std::max(boundingBox1.y, boundingBox2.y);

	p2.x = std::min(boundingBox1.x + boundingBox1.width, boundingBox2.x + boundingBox2.width);
	p2.y = std::min(boundingBox1.y + boundingBox1.height, boundingBox2.y + boundingBox2.height);

	double AJoin = 0;
	if (p2.x > p1.x && p2.y > p1.y)
	{
		AJoin = (p2.x - p1.x) * (p2.y - p1.y) * 1.0;
		x1 = p1.x;
		y1 = p1.y;
		x2 = p2.x;
		y2 = p2.y;
		return true;
	}
	else
	{
		return false;
	}
}

double MergeMovingObjects::calActivityLoss(map<int, vector<CvObject>> &objects, VideoCapture &sourceVideoCapture)
{
	double sumActivityLoss = 0;
	set<int>::iterator itx;
	vector<CvObject>::iterator ity;
	for (itx = removeObjects.begin(); itx != removeObjects.end(); itx++)
	{
		if (objects.find(*itx) != objects.end())
		{
			for (ity = objects[*itx].begin(); ity != objects[*itx].end(); ity++)
			{
				sumActivityLoss += ity->rect.area();
			}
		}
	}
	return sumActivityLoss;
}

double MergeMovingObjects::getRGB(int x, int y, int t, VideoCapture &sourceVideoCapture)
{
	Mat frame;
	sourceVideoCapture.set(CV_CAP_PROP_POS_FRAMES, t*1.0);
	sourceVideoCapture >> frame;
	return (frame.at<cv::Vec3b>(x, y)[0] + frame.at<cv::Vec3b>(x, y)[1] + frame.at<cv::Vec3b>(x, y)[2]) / 3;
}

double MergeMovingObjects::calOcclusions(map<int, vector<CvObject>> &objects, VideoCapture &sourceVideoCapture)
{
	double sumOcclusions = 0;
	map<int, vector<CvObject>>::iterator itx;
	map<int, vector<CvObject>>::iterator ity;
	map<int, vector<CvObject>>::iterator itemp;
	vector<CvObject>::iterator itvx;
	vector<CvObject>::iterator itvy;
	int i, j;
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;

	for (itx = objects.begin(); itx != objects.end(); itx++)
	{
		itemp = itx;
		itemp++;
		for (ity = itemp; ity != objects.end(); ity++)
		{
			for (itvx = itx->second.begin(), itvy = ity->second.begin(); itvx < itx->second.end(), itvy < ity->second.end(); itvx++, itvy++)
			{
				if (overlapBoundingBox(itvx->rect, itvy->rect, x1, y1, x2, y2))
				{
					for (i = x1; i <= x2; i++)
					{
						for (j = y1; j <= y2; j++)
						{
							sumOcclusions += pow(getRGB(i, j, itvx->t, sourceVideoCapture) - getRGB(i, j, itvx->t, sourceVideoCapture), 2);
						}
					}
				}
			}
		}
	}
	return sumOcclusions;
}

double MergeMovingObjects::calEnergy(map<int, vector<CvObject>> &objects, VideoCapture &sourceVideoCapture)
{
	return calActivityLoss(objects, sourceVideoCapture) + alpha * calOcclusions(objects, sourceVideoCapture) + beta * this->length;
}

void MergeMovingObjects::sa(map<int, vector<CvObject>> &objects, VideoCapture &sourceVideoCapture)
{
	removeObjects.clear();
	bool flag;
	double t = INITIAL_TEMP;
	double r = SPEED;
	double t_min = MIN_TEMP;
	double energy0, energy1, dEnergy;
	int Id, numberOfObjects;
	energy0 = calEnergy(objects, sourceVideoCapture);
	numberOfObjects = (int)objects.size();
	
	while (t > t_min)
	{
		Id = fast_rand() % numberOfObjects;
		if (removeObjects.find(Id) == removeObjects.end())
		{
			flag = true;
			removeObjects.insert(Id);
		}
		else
		{
			flag = false;
			removeObjects.erase(Id);
		}
		
		energy1 = calEnergy(objects, sourceVideoCapture);
		dEnergy = energy1 - energy0;
		if (dEnergy < 0 && fast_rand() % 1000 / 1000.0 > exp(-dEnergy * 1.0 / t))
		{
			if (flag)
			{
				removeObjects.erase(Id);
			}
			else
			{
				removeObjects.insert(Id);
			}
		}
		t *= r;
	}
}

void MergeMovingObjects::copyROI(Mat &src, Mat &dst, Rect &rect)
{
	Mat roi = src(rect);
	roi.copyTo(dst);
}

void MergeMovingObjects::merge(map<int, vector<CvObject>> &objects, VideoCapture &sourceVideoCapture)
{
	VideoWriter outputVideo;
	Size S = Size((int)sourceVideoCapture.get(CV_CAP_PROP_FRAME_WIDTH),    //Acquire input size
		(int)sourceVideoCapture.get(CV_CAP_PROP_FRAME_HEIGHT));
	int ex = static_cast<int>(sourceVideoCapture.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form
	outputVideo.open("output.avi", ex, sourceVideoCapture.get(CV_CAP_PROP_FPS), S, true);
	if (!outputVideo.isOpened())
	{
		return;
	}

	Mat backGround, tempFrame;
	sourceVideoCapture.set(CV_CAP_PROP_POS_FRAMES, 0);
	sourceVideoCapture >> backGround;

	map<int, vector<CvObject>>::iterator it;
	for (int i = 0; i < this->length; i++)
	{
		Mat frame = backGround;
		for (it = objects.begin(); it != objects.end(); it++)
		{
			if (i < it->second.size())
			{
				sourceVideoCapture.set(CV_CAP_PROP_POS_FRAMES, it->second[i].t);
				sourceVideoCapture >> tempFrame;
				copyROI(tempFrame, frame, it->second[i].rect);
			}
		}
		outputVideo << frame;
	}
}

void MergeMovingObjects::run(map<int, vector<CvObject>> &objects, VideoCapture &sourceVideoCapture)
{
	preDefinedLength(objects);
	sa(objects, sourceVideoCapture);
	merge(objects, sourceVideoCapture);
}
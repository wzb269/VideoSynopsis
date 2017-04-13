// VideoSynopsis.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "videoIO.h"
#include "preprocessVideo.h"
#include "detectMovingObjects.h"
#include "sharedInterface.h"
#include "multipleObjectTracking.h"
#include "similarityCalculator.h"
#include "mergeMovingObjects.h"

using namespace cv;
using namespace std;

map<int, vector<CvObject>> S;
int NumberOfObjects = 0;

void addNewPath(int ID, int time, Rect boundingBox)
{
	if (S.find(ID) == S.end())
	{
		vector<CvObject> boundingBoxs(1);
		boundingBoxs.push_back(CvObject(boundingBox, ID, time));
		S.insert(make_pair(ID, boundingBoxs));
	}
	else
	{
		S[ID].push_back(CvObject(boundingBox, ID, time));
	}
}

void addNewPaths(vector<CvObject>& T)
{
	for (int i = 0; i < T.size(); i++)
	{
		addNewPath(T[i].ID, T[i].t, T[i].rect);
	}
}

void estimateWhetherAddNewTrackers(const Mat & image, MultipleObjectTracking& mot, vector<Rect>& B, vector<CvObject>& T, int time)
{
	SimilarityCalculator sc;
	for (int i = 0; i < B.size(); i++)
	{
		bool flag = false;
		for (int j = 0; j < T.size(); j++)
		{
			if (sc.isBeSimilar(image, B[i], T[j].rect))
			{
				if (mot.updateTracker(image, B[i], T[j].ID))
				{
					flag = true;
					break;
				}
			}
		}
		if (!flag)
		{
			mot.addTracker(image, B[i]);
		}
	}
	mot.deleteTracker();
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cout << "Not enough parameters" << endl;
		return -1;
	}
	const string sourceReference = argv[1];
	//workflow: readVideo->preprocessVideo->backgroundModing->detect moving objects->track moving objects->classify
	//using deeplearing->energy minimize->generate video synopsis->writeVideo.

	Video video;
	//readVideo
	VideoCapture sourceVideoCapture = video.readVideo(sourceReference);
	if (!sourceVideoCapture.isOpened())
	{
		cout << "Could not open reference " << sourceReference << endl;
		return -1;
	}

	Preprocesser preprocesser;
	MovingObjects movingObjects;
	MultipleObjectTracking mot;
	int time = -1;

	namedWindow("video", 1);
	namedWindow("detect moving objects", 1);
	namedWindow("tracking moving objects", 1);
	vector<Rect> B;
	vector<CvObject> T;

	while (true)
	{
		Mat frame;
		sourceVideoCapture >> frame;
		if (frame.empty())
			break;
		time++;
		//preprocessVideo
		frame = preprocesser.preprocess(frame);

		//backgroundModing and detect moving object
		B = movingObjects.detectMovingObjects(frame);

		//multi object tracking 
		mot.trackMovingObjects(frame, T, time);
		addNewPaths(T);

		//the algorithm to estimate whether add new objects into tracking objects.
		estimateWhetherAddNewTrackers(frame, mot, B, T, time);
		
		//char keycode = (char)waitKey(30);
		//if (keycode == 27)
		//	break;
	}

	MergeMovingObjects mergeMovingObjects;
	mergeMovingObjects.run(S, sourceVideoCapture);

	
	
	destroyWindow("video");
	destroyWindow("detect moving objects");
	destroyWindow("tracking moving objects");
	sourceVideoCapture.release();
	return 0;
}

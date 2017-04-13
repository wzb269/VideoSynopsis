#include "stdafx.h"
#include "multipleObjectTracking.h"

MultipleObjectTracking::MultipleObjectTracking()
{
	clearTrackers();
	setNumberOfTrackers(0);
	setTrackerAlgorithm("KCF");
}

MultipleObjectTracking::~MultipleObjectTracking()
{
}

void MultipleObjectTracking::clearTrackers()
{
	trackers.clear();
}

void MultipleObjectTracking::setNumberOfTrackers(int count)
{
	numberOfTrackers = count;
}

void MultipleObjectTracking::setTrackerAlgorithm(String algorithm)
{
	tracker_algorithm = algorithm;
}

void MultipleObjectTracking::addNumberOfTrackers()
{
	numberOfTrackers++;
}


void MultipleObjectTracking::clearIDSet()
{
	updatedID.clear();
}

void MultipleObjectTracking::insertID(int ID)
{
	updatedID.insert(ID);
}

int MultipleObjectTracking::getNewTrackerID()
{
	return numberOfTrackers;
}

bool MultipleObjectTracking::addTracker(const Mat & image, Rect2d boundingBox)
{
	Ptr<Tracker> tracker = Tracker::create(tracker_algorithm);
	if (tracker->init(image, boundingBox))
	{
		trackers.insert((make_pair(getNewTrackerID(), tracker)));
		updatedID.insert(getNewTrackerID());
		addNumberOfTrackers();
		return true;
	}
	else
	{
		return false;
	}
}

bool MultipleObjectTracking::addTrackers(const Mat & image, vector< Rect2d > boundingBoxs)
{
	bool flag = false;
	for (int i = 0; i < boundingBoxs.size(); i++)
	{
		if (addTracker(image, boundingBoxs[i]))
		{
			flag = false;
		}
	}
	return flag;
}

bool MultipleObjectTracking::trackMovingObjects(const Mat & image, vector<CvObject> & cvObjects, int time)
{
	Rect2d boundingBox;
	cvObjects.clear();
	bool flag = false;
	map<int, Ptr<Tracker> >::iterator it;
	
	Mat tempImage = image;
	RNG rng(12345);
	for (it = trackers.begin(); it != trackers.end();++it)
	{
		if (it->second->update(image, boundingBox))
		{
			cvObjects.push_back(CvObject(boundingBox, it->first, time));
			flag = true;

			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			rectangle(tempImage, boundingBox.tl(), boundingBox.br(), color, 2, 8, 0);
		}
	}
	//imshow("tracking moving objects", tempImage);
	return flag;
}

bool MultipleObjectTracking::updateTracker(const Mat & image, Rect2d boundingBox, int ID)
{
	if (trackers[ID]->init(image, boundingBox))
	{
		insertID(ID);
		return true;
	}
	else
	{
		return false;
	}
}

void MultipleObjectTracking::deleteTracker()
{
	map<int, Ptr<Tracker> >::iterator it;
	for (it = trackers.begin(); it != trackers.end();)
	{
		if (updatedID.find(it->first) == updatedID.end())
		{
			//delete it->second;
			trackers.erase(it++);
		}
		else
		{
			it++;
		}
	}
	clearIDSet();
}

#pragma once
#include "stdafx.h"
#include "sharedInterface.h"

class MultipleObjectTracking
{
public:
	MultipleObjectTracking();
	~MultipleObjectTracking();
	bool trackMovingObjects(const Mat & image, vector<CvObject> & cvObjects, int time);
	bool addTrackers(const Mat & image, vector< Rect2d > boundingBoxs);
	bool addTracker(const Mat & image, Rect2d boundingBox);
	bool updateTracker(const Mat & image, Rect2d boundingBox, int Id);
	void deleteTracker();
private:
	void insertID(int ID);
	void clearIDSet();
	int getNewTrackerID();
	void setNumberOfTrackers(int count);
	void addNumberOfTrackers();
	void clearTrackers();
	void setTrackerAlgorithm(String tracker_algorithm);
	int numberOfTrackers;
	map<int, Ptr<Tracker> > trackers;
	String tracker_algorithm;
	set<int> updatedID;
};
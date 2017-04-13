#pragma once
#include "stdafx.h"

typedef struct CvObject
{
	Rect rect;
	int t;
	int ID;
	CvObject(Rect r, int I, int time):rect(r), ID(I), t(time) {}
	CvObject(){}
} CvObject;
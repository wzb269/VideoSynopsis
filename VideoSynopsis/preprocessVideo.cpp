#include "stdafx.h"
#include "preprocessVideo.h"

Preprocesser::Preprocesser()
{
}

Preprocesser::~Preprocesser()
{
}

Mat Preprocesser::preprocess(Mat frame)
{
	Mat out_frame;
	//GaussianBlur
	GaussianBlur(frame, out_frame, Size(3, 3), 0, 0);
	return out_frame;
}
#pragma once
#include "stdafx.h"

class Video
{
public:
	Video();
	~Video();
	VideoCapture readVideo(string sourceReference);
	bool writeVideo(string outputPath);
	int videoIO(int argc, char *argv[]);

private:
	double getPSNR(const Mat& I1, const Mat& I2);
	Scalar getMSSIM(const Mat& I1, const Mat& I2);
	static void help();
};
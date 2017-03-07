// VideoSynopsis.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "videoIO.h"
#include "preprocessVideo.h"
#include "detectMovingObjects.h"

using namespace cv;
using namespace std;

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
	VideoCapture sourceVideoCapture = video.readVideo(sourceReference);
	if (!sourceVideoCapture.isOpened())
	{
		cout << "Could not open reference " << sourceReference << endl;
		return -1;
	}
	
	Preprocesser preprocesser;
	VideoCapture preprocessVideoCapture = preprocesser.preprocess(sourceVideoCapture);
	if (!preprocessVideoCapture.isOpened())
	{
		cout << "Could not preprocess reference " << sourceReference << endl;
		return -1;
	}
	
	MovingObjects movingObjects;
	movingObjects.detectMovingObjects(preprocessVideoCapture);
	

	preprocessVideoCapture.release();
	sourceVideoCapture.release();
	return 0;
}

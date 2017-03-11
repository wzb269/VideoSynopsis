// VideoSynopsis.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "videoIO.h"
#include "preprocessVideo.h"
#include "detectMovingObjects.h"
#include "sharedInterface.h"

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
	//readVideo
	VideoCapture sourceVideoCapture = video.readVideo(sourceReference);
	if (!sourceVideoCapture.isOpened())
	{
		cout << "Could not open reference " << sourceReference << endl;
		return -1;
	}

	Preprocesser preprocesser;
	MovingObjects movingObjects;

	namedWindow("video", 1);
	namedWindow("detect moving objects", 1);

	while (true)
	{
		Mat frame;
		sourceVideoCapture >> frame;

		//preprocessVideo
		frame = preprocesser.preprocess(frame);

		//backgroundModing and detect moving object
		movingObjects.detectMovingObjects(frame);
		
		char keycode = (char)waitKey(30);
		if (keycode == 27)
			break;
	}

	destroyWindow("video");
	destroyWindow("detect moving objects");
	sourceVideoCapture.release();
	return 0;
}

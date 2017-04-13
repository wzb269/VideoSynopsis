#include "stdafx.h"
#include "similarityCalculator.h"


SimilarityCalculator::SimilarityCalculator()
{
}

SimilarityCalculator::~SimilarityCalculator()
{
}

bool SimilarityCalculator::surfAlgorithm(const Mat & image1, const Mat & image2)
{
	return false;
}

bool SimilarityCalculator::overlapAlgorithm(Rect boundingBox1, Rect boundingBox2)
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
	}

	double A1 = boundingBox1.width * boundingBox1.height * 1.0;
	double A2 = boundingBox2.width * boundingBox2.height * 1.0;
	double AUnion = (A1 + A2 - AJoin);

	/*
	if (AUnion > 0 && AJoin / AUnion > 0.1)
	{
		return true;
	}
	*/
	if (A1 > 0 && AJoin / A1 > 0.5)
	{
		return true;
	}
	else if (A2 > 0 && AJoin / A2 > 0.5)
	{
		return true;
	}
	else
		return false;
}

bool SimilarityCalculator::isBeSimilar(const Mat & image, Rect boundingBox1, Rect boundingBox2)
{
	return overlapAlgorithm(boundingBox1, boundingBox2);
}
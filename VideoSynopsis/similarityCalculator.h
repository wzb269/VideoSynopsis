#pragma once
#include "stdafx.h"

class SimilarityCalculator
{
public:
	SimilarityCalculator();
	~SimilarityCalculator();
	bool isBeSimilar(const Mat & image, Rect boundingBox1, Rect boundingBox2);

private:
	bool surfAlgorithm(const Mat & image1, const Mat & image2);
	bool overlapAlgorithm(Rect boundingBox1, Rect BoundingBox2);
};
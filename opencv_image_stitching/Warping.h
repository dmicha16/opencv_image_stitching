#pragma once
#include "Wrapper.h"
#include "FeatureFindMatch.h"

class Warping :
	public Wrapper {
public:
	Warping();
	Mat warp(Mat &image, MatchedKeyPoint features);
	~Warping();

private:
	void perspective_warping_(Mat &img);
	void vector_split_(MatchedKeyPoint features);
	vector<Point2f> baseImagePts_;
	vector<Point2f> dstPts_;
protected:
	Mat warpedImage;
};


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
	int offSetY = 0;
	int offSetX = 0;
	int pre_offSetY = 0;
	int new_offSetY = 0;
	int numFeatures;
	vector<Point2f> baseImagePts_;
	vector<Point2f> dstPts_;
	Mat warpedImage;
	void perspective_warping_(Mat &img);
	void vector_split_(MatchedKeyPoint features);
};


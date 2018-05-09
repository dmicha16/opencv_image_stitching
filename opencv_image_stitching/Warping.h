#pragma once
#include "Wrapper.h"
#include "FeatureFindMatch.h"

class Warping :
	public Wrapper {
public:
	Warping();
	Mat warp(Mat image, MatchedKeyPoint features);
	Mat translate(Mat img, int offsetx, int offsety);
	~Warping();

private:
	int offSetY = 0;
	int offset_x_ = 0;
	int prev_offset_y_ = 0;
	int new_offset_y_ = 0;
	int num_features_;
	vector<Point2f> base_image_pts_;
	vector<Point2f> dst_pts_;
	Mat perspective_warping_(Mat &img);
	void vector_split_(MatchedKeyPoint features);
};

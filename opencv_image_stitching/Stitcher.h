#pragma once
#include "Wrapper.h"
//#include "Undistorter.h"

class Stitcher :
	public Wrapper {
public:
	Stitcher();
	Mat merging(Mat &img1, Mat &img2);
	Mat customMerger(Mat &img1, Mat &img2);
	~Stitcher();
private:
	Vec3b BLACKPIXEL = { 0, 0, 0 };
	Mat3b stitchedImage;
};


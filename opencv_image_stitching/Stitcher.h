#pragma once
#include "Wrapper.h"
//#include "Undistorter.h"

class Stitcher :
	public Wrapper {
public:
	Stitcher();
	Mat customMerger(Mat &img1, Mat &img2);
	Mat reduceImage(Mat &img, int offSetX);
	~Stitcher();
private:
	Vec3b BLACKPIXEL = {0,0,0};
	Mat3b stitchedImage;
};


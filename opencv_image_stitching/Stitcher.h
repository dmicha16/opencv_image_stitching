#pragma once
#include "Wrapper.h"
#include "Undistorter.h"

class Stitcher :
	public Wrapper {
public:
	Stitcher();
	Mat merging(Mat &img1, Mat &img2);
	~Stitcher();
protected:
	Mat3b stitchedImage;

};


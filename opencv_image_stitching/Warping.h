#pragma once
#include "Wrapper.h"
class Warping :
	public Wrapper {
public:
	Warping();
	void warp(Mat &image, Point2f features[][2]);
	~Warping();

private:
	void perspective_warping_(Mat &img);
	void vector_split_(Point2f features[][2]);
	vector<Point2f> baseImagePts_;
	vector<Point2f> dstPts_;

protected:
	Mat warpedImage;
};


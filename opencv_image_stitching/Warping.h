#pragma once
#include "Wrapper.h"
class Warping :
	public Wrapper {
public:
	Warping(vector<CameraParams> cameras, vector<Mat> images);
	void warp(Mat &image, vector<Point2f> &features);
	~Warping();

private:
	void perspective_(Mat &img);
	void vector_split_(vector<Point2f> &features);
	vector<Point2f> baseImagePts_;
	vector<Point2f> dstPts_;
protected:
	Mat warpedImage;
};


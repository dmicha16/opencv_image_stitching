#pragma once
#include "Wrapper.h"
class Undistorter :
	public Wrapper {
public:

	Undistorter(vector<Mat> images);
	vector<Mat> returnDistCoef();
	vector<CameraParams> returnCamera();
	vector<Mat> returnUndistortedImages();
	~Undistorter();

private:
	vector<Mat> undist_images_clone;
	Mat dist_coef;
	Mat R; 
	Mat K; 
	double aspect;
	double ppx;
	double ppy;
	double focal;
	Mat t;
	vector<CameraParams> cameras_temp;
};


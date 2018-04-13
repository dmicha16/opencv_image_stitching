#pragma once
#include "Wrapper.h"
class CameraParameters :
	public Wrapper {
public:

	CameraParameters(vector<Mat> images);
	vector<Mat> returnDistCoef();
	vector<CameraParams> returnCamera();
	vector<Mat> returnUndistortedImages();
	~CameraParameters();

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


#pragma once
#include "Pipeline.h"
class CameraParameters :
	public Pipeline {
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
	Mat t;
	vector<CameraParams> cameras_temp;
};


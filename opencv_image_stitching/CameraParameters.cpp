#pragma once
#include "CameraParameters.h"

CameraParameters::CameraParameters(vector<Mat> images) {

	dist_coef = (Mat_<double>(5, 1) << 4.375478934563331e-05, 4.403608673838324e-05, 2.935527496986621e-05, 3.351971854648422e-06, 6.29700209050618e-05);
	R = (Mat_<double>(3, 3) << 6401.891293682877, 0, 2.958184954287773, 0, 11509.41847923992, 4.334193404838331, 0, 0, 1);

	vector<Mat> undist_images(images.size());
	vector<CameraParams> cameras(images.size());

	for (int i = 0; i < static_cast<int>(images.size()); i++) {
		undistort(images[i], undist_images[i], R, dist_coef);
	}
	for (size_t i = 0; i < static_cast<int>(images.size()); i++) {
		undist_images_clone.push_back(undist_images[i]);
	}

	for (size_t i = 0; i < images.size(); ++i) {
		cameras[i].R.convertTo(R, CV_32F);
		cameras[i].R = R;
	}

	for (size_t i = 0; i < cameras.size(); i++) {
		cameras_temp.push_back(cameras[i]);
	}
}



vector<Mat> CameraParameters::returnUndistortedImages() {
	return undist_images_clone;
}

vector<Mat> CameraParameters::returnDistCoef() {
	return dist_coef;
}

vector<CameraParams> CameraParameters::returnCamera() {
	return cameras_temp;
}	

CameraParameters::~CameraParameters() {
}

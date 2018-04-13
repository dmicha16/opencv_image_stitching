#pragma once
#include "Undistorter.h"

Undistorter::Undistorter(vector<Mat> images) {

	dist_coef = (Mat_<double>(5, 1) << -0.1231, 0.0660, -2.2759e-04, -0.0036, -0.0040);
	//K = (Mat_<double>(3, 3) << 3713.62144680322, 0, 2722.06620618404, 0, 3694.94103056275, 1838.15786367077, 0, 0, 1); // you cant set K. It's a function in the CameraParams struct
	R = (Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);
	t = (Mat_<double>(1, 3) << 0, 0, 0); //note that this might be Mat_<double>(3,1)... shitty documentation as usual so if it doesn't work then this might be it
	aspect = (3.6949/3.7136);
	ppx = 2.7221e+03;
	ppy = 1.8382e+03;
	focal = 3.7000e+03; //another uncertain value. I don't quite know which focal length they want here

	vector<Mat> undist_images(images.size());
	vector<CameraParams> cameras(images.size());

	for (int i = 0; i < static_cast<int>(images.size()); i++) {
		undistort(images[i], undist_images[i], R, dist_coef);
	}
	for (size_t i = 0; i < static_cast<int>(images.size()); i++) {
		undist_images_clone.push_back(undist_images[i]);
	}

	for (size_t i = 0; i < images.size(); ++i) {
		cameras[i].R.convertTo(R, CV_32F); //I don't think you need this. It's just to convert the values in the Mat to floats, and they're already doubles
		cameras[i].R = R;
		//cameras[i].K = K;
		cameras[i].t = t;
		cameras[i].ppx = ppx;
		cameras[i].ppy = ppy;
		cameras[i].focal = focal;
		cameras[i].aspect = aspect;
	}

	for (size_t i = 0; i < cameras.size(); i++) {
		cameras_temp.push_back(cameras[i]);
	}
}

vector<Mat> Undistorter::returnUndistortedImages() {
	return undist_images_clone;
}

vector<Mat> Undistorter::returnDistCoef() {
	return dist_coef;
}

vector<CameraParams> Undistorter::returnCamera() {
	return cameras_temp;
}	

Undistorter::~Undistorter() {
}

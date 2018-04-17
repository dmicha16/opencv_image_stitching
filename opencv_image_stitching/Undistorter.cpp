#pragma once
#include "Undistorter.h"

Undistorter::Undistorter() {
}

vector<Mat> Undistorter::undistort_images(vector<Mat> images) {
	int num_images = static_cast <int> (images.size());


	dist_coef_ = (Mat_<double>(5, 1) << -0.1231, 0.0660, -2.2759e-04, -0.0036, -0.0040);
	//K = (Mat_<double>(3, 3) << 3713.62144680322, 0, 2722.06620618404, 0, 3694.94103056275, 1838.15786367077, 0, 0, 1); // you cant set K. It's a function in the CameraParams struct
	R_ = (Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);
	t_ = (Mat_<double>(1, 3) << 0, 0, 0); //note that this might be Mat_<double>(3,1)... shitty documentation as usual so if it doesn't work then this might be it
	aspect_ = (3.6949 / 3.7136);
	ppx_ = 2.7221e+03;
	ppy_ = 1.8382e+03;
	focal_ = 3.7000e+03; //another uncertain value. I don't quite know which focal length they want here

	vector<Mat> undist_images(num_images);
	vector<CameraParams> cameras(num_images);

	for (int i = 0; i < num_images; i++) {
		undistort(images[i], undist_images[i], R_, dist_coef_);
	}

	for (size_t i = 0; i < images.size(); ++i) {
		cameras[i].R.convertTo(R_, CV_32F); //I don't think you need this. It's just to convert the values in the Mat to floats, and they're already doubles
		cameras[i].R = R_;
		//cameras[i].K = K;
		cameras[i].t = t_;
		cameras[i].ppx = ppx_;
		cameras[i].ppy = ppy_;
		cameras[i].focal = focal_;
		cameras[i].aspect = aspect_;
	}
	return undist_images;
}

Undistorter::~Undistorter() {
}

#pragma once
#include "Undistorter.h"

Undistorter::Undistorter() {
}

vector<Mat> Undistorter::undistort_images(vector<Mat> images) {
	int num_images = static_cast <int> (images.size());
	cout << "What focal length where the images taken at? 24, 70 or 105? \n";
	bool valid_focal = false;

	while (!valid_focal)
	{
		cin >> focal_choice;

		switch (focal_choice) {
		case 24:
			dist_coef_ = (Mat_<double>(4, 1) << -0.1230, 0.0656, 3.6320e-04, -0.0037);
			K_ = (Mat_<double>(3, 3) << 3833.31781429424, 0, 2697.48278141202, 0, 3813.37799108667, 1803.10641934199, 0, 0, 1);
			valid_focal = true;
			break;
		case 70:
			dist_coef_ = (Mat_<double>(4, 1) << 0.3307, -0.1960, -0.0026, -0.0226);
			K_ = (Mat_<double>(3, 3) << 10943.9786113928, 0, 2399.19499475266, 0, 10947.5919070818, 1776.78160458216, 0, 0, 1);
			valid_focal = true;
			break;
		case 105:
			dist_coef_ = (Mat_<double>(4, 1) << 0.5809, -0.7583, 0.0247, -0.0292);
			K_ = (Mat_<double>(3, 3) << 14540.8047710624, 0, 2220.02157085377, 0, 14593.7668319789, 2284.14820122021, 0, 0, 1);
			valid_focal = true;
			break;
		default:
			cout << "Invalid focal length, try again.\n";
			break;
		}
	}

	vector<Mat> undist_images(num_images);


	for (int i = 0; i < num_images; i++) {
		undistort(images[i], undist_images[i], K_, dist_coef_);
	}
	return undist_images;
}

Undistorter::~Undistorter() {
}

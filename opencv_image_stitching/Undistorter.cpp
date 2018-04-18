#pragma once
#include "Undistorter.h"

Undistorter::Undistorter() {
}

vector<Mat> Undistorter::undistort_images(vector<Mat> images) {
	int num_images = static_cast <int> (images.size());
	
	dist_coef_ = (Mat_<double>(5, 1) << -0.1231, 0.0660, -2.2759e-04, -0.0036, -0.0040);
	K_ = (Mat_<double>(3, 3) << 3713.62144680322, 0, 2722.06620618404, 0, 3694.94103056275, 1838.15786367077, 0, 0, 1); 
	
	vector<Mat> undist_images(num_images);
	

	for (int i = 0; i < num_images; i++) {
		undistort(images[i], undist_images[i], K_, dist_coef_);
	}
	return undist_images;
}

Undistorter::~Undistorter() {
}

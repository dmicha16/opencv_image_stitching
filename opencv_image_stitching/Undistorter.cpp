#pragma once
#include "Undistorter.h"

Undistorter::Undistorter() {
}

vector<Mat> Undistorter::undistort_images(vector<Mat> images) {
	int num_images = static_cast <int> (images.size());
	cout << "Input the focal length: 24, 70 or 105" << endl;
	bool valid_focal = false;
	int focal_choice;
	Mat dist_coef_;
	Mat K_;

	while (!valid_focal)
	{
		cin >> focal_choice;

		switch (focal_choice) {
		case 24:
			dist_coef_ = (Mat_<double>(5, 1) << -0.1231, 0.0660, -2.2759e-04, -0.0036, -0.0040);
			K_ = (Mat_<double>(3, 3) << 3713.62144680322, 0, 2722.06620618404, 0, 3694.94103056275, 1838.15786367077, 0, 0, 1);
			valid_focal = true;
			break;
		case 70:
			dist_coef_ = (Mat_<double>(5, 1) << 0.2194, 0.3437, -0.0172, 0.0014, -1.8523);
			K_ = (Mat_<double>(3, 3) << 10082.4680175580, 0, 2798.12180794860, 0, 10157.7588012111, 1498.56939956438, 0, 0, 1);
			valid_focal = true;
			break;
		case 105:
			dist_coef_ = (Mat_<double>(5, 1) << 0.7936, -6.1629, 0.0602, -0.0051, 62.1371);
			K_ = (Mat_<double>(3, 3) << 14861.9389818321, 0, 2756.13810209998, 0, 14974.9858416224, 2903.01477743931, 0, 0, 1);
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

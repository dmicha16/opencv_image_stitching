#pragma once
#include "Undistorter.h"

Undistorter::Undistorter(string path) {
	vector<Mat> images = read_images_(path);
	undistort_images_(images);
}

void Undistorter::undistort_images_(vector<Mat> images) {
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
}

vector<Mat> Undistorter::read_images_(string path) {
	vector<String> photos;
	glob(path, photos, false);
	//string file_name = "C:/photos/T4D/KEYPOINTS/test";

	cout << "Images read: " << photos.size() << endl;
	for (int i = 0; i < photos.size(); i++) {
		img_names.push_back(photos[i]);
	}

	num_images = static_cast <int> (img_names.size());
	vector<Size> full_img_sizes(num_images);
	vector<Mat> images(num_images);
	images = upload_images_(images, full_img_sizes);
	return images;
}

vector<Mat> Undistorter::upload_images_(vector<Mat> images, vector<Size> full_img_sizes) {

	Mat full_img, img;
	double work_megapix = 0.6;
	double seam_megapix = 0.1;

	double work_scale = 1, seam_scale = 1, compose_scale = 1, seam_work_aspect = 1;
	bool is_work_scale_set = false, is_seam_scale_set = false, is_compose_scale_set = false;

	for (int i = 0; i < num_images; ++i) {
		full_img = imread(img_names[i]);
		full_img_sizes[i] = full_img.size();

		if (work_megapix < 0) {
			img = full_img;
			work_scale = 1;
			is_work_scale_set = true;
		}
		else {
			if (!is_work_scale_set) {
				work_scale = min(1.0, sqrt(work_megapix * 1e6 / full_img.size().area()));
				is_work_scale_set = true;
			}
			resize(full_img, img, Size(), work_scale, work_scale, INTER_LINEAR_EXACT);
		}
		if (!is_seam_scale_set) {
			seam_scale = min(1.0, sqrt(seam_megapix * 1e6 / full_img.size().area()));
			seam_work_aspect = seam_scale / work_scale;
			is_seam_scale_set = true;
		}


		images[i] = img.clone();
	}
	full_img.release();
	img.release();
	return images;
}

Undistorter::~Undistorter() {
}

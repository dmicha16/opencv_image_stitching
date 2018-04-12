#include "Warping.h"

Warping::Warping(vector<CameraParams> cameras, vector<Mat> images) {
	double work_scale = 1, seam_scale = 1, compose_scale = 1, seam_work_aspect = 1;
	total_warper(cameras, images, seam_work_aspect);
}

void Warping::total_warper(vector<CameraParams> cameras, vector<Mat> images, double seam_work_aspect) {
	int num_images = static_cast<int>(images.size());
	float warped_image_scale = 0;
	vector<Point> corners(num_images);
	vector<UMat> masks_warped(num_images);
	vector<UMat> images_warped(num_images);
	vector<Size> sizes(num_images);
	vector<UMat> masks(num_images);

	for (int i = 0; i < num_images; ++i) {
		masks[i].create(images[i].size(), CV_8U);
		masks[i].setTo(Scalar::all(255));
	}

	Ptr<WarperCreator> warper_creator;
	warper_creator = makePtr<cv::AffineWarper>();
	Ptr<RotationWarper> warper = warper_creator->create(static_cast<float>(warped_image_scale * seam_work_aspect));
	cout << cameras.size() << endl;
	WINPAUSE;
	for (int i = 0; i < num_images; ++i) {
		Mat_<float> K;
		cameras[i].K().convertTo(K, CV_32F); // Converts an array to another data type with optional scaling. K = output
		float swa = (float)seam_work_aspect;
		K(0, 0) *= swa; K(0, 2) *= swa;
		K(1, 1) *= swa; K(1, 2) *= swa;

		corners[i] = warper->warp(images[i], K, cameras[i].R, INTER_LINEAR, BORDER_REFLECT, images_warped[i]);
		sizes[i] = images_warped[i].size();

		warper->warp(masks[i], K, cameras[i].R, INTER_NEAREST, BORDER_CONSTANT, masks_warped[i]); // Warps the current image
	}
	
	vector<UMat> images_warped_f(num_images);
	for (int i = 0; i < num_images; ++i) {
		images_warped[i].convertTo(images_warped_f[i], CV_32F);
		cout << images_warped[i].size() << endl;
	}

	for (size_t i = 0; i < images_warped.size(); i++) {
		images_warped_temp.push_back(images_warped[i]);
	}

	for (size_t i = 0; i < corners.size(); i++) {
		corners_temp.push_back(corners[i]);
	}

	for (size_t i = 0; i < images_warped.size(); i++) {
		masks_warped_temp.push_back(masks_warped[i]);
	}

}

vector<Point> Warping::returnCorners() {
	return corners_temp;
}

vector<UMat> Warping::returnImagesWarped() {
	return images_warped_temp;
}

vector<UMat> Warping::returnMasksWarped() {
	return masks_warped_temp;
}


Warping::~Warping() {
}
#include "Warping.h"
#include <opencv2/opencv.hpp>

Warping::Warping() {

}

Mat Warping::warp(Mat image, MatchedKeyPoint features) {
	vector_split_(features);
	return perspective_warping_(image);
}

Mat Warping::perspective_warping_(Mat &img) {
	cout << "Perspective() {" << endl;

	//double threshold = (num_features_ / 25) - 1;
	//double iterations = (0.9893 * pow(threshold, 3)) - (22.0776 * pow(threshold, 2)) + (132.1516 * threshold) - 0.1395;

	// Find homography (pixel != black)
	//Mat h = findHomography(baseImagePts_, dstPts_, RANSAC, thresHold, noArray(), iterations);
	//Mat h = findHomography(base_image_pts_, dst_pts_, RANSAC, 7);
	Mat h = findHomography(base_image_pts_, dst_pts_, LMEDS);
	//Mat h = findHomography(baseImagePts_, dstPts_, RHO, 5); // 4
	//cout << endl << "homography = " << endl << h << endl << endl;

	prev_offset_y_ = new_offset_y_;
	cout << "pre_offSetY = " << prev_offset_y_ << endl;
	// Finding the needed canvas size 
	offset_x_ = h.at<double>(0, 2);
	offSetY = h.at<double>(1, 2);
	//cout << "offSetX = " << offSetX << endl;
	//cout << "offSetY = " << offSetY << endl;

	new_offset_y_ = prev_offset_y_ + abs(offSetY);
	cout << "new_offSetY = " << new_offset_y_ << endl;

	// Use homography to warp image
	Mat warpedImage;
	warpPerspective(img, warpedImage, h, Size(img.cols, img.rows + new_offset_y_));

	cout << "}" << endl;
	return warpedImage;
}

void Warping::vector_split_(MatchedKeyPoint features) {
	cout << "vector_split_() {" << endl;

	base_image_pts_.resize(features.image_1.size());
	dst_pts_.resize(features.image_2.size());

	for (size_t i = 0; i < features.image_1.size(); i++) {
		base_image_pts_[i] = features.image_1[i];
		dst_pts_[i] = features.image_2[i];
	}

	//cout << "pts_base_image = " << endl << baseImagePts_ << endl << endl;
	//cout << "pts_dst = " << endl << dstPts_ << endl << endl;

	num_features_ = features.image_1.size();
	cout << "}" << endl;
}


// Translational warping
Mat Warping::translate(Mat img, int offsetx, int offsety) {
	//cout << "translate() {" << endl;

	Mat trans_img;
	Mat trans_mat = Mat::ones(offsety, offsetx, img.type());

	//cout << "trans img size = " << img.size() << endl;
	//cout << "trans_mat size = " << trans_mat.size() << endl;

	trans_mat = (Mat_<double>(2, 3) << 1, 0, offsetx, 0, 1, offsety);
	warpAffine(img, trans_img, trans_mat, Size(img.cols + offsetx, img.rows + offsety));

	//cout << "trans_img size = " << trans_img.size() << endl;
	//cout << "}" << endl << endl;
	return trans_img;
}

Warping::~Warping() {
}
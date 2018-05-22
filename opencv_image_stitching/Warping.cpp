#include "Warping.h"
#include <opencv2/opencv.hpp>

Warping::Warping() {
	iteration_ = 1;
}

Mat Warping::warp(Mat &image, MatchedKeyPoint features) {
	vector_split_(features);
	return perspective_warping_(image);
}

Mat Warping::perspective_warping_(Mat &img) {
	cout << "Perspective() {" << endl;

	// Finding the reprojection threshold
	vector<Point2f> dst_pts_out2;
	vector<Point2f> base_image_pts_out2;
	normalize(dst_pts_, dst_pts_out2, 1000, 0, NORM_L1, -1, noArray());
	normalize(base_image_pts_, base_image_pts_out2, 1000, 0, NORM_L1, -1, noArray());
	double dist = norm(base_image_pts_out2, dst_pts_out2);

	std::cout << "dist = " << dist << endl;

	// Finding the needed canvas size 
	vector<int> alt_offsety;
	int offsety_sum = 0;
	for (size_t i = 0; i < dst_pts_.size(); i++) {
		alt_offsety.push_back(abs(base_image_pts_[i].y - dst_pts_[i].y));
	}
	for (size_t i = 0; i < dst_pts_.size(); i++) {
		if (alt_offsety[i] > offsety_sum) {
			offsety_sum = alt_offsety[i];
		}
	}
	std::cout << "offsetRows = " << offsety_sum << endl;

	// Find homography
	//Mat h = findHomography(base_image_pts_, dst_pts_, LMEDS);
	//Mat h = findHomography(base_image_pts_, dst_pts_, RANSAC, dist);
	Mat h = findHomography(base_image_pts_, dst_pts_, RHO, dist);
	
	double offset_y = h.at<double>(1,2);
	//cout << endl << "homography = " << endl << h << endl << endl;

	// Clearing the splitted vectors
	base_image_pts_.clear();
	dst_pts_.clear();

	// Warping the image using the homography matrix
	std::cout << "Warping perspective...." << endl;
	Mat warpedImage;
	try{
	warpPerspective(img, warpedImage, h, Size(img.cols, img.rows + offsety_sum));
	}
	catch (const std::exception& e) {
		std::cout << e.what() << endl;
	}

	String output_location = "../opencv_image_stitching/Images/Results/PROSAC_dist_warped#" + to_string(iteration_) + "_0.5.jpg";
	cv::imwrite(output_location, warpedImage);

	iteration_ = iteration_ + 1;

	cout << "}" << endl;
	return warpedImage;
}

void Warping::vector_split_(MatchedKeyPoint features) {
	cout << "vector_split_() {" << endl;

	// Resizing the vectors to fit the incoming vectors
	base_image_pts_.resize(features.image_1.size());
	dst_pts_.resize(features.image_2.size());

	// Populating the vectors 
	for (size_t i = 0; i < features.image_1.size(); i++) {
		base_image_pts_[i] = features.image_2[i];
		dst_pts_[i] = features.image_1[i];
	}

	//cout << "pts_base_image = " << endl << baseImagePts_ << endl << endl;
	//cout << "pts_dst = " << endl << dstPts_ << endl << endl;
	cout << "}" << endl;
}

Mat Warping::translate(Mat &img, int offsetx, int offsety) {
	cout << "translate() {" << endl;

	Mat trans_img;
	Mat trans_mat = Mat::ones(offsety, offsetx, img.type());

	//cout << "trans img size = " << img.size() << endl;
	//cout << "trans_mat size = " << trans_mat.size() << endl;

	trans_mat = (Mat_<double>(2, 3) << 1, 0, offsetx, 0, 1, offsety);

	try {
		warpAffine(img, trans_img, trans_mat, Size(img.cols + offsetx, img.rows + offsety));
	}
	catch (const std::exception& e) {
		cout << e.what() << endl;
	}
	cout << "trans_img size = " << trans_img.size << endl;
	cout << "}" << endl;
	return trans_img;
}

Warping::~Warping() {
}
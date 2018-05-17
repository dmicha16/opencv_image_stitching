#include "Warping.h"
#include <opencv2/opencv.hpp>

Warping::Warping() {

}

Mat Warping::warp(Mat &image, MatchedKeyPoint features) {
	vector_split_(features);
	return perspective_warping_(image);
}

Mat Warping::perspective_warping_(Mat &img) {
	cout << "Perspective() {" << endl;

	vector<Point2f> dst_pts_out;
	vector<Point2f> base_image_pts_out;
	normalize(dst_pts_, dst_pts_out, 100, 0, NORM_L2, -1, noArray());
	normalize(base_image_pts_, base_image_pts_out, 100, 0, NORM_L2, -1, noArray());
	double dist1 = norm(base_image_pts_out, dst_pts_out);

	vector<Point2f> dst_pts_out1;
	vector<Point2f> base_image_pts_out1;
	normalize(dst_pts_, dst_pts_out1, 10, 0, NORM_L2, -1, noArray());
	normalize(base_image_pts_, base_image_pts_out1, 10, 0, NORM_L2, -1, noArray());
	double dist2 = norm(base_image_pts_out1, dst_pts_out1, NORM_L1);

	vector<Point2f> dst_pts_out2;
	vector<Point2f> base_image_pts_out2;
	normalize(dst_pts_, dst_pts_out2, 1000, 0, NORM_L1, -1, noArray());
	normalize(base_image_pts_, base_image_pts_out2, 1000, 0, NORM_L1, -1, noArray());
	double dist3 = norm(base_image_pts_out2, dst_pts_out2);

	//cout << "dist1 = " << dist1 << endl;
	//cout << "dist2 = " << dist2 << endl;
	//cout << "dist3 = " << dist3 << endl;

	if (iteration == 0 || dist > dist3) {
		dist = dist3;
	}
	iteration = iteration + 1;

	cout << "dist = " << dist << endl;

	// Find homography (pixel != black)
	//Mat h = findHomography(base_image_pts_, dst_pts_, LMEDS);
	//Mat h = findHomography(base_image_pts_, dst_pts_, RANSAC, dist);
	//Mat h = findHomography(base_image_pts_, dst_pts_, RANSAC, 15);
	//Mat h = findHomography(base_image_pts_, dst_pts_, RHO, dist);
	Mat h = findHomography(base_image_pts_, dst_pts_, RHO, 7);
	//cout << endl << "homography = " << endl << h << endl << endl;

	prev_offset_y_ = new_offset_y_;
	cout << "pre_offSet_y_ = " << prev_offset_y_ << endl;
	// Finding the needed canvas size 
	int offSet_y = 0;
	offSet_y = h.at<double>(1, 2);
	cout << "offSet_y = " << offSet_y << endl;
	/*if (abs(offSet_y) < 100) {
		offSet_y_ = 100;
		cout << "reviesed offSet_y_ = " << offSet_y_ << endl;
	}*/

	new_offset_y_ = (prev_offset_y_ + abs(offSet_y)); //*0.9
	cout << "new_offSet_y_ = " << new_offset_y_ << endl;
	cout << "Warping perspective...." << endl;

	// Use homography to warp image
	Mat warpedImage;
	try{
	warpPerspective(img, warpedImage, h, Size(img.cols, img.rows + new_offset_y_)); 
	}
	catch (const std::exception& e) {
		cout << e.what() << endl;
	}

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
	cout << "}" << endl;
}


// Translational warping
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
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

	double thresHold = (numFeatures / 25) - 1;
	double iterations = (0.9893 * pow(thresHold, 3)) - (22.0776 * pow(thresHold, 2)) + (132.1516 * thresHold) - 0.1395;

	cout << "thresHold = " << thresHold << endl;
	cout << "iterations = " << iterations << endl;

	// Find homography (pixel != black)
	//Mat h = findHomography(baseImagePts_, dstPts_, RANSAC, thresHold, noArray(), iterations);
	Mat h = findHomography(baseImagePts_, dstPts_, RANSAC, 7);
	//Mat h = findHomography(baseImagePts_, dstPts_, LMEDS);
	//Mat h = findHomography(baseImagePts_, dstPts_, RHO, 5); // 4
	//cout << endl << "homography = " << endl << h << endl << endl;

	pre_offSetY = new_offSetY;
	cout << "pre_offSetY = " << pre_offSetY << endl;
	// Find the needed canvas size 
	offSetX = h.at<double>(0, 2);
	offSetY = h.at<double>(1, 2);
	//cout << "offSetX = " << offSetX << endl;
	//cout << "offSetY = " << offSetY << endl;

	new_offSetY = pre_offSetY + abs(offSetY);
	cout << "new_offSetY = " << new_offSetY << endl;

	// Use homography to warp image
	Mat warpedImage;
	warpPerspective(img, warpedImage, h, Size(img.cols, img.rows + new_offSetY));

	cout << "}" << endl << endl;
	return warpedImage;
}

void Warping::vector_split_(MatchedKeyPoint features) {
	cout << endl << "vector_split_() {" << endl;

	baseImagePts_.resize(features.image_1.size());
	dstPts_.resize(features.image_2.size());

	for (size_t i = 0; i < features.image_1.size(); i++) {
		baseImagePts_[i] = features.image_1[i];
		dstPts_[i] = features.image_2[i];
	}

	//cout << "pts_base_image = " << endl << baseImagePts_ << endl << endl;
	//cout << "pts_dst = " << endl << dstPts_ << endl << endl;

	numFeatures = features.image_1.size();
	cout << "}" << endl << endl;
}


// Translational warping
Mat Warping::translate(Mat &img, int offsetx, int offsety) {
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
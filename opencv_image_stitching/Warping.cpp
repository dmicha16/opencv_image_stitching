#include "Warping.h"
#include <opencv2/opencv.hpp>

Warping::Warping() {

}

Mat Warping::warp(Mat &image, MatchedKeyPoint features) {
	vector_split_(features);
	perspective_warping_(image);
	return warpedImage;
}

void Warping::perspective_warping_(Mat &img) {
	cout << "Perspective() {" << endl;

	double thresHold = (numFeatures / 25) - 1;
	double iterations = (0.9893 * pow(thresHold, 3)) - (22.0776 * pow(thresHold, 2)) + (132.1516 * thresHold) - 0.1395;

	cout << "thresHold = " << thresHold << endl;
	cout << "iterations = " << iterations << endl;

	// Find homography
	//Mat h = findHomography(baseImagePts_, dstPts_, RANSAC, thresHold, noArray(), iterations);
	Mat h = findHomography(baseImagePts_, dstPts_, LMEDS); 
	//Mat h = findHomography(baseImagePts_, dstPts_, RHO, 10);
	//cout << endl << "homography = " << endl << h << endl << endl;
	
	//pre_offSetY = abs(offSetY) + pre_offSetY;
	pre_offSetY = new_offSetY;
	cout << "pre_offSetY = " << pre_offSetY << endl;
	// Find the needed canvas size 
	offSetX = h.at<double>(0, 2);
	offSetY = h.at<double>(1, 2);
	//cout << "offSetX = " << offSetX << endl;
	//cout << "offSetY = " << offSetY << endl;

	//cout << "img.rows = " << img.rows << endl;
	new_offSetY = img.rows - (pre_offSetY + abs(offSetY));
	cout << "new_offSetY = " << new_offSetY << endl;


	// Use homography to warp image
	//warpPerspective(img, warpedImage, h, Size(img.cols, img.rows + abs(offSetY) + new_offSetY));
	warpPerspective(img, warpedImage, h, Size(img.cols, img.rows + new_offSetY+40));

	//cout << "img.size() = " << img.size() << endl;
	//cout << "warpedImage.size() = " << warpedImage.size() << endl <<endl;
	
	cout << "}" << endl << endl;
}

void Warping::vector_split_(MatchedKeyPoint features) {
	cout << endl << "vector_split_() {" << endl;

	baseImagePts_.resize(features.image_1.size());
	dstPts_.resize(features.image_2.size());
	
	for (size_t i = 0; i < features.image_1.size(); i++) {
		baseImagePts_[i] = features.image_1[i];
		dstPts_[i] = features.image_2[i];
	}
	

	numFeatures = features.image_1.size();
	//cout << "pts_base_image = " << endl << baseImagePts_ << endl << endl;
	//cout << "pts_dst = " << endl << dstPts_ << endl << endl;
	cout << "}" << endl << endl;
}

Warping::~Warping() {
}
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
	cout << "Perspective() {" << endl << endl;

	// Find homography
	//Mat h = findHomography(baseImagePts_, dstPts_, RANSAC, 16);
	//Mat h = findHomography(baseImagePts_, dstPts_, LMEDS, 3); // This one is pretty good
	Mat h = findHomography(baseImagePts_, dstPts_, RHO, 1); // This one is pretty very good
	//Mat h = findHomography(baseImagePts_, dstPts_, 0, 3);
	//cout << endl << "homography = " << endl << h << endl << endl;
	

	// Find the needed canvas size 
	// There is still some offset error but I think that for our case this will be more then fine, since this warping example is extrem.  
	int offSetX = h.at<double>(0, 2);
	int offSetY = h.at<double>(1, 2);
	cout << "offSetX = " << offSetX << endl;
	cout << "offSetY = " << offSetY << endl;

	
	// Use homography to warp image
	//warpPerspective(img, warpedImage, h, Size(img.cols + abs(offSetX), img.rows + abs(offSetY)+50));
	warpPerspective(img, warpedImage, h, Size(img.cols, img.rows + abs(offSetY)));
	
	cout << "img.size() = " << img.size() << endl;
	cout << "warpedImage.size() = " << warpedImage.size() << endl <<endl;
	
	cout << "}" << endl << endl;

}

void Warping::vector_split_(MatchedKeyPoint features) {
	cout << endl << "vector_split_() {" << endl << endl;

	baseImagePts_.resize(features.image_1.size());
	dstPts_.resize(features.image_2.size());
	
	for (size_t i = 0; i < features.image_1.size(); i++) {
		baseImagePts_[i] = features.image_1[i];
		dstPts_[i] = features.image_2[i];
	}

	//cout << "pts_base_image = " << endl << baseImagePts_ << endl << endl;
	//cout << "pts_dst = " << endl << dstPts_ << endl << endl;
	cout << "}" << endl << endl;
}

Warping::~Warping() {
}
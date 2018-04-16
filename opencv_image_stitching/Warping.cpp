#include "Warping.h"
#include <opencv2/opencv.hpp>

Warping::Warping(vector<CameraParams> cameras, vector<Mat> images) {

}

void Warping::warp(Mat &image, vector<Point2f> &features) {
	vector_split_(features);
	perspective_(image);

}

void Warping::perspective_(Mat &img) {
	cout << "Perspective() {" << endl << endl;

	cout << "pts_base_image = " << endl << baseImagePts_ << endl << endl;
	cout << "pts_dst = " << endl << dstPts_ << endl << endl;

	// Find homography
	Mat h = findHomography(baseImagePts_, dstPts_, RANSAC, 3);
	cout << '\n' << "homography = " << endl << h << endl << endl;

	// Find the needed canvas size 
	// There is still some offset error but I think that for our case this will be more then fine, since this warping example is extrem.  
	int offSetX = h.at<double>(0, 2);
	int offSetY = h.at<double>(1, 2);
	cout << "offSetX = " << offSetX << endl;
	cout << "offSetY = " << offSetY << endl;
	cout << "}" << endl << endl;

	// Use homography to warp image
	warpPerspective(img, warpedImage, h, Size(img.cols + offSetX * 1.5, img.rows)); // The offSetX *1.5 (1.5 is just an arbitrary number)  

}

void Warping::vector_split_(vector<Point2f> features[2][]) {
	int matLength = size(features[0]);
	//cout << "matLength = " << matLength << endl << endl;

	for (int i = 0; i < matLength; i++) {
		//cout << "mat[0][" << i << "] = " << endl << mat[0][i] << endl;
		//cout << "mat[1][" << i << "] = " << endl << mat[1][i] << endl << endl;
		baseImagePts_.push_back(features[0][i]);
		dstPts_.push_back(features[1][i]);
	}
}

Warping::~Warping() {
}
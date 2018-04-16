#include "Warping.h"
#include <opencv2/opencv.hpp>

Warping::Warping() {

}

void Warping::warp(Mat &image, Point2f features[][2]) {
	vector_split_(features);
	perspective_warping_(image);
}

void Warping::perspective_warping_(Mat &img) {
	cout << "Perspective() {" << endl << endl;

	// Find homography
	Mat h = findHomography(baseImagePts_, dstPts_, RANSAC, 3);
	cout << endl << "homography = " << endl << h << endl << endl;

	// Find the needed canvas size 
	// There is still some offset error but I think that for our case this will be more then fine, since this warping example is extrem.  
	int offSetX = h.at<double>(0, 2);
	int offSetY = h.at<double>(1, 2);
	cout << "offSetX = " << offSetX << endl;
	cout << "offSetY = " << offSetY << endl;

	// Use homography to warp image
	warpPerspective(img, warpedImage, h, Size(img.cols + offSetX * 1.5, img.rows)); // The offSetX *1.5 (1.5 is just an arbitrary number)  

	cout << "}" << endl << endl;
}

void Warping::vector_split_(Point2f features[][2]) {
	cout << "vector_split_() {" << endl << endl;

	int i = 0;
	while (features[i][0].x > 0) {
		baseImagePts_.push_back(features[i][0]);
		dstPts_.push_back(features[i][1]);
		i++;
	}
	cout << "Number of pairwise matches = " << i << endl << endl;
	cout << "pts_base_image = " << endl << baseImagePts_ << endl << endl;
	cout << "pts_dst = " << endl << dstPts_ << endl << endl;
	cout << "}" << endl << endl;
}

Warping::~Warping() {
}
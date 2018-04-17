#include "Warping.h"
#include <opencv2/opencv.hpp>

Warping::Warping() {

}

void Warping::warp(Mat &image, MatchedKeyPoint features) {
	vector_split_(features);
	perspective_warping_(image);
}

void Warping::perspective_warping_(Mat &img) {
	cout << "Perspective() {" << endl << endl;

	// Find homography
	//Mat h = findHomography(baseImagePts_, dstPts_, RANSAC, 3);
	Mat h = findHomography(baseImagePts_, dstPts_, 0, 3);
	//cout << endl << "homography = " << endl << h << endl << endl;
	/*
	Point2f srcpts[4];
	Point2f dstpts[4];

	for (size_t i = 0; i < 4; i++) {
		srcpts[i] = baseImagePts_[i];
		dstpts[i] = dstPts_[i];
	}
	*/
	//Mat h = getPerspectiveTransform(srcpts, dstpts);

	// Find the needed canvas size 
	// There is still some offset error but I think that for our case this will be more then fine, since this warping example is extrem.  
	int offSetX = h.at<double>(0, 2);
	int offSetY = h.at<double>(1, 2);
	cout << "offSetX = " << offSetX << endl;
	cout << "offSetY = " << offSetY << endl;

	// Use homography to warp image
	//warpPerspective(img, warpedImage, h, Size(img.cols + offSetX, img.rows + offSetY)); // The offSetX *1.5 (1.5 is just an arbitrary number)  
	warpPerspective(img, warpedImage, h, Size(img.cols, img.rows));
	//warpAffine(img, warpedImage, warpMat, Size(img.cols + offSetX, img.rows));
	Mat3b res(img.rows + offSetY, img.cols + offSetX, Vec3b(0, 0, 0));
	try
	{
		warpedImage.copyTo(res(Rect(0, 0, warpedImage.cols, warpedImage.rows)));
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
		WINPAUSE;
	}

	//Mat3b res(img.rows + offSetY, img.cols + offSetX, Vec3b(0,0,0));
	//warpedImage.copyTo(res(Rect(0, 0, warpedImage.cols, warpedImage.rows)));

	cout << "}" << endl << endl;
	//imshow("warped image", warpedImage);
	String output_location = "../opencv_image_stitching/Images/Results/test_warp.jpg";
	imwrite(output_location, res);
	WINPAUSE;
}

void Warping::vector_split_(MatchedKeyPoint features) {
	cout << "vector_split_() {" << endl << endl;

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
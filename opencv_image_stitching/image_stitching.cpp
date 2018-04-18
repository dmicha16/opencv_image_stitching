#include "Wrapper.h"
#include "FeatureFindMatch.h"
#include "ImageReader.h"
#include "Undistorter.h"
#include "Warping.h"
#include "Stitcher.h"



int main() {
	INIT_CLOGGING;

	ADD_FILE("clogging.log");
	cv::ocl::setUseOpenCL(false);

	CLOG("test");

	/******************************************* Reader *******************************************/
	
	ImageReader image_reader;
	vector<Mat> raw_images = image_reader.get_images();
	


	/**************************************** UNDISTORTION *****************************************/
	
	//Undistorter undistorter;
	//vector<Mat> undist_images = undistorter.undistort_images(raw_images);
	
	

	/****************************************** FEATURES *******************************************/
	
	FeatureFindMatch finder;
	finder.find_features(raw_images);
	MatchedKeyPoint matched_key_points = finder.get_matched_coordinates();
	
	Mat raw;
	resize(raw_images[1], raw, cvSize(0, 0), 0.5, 0.5);
	imshow("base image", raw);
	/******************************************* WARPING *******************************************/

	Warping warper;
	Mat warpedImg = warper.warp(raw_images[0], matched_key_points);

	Mat warp;
	resize(warpedImg, warp, cvSize(0, 0), 0.5, 0.5);
	imshow("warped image", warp);
	/****************************************** Stitching *******************************************/

	Stitcher stitcher;
	Mat stitchedImg = stitcher.merging(raw_images[1], warpedImg);

	Mat stitched;
	resize(stitchedImg, stitched, cvSize(0, 0), 0.5, 0.5);
	imshow("Stitched image", stitched);
	waitKey(0);
}
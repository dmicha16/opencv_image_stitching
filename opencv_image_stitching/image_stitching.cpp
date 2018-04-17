#include "Wrapper.h"
#include "FeatureFindMatch.h"
#include "ImageReader.h"
#include "Undistorter.h"
#include "Warping.h"



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
	
	

	/******************************************* WARPING *******************************************/
	Warping warper;
	warper.warp(raw_images[1], matched_key_points);
	
	waitKey(0);
}
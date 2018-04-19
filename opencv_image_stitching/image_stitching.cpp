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

	/**************************************** CONSTRUCTORS *****************************************/

	//Undistorter undistorter;
	FeatureFindMatch finder;
	Warping warper;
	Stitcher stitcher;
	ImageReader image_reader;



	/******************************************* READER *******************************************/
	
	vector<Mat> raw_images = image_reader.get_images();
	


	/**************************************** UNDISTORTION *****************************************/
	
	//vector<Mat> undist_images = undistorter.undistort_images(raw_images);



	/*************************************** STITCHING LOOP ****************************************/

	Mat stitched;
	Mat stitchedImg;

	vector<Mat> images_to_stitch;
	images_to_stitch.resize(2);
	
	for (size_t i = 0; i < 2; i++) {
		images_to_stitch[i] = raw_images[i];
	}

	

	for (size_t i = 0; i < raw_images.size() - 1; i++) {

		/****************************************** FEATURES *******************************************/

		finder.find_features(images_to_stitch);
		MatchedKeyPoint matched_key_points = finder.get_matched_coordinates();

		//Mat raw;
		//resize(images_to_stitch[1], raw, cvSize(0, 0), 0.5, 0.5);
		/*imshow("base image", raw);
		WINPAUSE;*/



		/******************************************* WARPING *******************************************/

		Mat warpedImg = warper.warp(images_to_stitch[0], matched_key_points);

		Mat warp;
		resize(warpedImg, warp, cvSize(0, 0), 0.5, 0.5);
		String name = "warped image." + i;
		imshow(to_string(i) , warp);
		//WINPAUSE;



		/****************************************** STITCHING *******************************************/

		stitchedImg = stitcher.merging(images_to_stitch[1], warpedImg);

		cout << endl << "Iteration number: " << i << endl;
		//WINPAUSE;

		if (i < (raw_images.size() - 2)) {
			cout << "I AM THE IF MOTHERFUCKER" << endl;
			images_to_stitch[0] = stitchedImg;
			images_to_stitch[1] = raw_images[i + 2];
			//break;
		}
	}

	Mat raw;
	resize(raw_images[0], raw, cvSize(0, 0), 0.5, 0.5);
	imshow("first image", raw);
	/*resize(stitchedImg, stitched, cvSize(0, 0), 0.75, 0.75);
	imshow("Stitched image", stitched);*/
	//imshow("Stitched image", stitchedImg);
	String output_location = "../opencv_image_stitching/Images/Results/test_2.jpg";
	imwrite(output_location, stitchedImg);
	cout << endl << "------------ MISSION COMPLETE ------------" << endl;
	waitKey(0);
}
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

	Stitcher stitcher;
	Warping warper;
	FeatureFindMatch finder;
	ImageReader image_reader;

	/******************************************* Reader *******************************************/

	vector<Mat> raw_images = image_reader.get_images();



	/**************************************** UNDISTORTION *****************************************/

	//Undistorter undistorter;
	//vector<Mat> undist_images = undistorter.undistort_images(raw_images);

	vector<Mat> images_to_stitch;
	images_to_stitch.resize(2);

	Mat stitchedImg;

	for (size_t i = 0; i < 2; i++) {
		images_to_stitch[i] = raw_images[i];
	}



	for (size_t i = 0; i < raw_images.size() - 1; i++) {
		/****************************************** FEATURES *******************************************/

		images_to_stitch[0] = warper.translate(images_to_stitch[0], 350, 0);
		Mat img1;
		resize(images_to_stitch[0], img1, cvSize(0, 0), 0.3, 0.3);
		string name1 = "images_to_stitch[0]" + to_string(i);
		//imshow(name1, img1);

		images_to_stitch[1] = warper.translate(images_to_stitch[1], 350, 0);
		Mat img2;
		resize(images_to_stitch[1], img2, cvSize(0, 0), 0.3, 0.3);
		string name2 = "images_to_stitch[1]" + to_string(i);
		//imshow(name2, img2);

		//finder.find_features(raw_images);
		finder.find_features(images_to_stitch);
		MatchedKeyPoint matched_key_points = finder.get_matched_coordinates();

		Mat raw;
		resize(images_to_stitch[1], raw, cvSize(0, 0), 0.4, 0.4);
		string base_name = "Base image" + to_string(i);
		//imshow(base_name, raw);
		/******************************************* WARPING *******************************************/
		
		WINPAUSE;

		Mat warpedImg = warper.warp(images_to_stitch[0], matched_key_points);

		Mat warp;
		resize(warpedImg, warp, cvSize(0, 0), 0.4, 0.4);
		string warp_name = "warped image" + to_string(i);
		//imshow(warp_name, warp);
		/****************************************** STITCHING *******************************************/

		stitchedImg = stitcher.customMerger(images_to_stitch[1], warpedImg);
		//stitchedImg = stitcher.merging(images_to_stitch[1], warpedImg);

		cout << endl << "Number of Iteration: " << i + 1 << endl;

		if (i < (raw_images.size() - 2)) {
			images_to_stitch.empty(); ///////////////
			images_to_stitch[0] = stitchedImg;
			images_to_stitch[1] = raw_images[i + 2];

			stitchedImg.empty();
			Mat inter;
			resize(images_to_stitch[0], inter, cvSize(0, 0), 0.4, 0.4);
			string int_name = "inter image" + to_string(i);
			//imshow(int_name, inter);
			//break;
		}

	}
	Mat stitched;
	resize(stitchedImg, stitched, cvSize(0, 0), 0.3, 0.3);
	imshow("Stitched image", stitched);


	cout << endl << "------------ MISSION COMPLETE ------------" << endl;
	waitKey(0);
}
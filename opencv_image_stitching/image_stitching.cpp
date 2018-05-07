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

	Mat stitched_img;

	for (size_t i = 0; i < 2; i++) {
		images_to_stitch[i] = raw_images[i];
	}

	for (size_t i = 0; i < raw_images.size() - 1; i++) {

		/************************************** INIT CURRENT IMAGES *************************************/
		cout << "first image size - " << images_to_stitch[0].size << endl;
		cout << "second image size - " << images_to_stitch[1].size << endl;
		//images_to_stitch[0] = warper.translate(images_to_stitch[0], 350, 0);
		//images_to_stitch[1] = warper.translate(images_to_stitch[1], 350, 0);

#pragma region output_current_images
		//Mat img1, img2, raw;
		//resize(images_to_stitch[0], img1, cvSize(0, 0), 0.3, 0.3);
		//string name1 = "images_to_stitch[0]" + to_string(i);
		//imshow(name1, img1);		
		
		//resize(images_to_stitch[1], img2, cvSize(0, 0), 0.3, 0.3);
		//string name2 = "images_to_stitch[1]" + to_string(i);
		//imshow(name2, img2);		
		//resize(images_to_stitch[1], raw, cvSize(0, 0), 0.4, 0.4);
		//string base_name = "Base image" + to_string(i);
		//imshow(base_name, raw);
#pragma endregion //output_current_images

		/************************************** FEATURES *************************************/

		int rows = 3, columns = 3, desired_occupied_rect = 4;
		float threshold = 0.25, image_overlap = 0.95;		
		finder.set_rectangle_info(rows, columns, image_overlap, desired_occupied_rect);
		finder.find_features(images_to_stitch, threshold);
		
		MatchedKeyPoint matched_key_points = finder.get_matched_coordinates();

		/******************************************* WARPING *******************************************/

		Mat warped_img = warper.warp(images_to_stitch[0], matched_key_points);
		/*Mat warp;
		resize(warped_img, warp, cvSize(0, 0), 0.4, 0.4);
		string warp_name = "warped image" + to_string(i);*/
		//imshow(warp_name, warp);
		/****************************************** STITCHING *******************************************/

		stitched_img = stitcher.customMerger(images_to_stitch[1], warped_img);
		//stitched_img = stitcher.merging(images_to_stitch[1], warped_img);

		cout << endl << "Number of Iteration: " << i + 1 << endl;

		if (i < (raw_images.size() - 2)) {
			images_to_stitch.empty();
			images_to_stitch[0] = stitched_img;
			images_to_stitch[1] = raw_images[i + 2];

			stitched_img.empty();
			/*Mat inter;
			resize(images_to_stitch[0], inter, cvSize(0, 0), 0.4, 0.4);
			string int_name = "inter image" + to_string(i);*/
			//imshow(int_name, inter);
			//break;
		}

	}
	Mat stitched;
	//resize(stitched_img, stitched, cvSize(0, 0), 0.3, 0.3);
	//imshow("Stitched image", stitched);
	String output_location = "../opencv_image_stitching/Images/Results/result4.jpg";
	imwrite(output_location, stitched_img);


	cout << endl << "------------ MISSION COMPLETE ------------" << endl;
	waitKey(0);
}
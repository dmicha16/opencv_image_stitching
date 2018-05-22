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
	Undistorter undistorter;

	/******************************************* Reader *******************************************/

	vector<Mat> raw_images = image_reader.get_images();

	/**************************************** UNDISTORTION *****************************************/

	vector<Mat> undist_images = undistorter.undistort_images(raw_images);
	for (size_t i = 0; i < raw_images.size(); i++)
	{
		raw_images[i].release();
	}

	vector<Mat> images_to_stitch;
	images_to_stitch.resize(2);

	Mat stitched_img;

	for (size_t i = 0; i < 2; i++) {
		images_to_stitch[i] = undist_images[i];
	}

	for (size_t i = 0; i < undist_images.size() - 1; i++) {

		/************************************** INIT CURRENT IMAGES *************************************/
		std::cout << endl << "Number of Iteration: " << i + 1 << endl;

		std::cout << "first image size - " << images_to_stitch[0].size << endl;
		std::cout << "second image size - " << images_to_stitch[1].size << endl;

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
		float threshold = 0.5, image_overlap = 0.5;		
		finder.set_rectangle_info(rows, columns, image_overlap, desired_occupied_rect);
		finder.set_images(images_to_stitch);
		finder.find_features(threshold, i);
		
		MatchedKeyPoint matched_key_points = finder.get_matched_coordinates();

		/******************************************* WARPING *******************************************/

		Mat warped_img = warper.warp(images_to_stitch[1], matched_key_points);
		
		/****************************************** STITCHING *******************************************/

		stitched_img = stitcher.customMerger(images_to_stitch[0], warped_img);


		if (i < (raw_images.size() - 2)) {
			images_to_stitch[0].release();
			images_to_stitch[1].release();

			images_to_stitch[0] = stitched_img;
			images_to_stitch[1] = undist_images[i + 2];

			String output_location = "../opencv_image_stitching/Images/Results/PROSAC_dist_intermediary#" + to_string(i+1) + "_0.5.jpg";
			cv::imwrite(output_location, stitched_img);

			stitched_img.release();
		}
	}

	String output_location = "../opencv_image_stitching/Images/Results/PROSAC_dist_0.5.jpg";
	cv::imwrite(output_location, stitched_img);

	std::cout << endl << "------------ MISSION COMPLETE ------------" << endl;
	WINPAUSE;
	cv::waitKey(0);
}
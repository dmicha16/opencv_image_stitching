#include "stdafx.h"
#include "RoiCalculator.h"

RoiCalculator::RoiCalculator() {
}

RoiCalculator::~RoiCalculator() {
}

void RoiCalculator::calculate_roi(int desired_rect) {

	rect_s_.desginate_rectengales(desired_rect);
	num_rect_ = rect_s_.rectengales.size();
	Mat current_img;
	int img_width;
	int img_height;	
	Point2f img_coords;
	img_coords.y = 0;

	for (size_t i = 0; i < images_.size(); i++) {
		current_img = images_[i];

		img_width = current_img.cols;
		img_height = current_img.rows;		

		for (size_t i = 0; i < (num_rect_ / 2); i++) {
			rect_s_.rectengales[i].x = 0;
			rect_s_.rectengales[i].y = img_coords.y;

			rect_s_.rectengales[i].width = img_width / 2;
			rect_s_.rectengales[i].height = img_height / num_rect_;

			img_coords.y += img_height / num_rect_;
		}

		img_coords.y = 0;

		for (size_t i = (num_rect_ / 2); i < num_rect_; i++) {
			rect_s_.rectengales[i].x = img_width / 2;
			rect_s_.rectengales[i].y = img_coords.y;

			rect_s_.rectengales[i].width = img_width / 2;
			rect_s_.rectengales[i].height = img_height / num_rect_;

			img_coords.y += img_height / num_rect_;
		}
		//check_keypoint();
	}
	
	//write_roi(current_img);
	
}

bool RoiCalculator::check_keypoint() {

	for (size_t i = 0; i < num_images_; i++) {
		for (size_t j = 0; j < num_rect_; j++) {

			if (i == 1) {
				for (size_t k = 0; k < matched_keypoints_.image_1.size(); k++) {
					if (rect_s_.rectengales[j].contains(matched_keypoints_.image_1[k])) {
						filtered_keypoints_.image_1.push_back(matched_keypoints_.image_1[k]);
					}
				}
			}
			else {
				for (size_t k = 0; i < matched_keypoints_.image_2.size(); k++) {

				}
			}
		}
	}
	

	return false;
}

void RoiCalculator::write_roi(Mat curr_img) {
	string output_location;
	Mat current_rect;

	for (size_t i = 0; i < rect_s_.rectengales.size(); i++) {
		current_rect  = curr_img(rect_s_.rectengales[i]);
		output_location = "../opencv_features_benchmark/Images/ROI/rect_" + to_string(i) + ".jpg";
		try {
			imwrite(output_location, current_rect);
		}
		catch (const std::exception& e) {
			cout << e.what() << endl;
			WINPAUSE;
		}		
		current_rect.empty();
		output_location = "../opencv_features_benchmark/Images/ROI/rect_";
	}
}

void RoiCalculator::set_images(vector<Mat> inc_images) {
	images_.resize(inc_images.size());
	num_images_ = inc_images.size();
	for (size_t i = 0; i < inc_images.size(); i++) {
		images_[i] = inc_images[i];
	}
}

void RoiCalculator::set_matched_keypoints(MatchedKeyPointCopy inc_matched_keypoints) {
	matched_keypoints_ = inc_matched_keypoints;
}

void Rectengales::desginate_rectengales(int desired_rect) {

	if (desired_rect % 2 != 0)
		desired_rect = desired_rect + 1;
	
	Rect rect_filler;		
	for (size_t i = 0; i < desired_rect; i++)
		rectengales.push_back(rect_filler);	
}

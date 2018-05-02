#include "stdafx.h"
#include "RoiCalculator.h"

RoiCalculator::RoiCalculator() {
}

RoiCalculator::~RoiCalculator() {
}

void RoiCalculator::calculate_roi(int desired_cols, int desired_rows, float overlap) {

	rect_s_.desginate_rectengales(desired_cols * desired_rows);
	num_rect_ = rect_s_.rectengales.size();

	//number of pixels
	const unsigned int img_width = image_.cols;
	const unsigned int img_height = image_.rows;
	vector<RowDefiner> row_definitions_(desired_rows);
	vector<int> starting_row_heights(desired_rows);

	int height_offset = img_height * overlap / desired_rows;

	for (size_t i = 0; i < starting_row_heights.size(); i++) {
		starting_row_heights[i] = img_height - height_offset * i;
	}

	for (size_t i = 0; i < row_definitions_.size(); i++) {
		row_definitions_[i] = populate_row_definer_(img_width, starting_row_heights[i], (height_offset * i));
	}

	//take image and display all points


	/*for (size_t i = 0; i < (num_rect_ / 2); i++) {
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
	}*/
	//check_keypoint();

	
	//write_roi(image_);
	
}

bool RoiCalculator::check_keypoint() {

	for (size_t i = 0; i < num_images_; i++) {
		for (size_t j = 0; j < num_rect_; j++) {
			if (i == 1) {
				for (size_t k = 0; k < matched_keypoints_.image_1.size(); k++) {
					if (rect_s_.rectengales[j].contains(matched_keypoints_.image_1[k])) {
						
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

RowDefiner RoiCalculator::populate_row_definer_(int img_width, int start_height, int offset) {

	RowDefiner row_defition;
	
	const Vec3b kBLACKPIXEL = {0, 0, 0};
	bool noise = true;

	for (int pixel_idx = 0; pixel_idx < img_width; pixel_idx++) {
		if (image_.at<Vec3b>(start_height, pixel_idx) != kBLACKPIXEL) {
			for (size_t j = 1; j <= 5; j++) {
				if (image_.at<Vec3b>(start_height, (pixel_idx + j)) != kBLACKPIXEL)
					noise = false;
				else {
					noise = true;
					break;
				}
			}
			if (!noise) {
				row_defition.left.x = pixel_idx;
				row_defition.left.y = start_height;
				row_defition.top_left.y = start_height - offset;
				row_defition.top_left.x = pixel_idx;
				break;
			}
		}
	}

	for (int pixel_idx = img_width - 1; pixel_idx >= 0; pixel_idx--) {
		if (image_.at<Vec3b>(start_height, pixel_idx) != kBLACKPIXEL) {
			for (size_t j = 1; j <= 5; j++) {
				if (image_.at<Vec3b>(start_height, (pixel_idx + j)) == kBLACKPIXEL)
					noise = false;
				else {
					noise = true;
					break;
				}
			}
			if (!noise) {
				row_defition.right.x = pixel_idx;
				row_defition.right.y = start_height;				
				break;
			}
		}
	}

	
	return row_defition;
}

void RoiCalculator::set_image(Mat inc_image) {
	image_ = inc_image;
}

void RoiCalculator::set_matched_keypoints(MatchedKeyPoint inc_matched_keypoints) {
	matched_keypoints_ = inc_matched_keypoints;
}

void Rectengales::desginate_rectengales(int desired_rect) {

	if (desired_rect % 2 != 0)
		desired_rect = desired_rect + 1;
	
	Rect rect_filler;		
	for (size_t i = 0; i < desired_rect; i++)
		rectengales.push_back(rect_filler);	
}

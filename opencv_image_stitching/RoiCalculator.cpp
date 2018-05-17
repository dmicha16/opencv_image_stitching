#include "stdafx.h"
#include "RoiCalculator.h"

RoiCalculator::RoiCalculator() {
}

RoiCalculator::~RoiCalculator() {
}

void RoiCalculator::calculate_roi(int desired_cols, int desired_rows, float overlap) {

	rectangles_s_.desginate_rectangles(desired_cols * desired_rows);
	num_rect_ = rectangles_s_.rectangles.size();
	
	const unsigned int img_width = image_.cols - 1;
	const unsigned int img_height = image_.rows - 1;
	row_definitions_.resize(desired_rows);
	vector<int> starting_row_heights(desired_rows);

	int height_offset = img_height * overlap / desired_rows;

	for (size_t i = 0; i < starting_row_heights.size(); i++) {
		starting_row_heights[i] = img_height - height_offset * i;
		//cout << "starting_row_heights " << starting_row_heights[i] << endl;
	}

	for (size_t i = 0; i < row_definitions_.size(); i++) {	

		row_definitions_[i] = populate_row_definer_(img_width, starting_row_heights[i], height_offset);
		//cout << "starting_row_heights " << starting_row_heights[i] << endl;
	}

	rectangles_s_.row_definitions = row_definitions_;
	rectangles_s_.populate_rectangles(height_offset, desired_cols);
	float min_height = (img_height * overlap);
	//write_roi_(min_height);
}

bool RoiCalculator::check_keypoint() {

	for (size_t i = 0; i < num_images_; i++) {
		for (size_t j = 0; j < num_rect_; j++) {
			if (i == 1) {
				for (size_t k = 0; k < matched_keypoints_.image_1.size(); k++) {
					if (rectangles_s_.rectangles[j].contains(matched_keypoints_.image_1[k])) {
						
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

void RoiCalculator::write_roi_(float min_height) {

	String output_location = "../opencv_image_stitching/Images/Results/roi.jpg"; 
	Point coord_to_display;

	for (size_t i = 0; i < rectangles_s_.rectangles.size(); i++)
		rectangle(image_, rectangles_s_.rectangles[i], Scalar(0, 0, 255), 3, LINE_8, 0);	
	
	for (size_t i = 0; i < matched_keypoints_.image_1.size(); i++) {
		coord_to_display.x = matched_keypoints_.image_1[i].x;
		coord_to_display.y = matched_keypoints_.image_1[i].y;		
		
		if (coord_to_display.y > (image_.rows - min_height))
			drawMarker(image_, coord_to_display, Scalar(0, 0, 255), 100, 50, LINE_8);			
		else
			drawMarker(image_, coord_to_display, Scalar(0, 255, 0), 100, 50, LINE_8);

		//cout << "ELSE display coords : (" << matched_keypoints_.image_1[i].x << ", " << matched_keypoints_.image_1[i].y << ")" << endl;
		coord_to_display.x = 0;
		coord_to_display.y = 0;
	}
	
	imwrite(output_location, image_);		
}

int RoiCalculator::num_occupied_rects() {

	int num_occupied_rects = 0;
	Point2f coord_to_check;

	for (size_t i = 0; i < rectangles_s_.rectangles.size(); i++) {
		for (size_t j = 0; j < matched_keypoints_.image_1.size(); j++) {
			coord_to_check.x = matched_keypoints_.image_1[j].x;
			coord_to_check.y = matched_keypoints_.image_1[j].y;
			if (rectangles_s_.rectangles[i].contains(coord_to_check)) {
				num_occupied_rects++;
				break;
			}
		}
	}

	rectangles_s_.reset_rectangles();
	return num_occupied_rects;
}

RowDefiner RoiCalculator::populate_row_definer_(int img_width, unsigned int start_height, int offset) {

	RowDefiner row_defition;
	const Vec3b kBLACKPIXEL = { 0, 0, 0 };

	/*cout << "offset: " << offset << endl;
	cout << "img_width: " << img_width << endl;
	cout << "start_height: " << start_height << endl;*/

	int noise_pixel_count = 0;

	row_defition.left.y = start_height;
	row_defition.right.y = start_height;

	//image_.at<Vec3b>(row, col) <-- at most importance

	for (int pixel_idx = 0; pixel_idx < img_width; pixel_idx++) {
		if (image_.at<Vec3b>(start_height, pixel_idx) != kBLACKPIXEL) {
			noise_pixel_count = 0;
			for (int j = 1; j <= 5; j++) {
				if (image_.at<Vec3b>(start_height, (pixel_idx + j)) != kBLACKPIXEL) {
					//cout << "pixel idx: " << pixel_idx + j << endl;
					noise_pixel_count++;
				}	
			}
			if (noise_pixel_count == 5) {
				row_defition.left.x = pixel_idx;
				row_defition.top_left.y = start_height - offset;
				row_defition.top_left.x = pixel_idx;
				noise_pixel_count = 0;
				break;
			}	
		}
	}

	//image_.at<Vec3b>(row, col) <-- at most importance

	for (int pixel_idx = img_width; pixel_idx > 0; pixel_idx--) {
		if (image_.at<Vec3b>(start_height, pixel_idx) != kBLACKPIXEL) {
			noise_pixel_count = 0;
			for (int j = 1; j <= 5; j++) {
				if (image_.at<Vec3b>(start_height, (pixel_idx - j)) != kBLACKPIXEL)
					noise_pixel_count++;
			}
			if (noise_pixel_count == 5) {
				row_defition.right.x = pixel_idx;
				break;
			}
		}
	}

	/*cout << "row def i: " << row_defition.left.x << endl;
	cout << "row def i: " << row_defition.left.y << endl;*/

	return row_defition;
}

void RoiCalculator::set_image(Mat inc_image) {
	image_ = inc_image;
}

void RoiCalculator::set_matched_keypoints(MatchedKeyPoint inc_matched_keypoints) {
	matched_keypoints_ = inc_matched_keypoints;
}

void Rectengales::populate_rectangles(int height_offset, int desired_cols) {
	
	int start_x = 0;

	for (size_t j = 0; j < row_definitions.size(); j++) {
		for (size_t i = j * desired_cols; i < (j * desired_cols + desired_cols); i++) {
			rectangles[i].width = (row_definitions[j].right.x - row_definitions[j].left.x) / desired_cols;
			rectangles[i].height = height_offset;
			 
			 if (i == j * desired_cols) {
				 start_x = row_definitions[j].top_left.x;
			 }

			 rectangles[i].x = start_x;
			 rectangles[i].y = row_definitions[j].top_left.y;

			 start_x += rectangles[i].width;

			//cout << "rect coords : (" << row_definitions[j].top_left.x << ", " << row_definitions[j].top_left.y << ")" << endl;
		}
		start_x = 0;
	}	
}

void Rectengales::reset_rectangles() {	
	rectangles.clear();
}

void Rectengales::desginate_rectangles(int desired_rect) {

	if (desired_rect % 2 != 0)
		desired_rect = desired_rect + 1;
	
	Rect rect_filler;	
	for (size_t i = 0; i < desired_rect; i++)
		rectangles.push_back(rect_filler);	
}

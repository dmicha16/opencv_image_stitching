#include "stdafx.h"
#include "RoiCalculator.h"

RoiCalculator::RoiCalculator() {
}

RoiCalculator::~RoiCalculator() {
}

void RoiCalculator::calculate_roi(int desired_rect) {

	rect_s_.desginate_rectengales(desired_rect);

	Mat current_img;
	cout << images_.size() << endl;
	current_img = images_[0];

	/*for (size_t i = 0; i < images_.size(); i++)
		current_img = images_[i];*/	
	
	int img_width = current_img.cols;
	int img_height = current_img.rows;
	int num_rect = rect_s_.rectengales.size();
	Point2f img_coords;
	img_coords.y = 0;

	for (size_t i = 0; i < (num_rect / 2); i++) {
		rect_s_.rectengales[i].x = 0;
		rect_s_.rectengales[i].y = img_coords.y;

		rect_s_.rectengales[i].width = img_width / 2;
		rect_s_.rectengales[i].height = img_height / num_rect;

		img_coords.y += img_height / num_rect;
	}
	
	img_coords.y = 0;

	for (size_t i = (num_rect / 2); i < num_rect; i++) {
		rect_s_.rectengales[i].x = img_width / 2;
		rect_s_.rectengales[i].y = img_coords.y;

		rect_s_.rectengales[i].width = img_width / 2;
		rect_s_.rectengales[i].height = img_height / num_rect;

		img_coords.y += img_height / num_rect;
	}	
	write_roi(current_img);
	
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
	for (size_t i = 0; i < inc_images.size(); i++) {
		images_[i] = inc_images[i];
		cout << "current_img: " << images_[i].size << endl;
	}
}

void Rectengales::desginate_rectengales(int desired_rect) {

	if (desired_rect % 2 != 0)
		desired_rect = desired_rect + 1;
	
	Rect rect_filler;		
	for (size_t i = 0; i < desired_rect; i++)
		rectengales.push_back(rect_filler);	
}

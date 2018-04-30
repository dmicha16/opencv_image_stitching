#pragma once
#include "Wrapper.h"
class ImageReader :
	public Wrapper {
public:
	ImageReader();
	~ImageReader();
	vector<Mat> get_images();
	int get_num_images();

private:
	int num_images_;
	vector<String> img_names_;
	Mat img_;
	vector<Mat> images_;
	void read_images_(const string path);
	vector<Mat> upload_images_(vector<Mat> images_);
};


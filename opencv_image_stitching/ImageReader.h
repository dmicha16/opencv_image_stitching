#pragma once
#include "Wrapper.h"
class ImageReader :
	public Wrapper {
public:
	ImageReader();
	~ImageReader();
	vector<Mat> get_images();
	int get_number_images();

private:
	int num_images;
	vector<String> img_names;
	Mat full_img, img;
	vector<Mat> images;
	void read_images_(string path);
	vector<Mat> upload_images_(vector<Mat> images, vector<Size> full_img_sizes);
};


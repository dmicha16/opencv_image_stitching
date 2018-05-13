#include "ImageReader.h"

ImageReader::ImageReader() {
	String path = "../opencv_image_stitching/Images/";
	read_images_(path);
}

ImageReader::~ImageReader() {
}

vector<Mat> ImageReader::get_images() {
	return images_;
}

int ImageReader::get_num_images() {
	return num_images_;
}

void ImageReader::read_images_(const string path) {
	vector<String> photos;

	for (auto & file : experimental::filesystem::directory_iterator(path))
		cout << file << endl;

	glob(path, photos, false);
	cout << "Images read: " << photos.size() << endl;

	for (int i = 0; i < photos.size(); i++) {
		img_names_.push_back(photos[i]);
	}

	num_images_ = static_cast <int> (img_names_.size());

	images_.resize(num_images_);
	images_ = upload_images_(images_);
	for (size_t i = 0; i < num_images_; i++) {
		resize(images_[i], images_[i], cvSize(0, 0), 0.55, 0.55);
	}
}

vector<Mat> ImageReader::upload_images_(vector<Mat> images_) {

	for (int i = 0; i < num_images_; ++i)
		images_[i] = imread(img_names_[i]);
	
	return images_;
}

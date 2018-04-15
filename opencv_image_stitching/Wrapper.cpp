#include "Wrapper.h"

Wrapper::Wrapper() {
	String path = "../opencv_image_stitching/Images/";
	read_images_(path);
}

Wrapper::~Wrapper() {
}

vector<Mat> Wrapper::get_images() {
	return images;
}

void Wrapper::read_images_(string path) {
	vector<String> photos;

	for (auto & file : experimental::filesystem::directory_iterator(path))
		cout << file << endl;

	glob(path, photos, false);

	//string file_name = "C:/photos/T4D/KEYPOINTS/test";

	cout << "Images read: " << photos.size() << endl;
	WINPAUSE;
	for (int i = 0; i < photos.size(); i++) {
		img_names.push_back(photos[i]);
	}

	num_images = static_cast <int> (img_names.size());
	vector<Size> full_img_sizes(num_images);

	images.resize(num_images);
	images = upload_images_(images, full_img_sizes);
}

vector<Mat> Wrapper::upload_images_(vector<Mat> images, vector<Size> full_img_sizes) {

	double work_megapix = 0.6;
	double seam_megapix = 0.1;
	double work_scale = 1, seam_scale = 1, compose_scale = 1, seam_work_aspect = 1;
	bool is_work_scale_set = false, is_seam_scale_set = false, is_compose_scale_set = false;

	for (int i = 0; i < num_images; ++i) {
		full_img = imread(img_names[i]);
		full_img_sizes[i] = full_img.size();

		if (work_megapix < 0) {
			img = full_img;
			work_scale = 1;
			is_work_scale_set = true;
		}
		else {
			if (!is_work_scale_set) {
				work_scale = min(1.0, sqrt(work_megapix * 1e6 / full_img.size().area()));
				is_work_scale_set = true;
			}
			resize(full_img, img, Size(), work_scale, work_scale, INTER_LINEAR_EXACT);
		}
		if (!is_seam_scale_set) {
			seam_scale = min(1.0, sqrt(seam_megapix * 1e6 / full_img.size().area()));
			seam_work_aspect = seam_scale / work_scale;
			is_seam_scale_set = true;
		}
		images[i] = img.clone();
	}
	full_img.release();
	img.release();
	return images;
}

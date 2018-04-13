#pragma once
#include "Wrapper.h"
class Undistorter :
	public Wrapper {
public:
	Undistorter();	
	~Undistorter();

protected:
	int num_images;
	vector<String> img_names;
	Mat full_img, img;

private:
	vector<Mat> read_images_(string path);
	void undistort_images_(vector<Mat> images);
	vector<Mat> upload_images_(vector<Mat> images, vector<Size> full_img_sizes);

	Mat dist_coef_;
	Mat R_;  
	double aspect_;
	double ppx_;
	double ppy_;
	double focal_;
	Mat t_;
};


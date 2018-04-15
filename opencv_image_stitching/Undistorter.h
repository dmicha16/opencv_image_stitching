#pragma once
#include "Wrapper.h"
class Undistorter :
	public Wrapper {
public:
	Undistorter();	
	~Undistorter();
	vector<Mat> undistort_images(vector<Mat> images);

private:

	Mat dist_coef_;
	Mat R_;  
	double aspect_;
	double ppx_;
	double ppy_;
	double focal_;
	Mat t_;
};


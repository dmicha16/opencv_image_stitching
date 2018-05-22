#pragma once
#include "Wrapper.h"
class Undistorter :
	public Wrapper {
public:
	Undistorter();	
	~Undistorter();
	vector<Mat> undistort_images(vector<Mat> images);
	//int focal_choice;

private:
	//Mat dist_coef_;
	//Mat K_;  
};


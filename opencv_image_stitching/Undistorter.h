#pragma once
#include "Wrapper.h"
class Undistorter :
	public Wrapper {
public:
	Undistorter();	
	~Undistorter();

private:
	Mat dist_coef_;
	Mat K_;  
};


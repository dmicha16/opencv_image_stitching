#pragma once
#include "Pipeline.h"
class ExposureComp :
	public Pipeline {
public:
	ExposureComp(vector<Point> corners, vector<UMat> images_warped, vector<UMat> masks_warped);
	vector<UMat> returnMasksWarped();
	~ExposureComp();

private:
	int expos_comp_type_ = ExposureCompensator::GAIN;
	vector<UMat> masks_warped;

};


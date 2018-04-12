#include "ExposureComp.h"



ExposureComp::ExposureComp(vector<Point> corners, vector<UMat> images_warped, vector<UMat> masks_warped) {
	Ptr<ExposureCompensator> compensator = ExposureCompensator::createDefault(expos_comp_type_);
	compensator->feed(corners, images_warped, masks_warped); // The output from this is an updated masks_warped variabel

	this->masks_warped = masks_warped;
}

vector<UMat> ExposureComp::returnMasksWarped() {
	return this->masks_warped;
}

ExposureComp::~ExposureComp() {
}

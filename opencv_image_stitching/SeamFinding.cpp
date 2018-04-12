#include "SeamFinding.h"



SeamFinding::SeamFinding(vector<UMat> images_warped_f, vector<Point> corners, vector<UMat> masks_warped){

	Ptr<SeamFinder> seam_finder;
	//seam_finder = makePtr<detail::NoSeamFinder>(); // Not sure which one is to be used.
	seam_finder = makePtr<detail::VoronoiSeamFinder>();
	seam_finder->find(images_warped_f, corners, masks_warped); // The output from this is an updated masks_warped variabel

	this->masks_warped = masks_warped;

	// Not sure we want to do the below code piece here
	/*
	// Release unused memory
	images.clear();
	images_warped.clear();
	images_warped_f.clear();
	masks.clear();
	*/
}

vector<UMat> SeamFinding::returnMasksWarped() {
	return this->masks_warped;
}

SeamFinding::~SeamFinding() {
}

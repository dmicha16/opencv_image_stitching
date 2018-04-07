#pragma once
#include "Pipeline.h"
class Features :
	public Pipeline {
public:
	Features(vector<Mat> images);
	~Features();

private:
	vector<Mat> findFeatures(vector<Mat> inc_images, vector<ImageFeatures> &features, vector<ImageFeatures> &features_new);
	void matchFeatures(vector<Mat> inc_images, vector<ImageFeatures> &features, vector<ImageFeatures> &features_new);
};


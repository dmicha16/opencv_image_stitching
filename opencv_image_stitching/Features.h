#pragma once
#include "Pipeline.h"
class Features :
	public Pipeline {
public:
	Features(vector<Mat> images);
	vector<KeyPoint> returnKeyPoints(vector<KeyPoint> filtered_keypoints);
	vector<DMatch> returnMatches(vector<DMatch> filtered_matches);
	~Features();

private:
	vector<Mat> findFeatures(vector<Mat> inc_images, vector<ImageFeatures> &features, vector<ImageFeatures> &features_new);
	void matchFeatures(vector<Mat> inc_images, vector<ImageFeatures> &features, vector<ImageFeatures> &features_new);
	int setThreshold(vector<DMatch> matches, float desired_percentage);
};


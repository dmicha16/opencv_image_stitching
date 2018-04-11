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
	void createImageSubset(vector<ImageFeatures> &features_new, vector<MatchesInfo> pairwise_matches, vector<Mat> images);
	int setThreshold(vector<DMatch> matches, float desired_percentage);
	void matchesDraw(Mat img_1, vector<KeyPoint> keypoints_1, Mat img_2, vector<KeyPoint> keypoints_2, vector<DMatch> good_matches);
};


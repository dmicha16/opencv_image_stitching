#pragma once
#include "Wrapper.h"
class FeatureFindMatch :
	public Wrapper {
public:
	FeatureFindMatch();
	void find_features(vector<Mat> inc_images);
	vector<vector<Point2f>> get_matched_coordinates();
	~FeatureFindMatch();

protected:
	vector<ImageFeatures> image_features;

private:

	int num_images;
	vector<vector<Point2f>> matched_keypoints;

	void match_features_(vector<Mat> inc_images, vector<ImageFeatures> strict_features);
	vector<Mat> createImageSubset(vector<ImageFeatures> &strict_features, vector<MatchesInfo> pairwise_matches, vector<Mat> images);
	int setThreshold(vector<DMatch> matches, float desired_percentage);
	void matchesDraw(Mat img_1, vector<KeyPoint> keypoints_1, Mat img_2, vector<KeyPoint> keypoints_2, vector<DMatch> good_matches);
	void displayPairWisematches(vector<MatchesInfo> pairwise_matches);
};


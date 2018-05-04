#pragma once
#include "Wrapper.h"
#include "RoiCalculator.h"

//typedef struct MatchedImageData {
//	Mat img_1, img_2;
//	vector<KeyPoint> keypoints_1;
//	vector<KeyPoint> keypoints_2;
//	vector<DMatch> good_matches;
//};


class FeatureFindMatch :
	public Wrapper {
public:
	FeatureFindMatch();
	~FeatureFindMatch();

	void find_features(const vector<Mat> inc_images, const float inc_threshold);
	MatchedKeyPoint get_matched_coordinates();
private:

	vector<ImageFeatures> image_features_;
	int num_images_;
	float threshold_;

	MatchedKeyPoint matched_keypoints_;

	bool keypoint_area_check_(vector<Mat> inc_images, int desired_occ_rects);
	void match_features_(const vector<Mat> inc_images, const vector<ImageFeatures> strict_features);
	int calculate_treshold_(vector<DMatch> matches, float desired_percentage);
	void matches_drawer_(Mat img_1, vector<KeyPoint> keypoints_1, Mat img_2, vector<KeyPoint> keypoints_2, vector<DMatch> good_matches);
	void display_pairwise_matches_(const vector<MatchesInfo> pairwise_matches);

	//vector<Mat> createImageSubset(vector<ImageFeatures> &strict_features, vector<MatchesInfo> pairwise_matches, vector<Mat> images);
};


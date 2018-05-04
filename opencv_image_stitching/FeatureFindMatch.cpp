#include "FeatureFindMatch.h"

struct SortOperator {
	bool operator() (int i, int j) {
		return (i < j);
	}
} sort_operator_;

FeatureFindMatch::FeatureFindMatch() {
}

void FeatureFindMatch::find_features(const vector<Mat> inc_images, const float inc_threshold) {

	threshold_ = inc_threshold;
	num_images_ = static_cast <int>(inc_images.size());
	image_features_.resize(num_images_);
	string features_out;

	for (int i = 0; i < num_images_; ++i) {

		features_out = "Features in image #";

		float scaleFactor = 1.2f;
		int nlevels = 8;
		int edgeThreshold = 31;
		int firstLevel = 0;
		int WTA_K = 2;
		int scoreType = ORB::HARRIS_SCORE;
		int patchSize = 31;
		int fastThreshold = 20;

		Ptr<ORB> detector_desciptor;

		detector_desciptor = ORB::create(1000, scaleFactor, nlevels, edgeThreshold,
			firstLevel, WTA_K, scoreType, patchSize, fastThreshold);

		try {
			InputArray mask = noArray();
			detector_desciptor->detectAndCompute(inc_images[i], mask, image_features_[i].keypoints, image_features_[i].descriptors);
		}
		catch (const std::exception& e) {
			cout << e.what() << endl;
		}

		image_features_[i].img_idx = i;
		features_out += to_string(i + 1) + ": " + to_string(image_features_[i].keypoints.size());
		CLOG(features_out, Verbosity::INFO);
		LOGLN(features_out);

	}
	match_features_(inc_images, image_features_);
}

MatchedKeyPoint FeatureFindMatch::get_matched_coordinates() {
	return matched_keypoints_;
}

bool FeatureFindMatch::keypoint_area_check_(vector<Mat> inc_images, int desired_occ_rects) {
	RoiCalculator roi_calculator;
	roi_calculator.set_image(inc_images[0]);
	roi_calculator.set_matched_keypoints(matched_keypoints_);
	roi_calculator.calculate_roi(3, 2, 0.8);		
	if (roi_calculator.num_occupied_rects() >= desired_occ_rects)
		return true;
	else
		false;
}

void FeatureFindMatch::match_features_(const vector<Mat> inc_images, const vector<ImageFeatures> image_features_) {

	float match_conf = 0.3f;
	bool try_cuda = false;

	vector<MatchesInfo> pairwise_matches;
	Mat img_1 = inc_images[0];
	Mat img_2 = inc_images[1];
	vector<KeyPoint> keypoints_1 = image_features_[0].keypoints;
	vector<KeyPoint> keypoints_2 = image_features_[1].keypoints;	

#pragma region logging

	string keypoints_features_1 = "Keypoints 1 from features i: " + to_string(keypoints_1.size());
	string keypoints_features_2 = "Keypoints 2 from features i: " + to_string(keypoints_2.size());

	CLOG(keypoints_features_1, Verbosity::INFO);
	CLOG(keypoints_features_2, Verbosity::INFO);

	string image_length = "Images length: " + to_string(inc_images.size());
	string keypoints_length_1 = "Keypoints_1 length: " + to_string(keypoints_1.size());
	string keypoints_length_2 = "Keypoints_2 length: " + to_string(keypoints_2.size());

	CLOG(image_length, Verbosity::INFO);
	CLOG(keypoints_length_1, Verbosity::INFO);
	CLOG(keypoints_length_2, Verbosity::INFO);

	LOG("Pairwise matching\n");
	CLOG("line");

#pragma endregion //local_logging

	Ptr<FeaturesMatcher> current_matcher = makePtr<AffineBestOf2NearestMatcher>(false, try_cuda, match_conf);

	try {
		(*current_matcher)(image_features_, pairwise_matches);
		//(*current_matcher)(image_features_[0], image_features_[1], pairwise_matches);
	}
	catch (const std::exception& e) {
		cout << e.what() << endl;
		CLOG("Matching failed.", Verbosity::ERR);
		WINPAUSE;
	}

	vector<DMatch> all_matches = pairwise_matches[1].matches;
	vector<DMatch> filtered_matches;

	//display_pairwise_matches_(pairwise_matches);
	
	int calculated_threshold = calculate_treshold_(all_matches, threshold_);	
	for (size_t i = 0; i < all_matches.size(); i++) {

		if (all_matches[i].distance <= calculated_threshold)
			filtered_matches.push_back(all_matches[i]);
	}

#pragma region logging
	cout << "min hamming of good matches: " << filtered_matches[0].distance << endl;
	cout << "Good matches #:" << filtered_matches.size() << endl;
	string filtered_matches_out = "Good Matches #: " + to_string(filtered_matches.size());
	CLOG(filtered_matches_out, Verbosity::INFO);

	for (size_t i = 0; i < filtered_matches.size(); i++) {
		string msg = "Matches distance : " + to_string(i) + to_string(filtered_matches[i].distance);
		string msg1 = "Matches imgIdx: " + to_string(i) + to_string(filtered_matches[i].imgIdx);
		string msg2 = "Matches trainIdx: " + to_string(i) + to_string(filtered_matches[i].trainIdx);
		string msg3 = "Matches queryIdx: " + to_string(i) + to_string(filtered_matches[i].queryIdx);
		CLOG(msg, Verbosity::INFO);
		CLOG(msg1, Verbosity::INFO);
		CLOG(msg2, Verbosity::INFO);
		CLOG(msg3, Verbosity::INFO);
	}

#pragma endregion // logging	

	matched_keypoints_.image_1.resize(filtered_matches.size());
	matched_keypoints_.image_2.resize(filtered_matches.size());

	for (size_t i = 0; i < filtered_matches.size(); i++) {		
		matched_keypoints_.image_1[i].x = (keypoints_1[filtered_matches[i].queryIdx].pt.x);
		matched_keypoints_.image_1[i].y = (keypoints_1[filtered_matches[i].queryIdx].pt.y);

		matched_keypoints_.image_2[i].x = (keypoints_2[filtered_matches[i].trainIdx].pt.x);
		matched_keypoints_.image_2[i].y = (keypoints_2[filtered_matches[i].trainIdx].pt.y);
		//cout << matched_keypoints_.image_1[i] << endl;
		//cout << matched_keypoints_.image_2[i] << endl;
	}

	bool enough_occupied = keypoint_area_check_(inc_images, 3);
	cout << "enough occupied? : " << enough_occupied << endl;
	WINPAUSE;
	matches_drawer_(img_1, keypoints_1, img_2, keypoints_2, filtered_matches);
	current_matcher->collectGarbage();
}

int FeatureFindMatch::calculate_treshold_(vector<DMatch> my_matches, float desired_percentage) {

	float calculated_tresh = 0;
	vector<float> distances;
	int avarage = 0;

	for (size_t i = 0; i < my_matches.size(); i++)
		distances.push_back(my_matches[i].distance);

	std::sort(distances.begin(), distances.end(), sort_operator_);

	string dmatches = "DMatches[i]: " + to_string(my_matches.size());	
	CLOG(dmatches, Verbosity::INFO);
	cout << dmatches << endl;

	for (size_t i = 0; i < my_matches.size(); i++)
		avarage = avarage + my_matches[i].distance;

	avarage = avarage / my_matches.size();
	string avrage = "Avarage: " + to_string(avarage);
	cout << avrage << endl;
	CLOG(avrage, Verbosity::INFO);

	calculated_tresh = distances.size() * desired_percentage;
	calculated_tresh = (int)calculated_tresh;
	calculated_tresh = distances[calculated_tresh];

	return calculated_tresh;
}

void FeatureFindMatch::matches_drawer_(Mat img_1, vector<KeyPoint> keypoints_1, Mat img_2, vector<KeyPoint> keypoints_2, vector<DMatch> filtered_matches) {

	String output_location = "../opencv_image_stitching/Images/Results/test_1.jpg";
	vector<char> mask(filtered_matches.size(), 1);
	Mat output_img;
	
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, filtered_matches, output_img, Scalar::all(-1),
		Scalar::all(-1), mask, DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	imwrite(output_location, output_img);
}

void FeatureFindMatch::display_pairwise_matches_(const vector<MatchesInfo> pairwise_matches) {

	cout << "\n-----------------------------" << endl;
	cout << "display_pairwise_matches_() {" << endl;
	cout << "\t-------------------" << endl;

	for (size_t i = 1; i < pairwise_matches.size() - 1; i++) {
		cout << "\tdst_img_indx: " << pairwise_matches[i].dst_img_idx << endl;
		cout << "\tconfidence: " << pairwise_matches[i].confidence << endl;
		cout << "\tH: " << pairwise_matches[i].H << endl;
		//cout << "inliers_mask[i]: " << pairwise_matches[i].inliers_mask[i] << endl;
		cout << "\tnum_inliers: " << pairwise_matches[i].num_inliers << endl;
		cout << "\t-------------------" << endl;
	}
	cout << "}" << endl;
	cout << "-----------------------------" << endl << endl;
}

FeatureFindMatch::~FeatureFindMatch() {
}
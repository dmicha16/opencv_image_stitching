#include "FeatureFindMatch.h"

FeatureFindMatch::FeatureFindMatch() {
}

void FeatureFindMatch::find_features(vector<Mat> inc_images) {
	num_images = static_cast <int>(inc_images.size());
	image_features.resize(num_images);

	string features_out = "Features in image #";

	for (int i = 0; i < num_images; ++i) {

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

		detector_desciptor = ORB::create(1500, scaleFactor, nlevels, edgeThreshold,
			firstLevel, WTA_K, scoreType, patchSize, fastThreshold);

		try {
			InputArray mask = noArray();
			detector_desciptor->detectAndCompute(inc_images[i], mask, image_features[i].keypoints, image_features[i].descriptors);
		}
		catch (const std::exception& e) {
			cout << e.what() << endl;
		}

		image_features[i].img_idx = i;
		features_out += to_string(i + 1) + ": " + to_string(image_features[i].keypoints.size());
		CLOG(features_out, Verbosity::INFO);
		LOGLN(features_out);

	}
	match_features_(inc_images, image_features);
}

MatchedKeyPoint FeatureFindMatch::get_matched_coordinates() {
	return matched_keypoints;
}

void FeatureFindMatch::match_features_(vector<Mat> inc_images, vector<ImageFeatures> image_features) {

	float match_conf = 0.3f;
	bool try_cuda = false;
	int range_width = -1;

	//vector<MatchesInfo> pairwise_matches;
	MatchesInfo pairwise_matches;
	//cout << "pairwise_matches #i: " << pairwise_matches.size() << endl;

	Mat img_1 = inc_images[0];
	Mat img_2 = inc_images[1];

	vector<KeyPoint> keypoints_1 = image_features[0].keypoints;
	vector<KeyPoint> keypoints_2 = image_features[1].keypoints;

	string keypoints_features_1 = "Keypoints 1 from features i: " + to_string(keypoints_1.size());
	string keypoints_features_2 = "Keypoints 2 from features i: " + to_string(keypoints_2.size());

	CLOG(keypoints_features_1, Verbosity::INFO);
	CLOG(keypoints_features_2, Verbosity::INFO);

	vector<DMatch> my_matches;
	vector<DMatch> good_matches;

	LOG("Pairwise matching\n");
	CLOG("line");

	Ptr<FeaturesMatcher> current_matcher = makePtr<AffineBestOf2NearestMatcher>(false, try_cuda, match_conf);

	try {
		//(*current_matcher)(image_features, pairwise_matches);
		(*current_matcher)(image_features[0], image_features[1], pairwise_matches);
	}
	catch (const std::exception& e) {
		cout << e.what() << endl;
		CLOG("Matching failed.", Verbosity::ERR);
		WINPAUSE;
	}

	string image_length = "Images length: " + to_string(inc_images.size());
	string keypoints_length_1 = "Keypoints_1 length: " + to_string(keypoints_1.size());
	string keypoints_length_2 = "Keypoints_2 length: " + to_string(keypoints_2.size());

	CLOG(image_length, Verbosity::INFO);
	CLOG(keypoints_length_1, Verbosity::INFO);
	CLOG(keypoints_length_2, Verbosity::INFO);

	cout << "Images length: " << inc_images.size() << endl;
	//cout << "pairwise_matches #i: " << pairwise_matches.size() << endl;
	
	//displayPairWisematches(pairwise_matches);

	//my_matches = pairwise_matches[1].matches;
	my_matches = pairwise_matches.matches;

	int threshold = setThreshold(my_matches, 0.25);
	for (size_t i = 0; i < my_matches.size(); i++) {

		if (my_matches[i].distance < threshold)
			good_matches.push_back(my_matches[i]);
	}

	cout << "Good matches #:" << good_matches.size() << endl;
	string good_matches_out = "Good Matches #: " + to_string(good_matches.size());
	CLOG(good_matches_out, Verbosity::INFO);

	for (size_t i = 0; i < good_matches.size(); i++) {
		string msg = "Matches distance : " + to_string(i) + to_string(good_matches[i].distance);
		string msg1 = "Matches imgIdx: " + to_string(i) + to_string(good_matches[i].imgIdx);
		string msg2 = "Matches trainIdx: " + to_string(i) + to_string(good_matches[i].trainIdx);
		string msg3 = "Matches queryIdx: " + to_string(i) + to_string(good_matches[i].queryIdx);
		CLOG(msg, Verbosity::INFO);
		CLOG(msg1, Verbosity::INFO);
		CLOG(msg2, Verbosity::INFO);
		CLOG(msg3, Verbosity::INFO);
	}

////////////////////////////////////////////////////////////////////////////////////////// sorting the matches
	
	// This sorts the macthes, so that the macthes with the smallest "distance" are put first in the vector  
	for (size_t i = 0; i < good_matches.size() + 1; i++) {
		sort(good_matches.begin(), good_matches.begin()+i);
	}
	
	/*
	for (size_t j = 0; j < good_matches.size(); j++) {
		cout << "good_matches[i].distance = " << good_matches[j].distance << endl;
	}*/

////////////////////////////////////////////////////////////////////////////////////////// sorting the matches

	matched_keypoints.image_1.resize(good_matches.size());
	matched_keypoints.image_2.resize(good_matches.size());
	
	for (size_t i = 0; i < good_matches.size(); i++) {
		matched_keypoints.image_1[i] = (keypoints_1[good_matches[i].queryIdx].pt);
		matched_keypoints.image_2[i] = (keypoints_2[good_matches[i].trainIdx].pt);
		/*cout << matched_keypoints.image_1[i] << endl;
		cout << matched_keypoints.image_2[i] << endl;*/
	}

	matchesDraw(img_1, keypoints_1, img_2, keypoints_2, good_matches);
	current_matcher->collectGarbage();
	//createImageSubset(image_features, pairwise_matches, inc_images);
}

vector<Mat> FeatureFindMatch::createImageSubset(vector<ImageFeatures> &image_features, vector<MatchesInfo> pairwise_matches, vector<Mat> inc_images) {

	float conf_thresh = 3.f;
	int num_images = static_cast <int>(inc_images.size());

	vector<int> indices = leaveBiggestComponent(image_features, pairwise_matches, conf_thresh);
	vector<Mat> img_subset;
	vector<String> img_names_subset;
	vector<Size> full_img_sizes_subset;

	for (size_t i = 0; i < indices.size(); ++i) {
		img_subset.push_back(inc_images[indices[i]]);
	}

	inc_images = img_subset;

	// Check if we still have enough images
	num_images = static_cast<int>(inc_images.size());
	cout << "num_images: " << num_images << endl;
	WINPAUSE;
	if (num_images < 2) {
		LOGLN("Need more images");
		WINPAUSE;
	}

	return inc_images;
}

int FeatureFindMatch::setThreshold(vector<DMatch> my_matches, float desired_percentage) {

	float threshold = 0;
	vector<int> distances(my_matches.size());
	for (size_t i = 0; i < my_matches.size(); i++)
		distances.push_back(my_matches[i].distance);

	string dmatches = "DMatches[i]: " + to_string(my_matches.size());
	int avarage = 0, max = 0, min = 80;
	CLOG(dmatches, Verbosity::INFO);
	cout << "DMatches[i]: " << my_matches.size() << endl;

	for (size_t i = 0; i < my_matches.size(); i++) {
		avarage = avarage + my_matches[i].distance;
		if (my_matches[i].distance > max) {
			max = my_matches[i].distance;
		}
		if (my_matches[i].distance < min) {
			min = my_matches[i].distance;
		}
	}

	avarage = avarage / my_matches.size();
	string avrage = "Avarage: " + to_string(avarage);
	string dist_max = "Distance max: " + to_string(max);
	string dist_min = "Distance min: " + to_string(min);
	CLOG(avrage, Verbosity::INFO);
	CLOG(dist_max, Verbosity::INFO);
	CLOG(dist_min, Verbosity::INFO);

	int combined_values = 0;

	for (size_t i = 0; i < distances.size(); i++) {
		combined_values += distances[i];
	}
	cout << "combined_values: " << combined_values << endl;
	threshold = my_matches.size() * desired_percentage;
	cout << "threshold: " << threshold << endl;
	
	return static_cast<int>(threshold);	
}

void FeatureFindMatch::matchesDraw(Mat img_1, vector<KeyPoint> keypoints_1, Mat img_2, vector<KeyPoint> keypoints_2, vector<DMatch> good_matches) {

	String output_location = "../opencv_image_stitching/Images/Results/test_1.jpg";
	vector<char> mask(good_matches.size(), 1);
	Mat output_img;

	try {
		drawMatches(img_1, keypoints_1, img_2, keypoints_2, good_matches, output_img, Scalar::all(-1),
			Scalar::all(-1), mask, DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	}
	catch (const std::exception& e) {
		cout << e.what() << endl;
		WINPAUSE;
	}

	Mat outImg;
	resize(output_img, outImg, cv::Size(), 1, 1);
	//imshow("Matching", outImg);

	imwrite(output_location, outImg);
}

void FeatureFindMatch::displayPairWisematches(vector<MatchesInfo> pairwise_matches) {

	cout << "\n-----------------------------" << endl;
	cout << "displayPairWisematches() {" << endl;
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

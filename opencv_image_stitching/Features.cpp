#include "Features.h"

Features::Features(vector<Mat> inc_images) {

	vector<ImageFeatures> features(inc_images.size());
	vector<ImageFeatures> features_new(inc_images.size());

	vector<Mat> images_local = findFeatures(inc_images, features, features_new);
	matchFeatures(images_local, features, features_new);
}

vector<Mat> Features::findFeatures(vector<Mat> inc_images, vector<ImageFeatures> &features, vector<ImageFeatures> &features_new) {
	int num_images = static_cast <int>(inc_images.size());
	Ptr<FeaturesFinder> finder = makePtr<OrbFeaturesFinder>();

	cout << num_images << endl;
	string features_out = "Features in image #";
	string new_features = "Number of features after ORB recustruct: ";

	for (int i = 0; i < num_images; ++i) {
		cout << "hello2" << endl;

		features_out = "Features in image #";
		new_features = "Number of features after ORB recustruct: ";
		try {
			(*finder)(inc_images[i], features[i]);
		}
		catch (const std::exception& e) {
			cout << e.what() << endl;
			WINPAUSE;
		}

		features[i].img_idx = i;
		features_out += to_string(i + 1) + ": " + to_string(features[i].keypoints.size());
		CLOG(features_out, Verbosity::INFO);
		LOGLN("Features in image #" << i + 1 << ": " << features[i].keypoints.size());

		float scaleFactor = 1.2f;
		int nlevels = 8;
		int edgeThreshold = 31;
		int firstLevel = 0;
		int WTA_K = 2;
		int scoreType = ORB::HARRIS_SCORE;
		int patchSize = 31;
		int fastThreshold = 20;

		Ptr<ORB> detector, extractor;

		extractor = ORB::create();
		detector = ORB::create(features[i].keypoints.size(), scaleFactor, nlevels, edgeThreshold,
			firstLevel, WTA_K, scoreType, patchSize, fastThreshold);

		try {
			detector->detect(inc_images[i], features_new[i].keypoints);
			extractor->compute(inc_images[i], features_new[i].keypoints, features_new[i].descriptors);
		}
		catch (const std::exception& e) {
			cout << e.what() << endl;
		}

		cout << features[i].keypoints.size() << endl;
		cout << features_new[i].keypoints.size() << endl;
		WINPAUSE;

	}
	finder->collectGarbage();
	return inc_images;
}

void Features::matchFeatures(vector<Mat> inc_images, vector<ImageFeatures> &features, vector<ImageFeatures> &features_new) {

	int num_images = static_cast <int>(inc_images.size());
	float match_conf = 0.3f;
	bool try_cuda = false;
	int range_width = -1;

	vector<MatchesInfo> pairwise_matches(num_images);
	Mat output_img;
	String output_location = "C:/photos/matching_output/test_";

	Ptr<FeaturesMatcher> current_matcher;
	Mat img_1 = inc_images[0];
	Mat img_2 = inc_images[1];

	vector<KeyPoint> keypoints_1 = features_new[0].keypoints;
	vector<KeyPoint> keypoints_2 = features_new[1].keypoints;


	for (size_t i = 0; i < keypoints_1.size(); i++) {
		cout << "keypoints_1[" << i << "]: " << keypoints_1[i].pt;
	}

	int x = keypoints_1[0].pt.x;
	int y = keypoints_1[0].pt.y;

	WINPAUSE;

	string keypoints_features_1 = "Keypoints 1 from features i: " + to_string(keypoints_1.size());
	string keypoints_features_2 = "Keypoints 2 from features i: " + to_string(keypoints_2.size());

	CLOG(keypoints_features_1, Verbosity::INFO);
	CLOG(keypoints_features_2, Verbosity::INFO);

	vector<DMatch> my_matches;
	vector<DMatch> good_matches;
	int avarage = 0, max = 0, min = 80;

	for (size_t i = 0; i < 3; i++) {

		my_matches.clear();
		good_matches.clear();

		output_location = "C:/photos/matching_output/test_";
		output_location = output_location + to_string(i + 1);
		output_location = output_location + ".jpg";
		LOG("Pairwise matching\n");
		CLOG("line");

		switch (i) {
		case 0:
			current_matcher = makePtr<AffineBestOf2NearestMatcher>(false, try_cuda, match_conf);
		case 1:
			current_matcher = makePtr<BestOf2NearestMatcher>(try_cuda, match_conf);
		case 2:
			current_matcher = makePtr<BestOf2NearestRangeMatcher>(range_width, try_cuda, match_conf);
		default:
			break;
		}

		try {
			(*current_matcher)(features_new, pairwise_matches);
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
		//cout << "Keypoints length: " << key_points_vector.size() << endl;

		my_matches = pairwise_matches[1].matches;
		string dmatches = "DMatches[i]: " + to_string(my_matches.size());
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

		for (size_t i = 0; i < my_matches.size(); i++) {

			//The smaller the better
			//if (my_matches[i].distance < 30)
			good_matches.push_back(my_matches[i]);
		}

		cout << "Good matches #:" << good_matches.size() << endl;
		string good_matches_out = "Good Matches #: " + to_string(good_matches.size());
		CLOG(good_matches_out, Verbosity::INFO);
		vector<char> mask(good_matches.size(), 1);

		for (size_t i = 0; i < good_matches.size(); i++) {
			string msg = "Matches distance i: " + to_string(good_matches[i].distance);
			string msg1 = "Matches imgIdx i: " + to_string(good_matches[i].imgIdx);
			string msg2 = "Matches trainIdx i: " + to_string(good_matches[i].trainIdx);
			string msg3 = "Matches queryIdx i: " + to_string(good_matches[i].queryIdx);
			CLOG(msg, Verbosity::INFO);
			CLOG(msg1, Verbosity::INFO);
			CLOG(msg2, Verbosity::INFO);
			CLOG(msg3, Verbosity::INFO);
		}

		try {
			drawMatches(img_1, keypoints_1, img_2, keypoints_2, good_matches, output_img, Scalar::all(-1),
				Scalar::all(-1), mask, DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
		}
		catch (const std::exception& e) {
			cout << e.what() << endl;
			WINPAUSE;
			continue;
		}

		Mat outImg;
		resize(output_img, outImg, cv::Size(), 1, 1);
		imshow("Matching", outImg);
		waitKey(0);
		current_matcher->collectGarbage();

#ifdef OUTPUT_TRUE 1
		imwrite(output_location, outImg);
#endif // OUTPUT_TRUE 1
		WINPAUSE;
	}
}


Features::~Features() {
}

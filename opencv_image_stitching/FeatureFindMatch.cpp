#include "FeatureFindMatch.h"

#include <windows.h> // for EXCEPTION_ACCESS_VIOLATION
#include <excpt.h>

struct SortOperator {
	bool operator() (int i, int j) {
		return (i < j);
	}
} sort_operator_;

FeatureFindMatch::FeatureFindMatch() {
}

FeatureFindMatch::~FeatureFindMatch() {
}

void FeatureFindMatch::find_features(const float inc_threshold, int inc_iteriation) {

	current_iteration_ = inc_iteriation;
	threshold_ = inc_threshold;
	num_images_ = static_cast <int>(inc_images_.size());
	for (size_t i = 0; i < image_features_.size(); i++) {
		image_features_[i].descriptors.release();
		image_features_[i].keypoints.clear();
		image_features_[i].img_idx = 0;		
	}
	
	image_features_.resize(num_images_);
	string features_out;

	float scaleFactor = 1.2f;
	int nlevels = 8;
	int edgeThreshold = 31;
	int firstLevel = 0;
	int WTA_K = 2;
	int scoreType = ORB::HARRIS_SCORE;
	int patchSize = 31;
	int fastThreshold = 20;

	Ptr<ORB> detector_desciptor;

	vector<ImageFeatures> temp_features(num_images_);
	vector<Mat> temp_images = calculate_temp_images();

	for (int i = 0; i < num_images_; ++i) {
		temp_features[i].descriptors.release();
		temp_features[i].keypoints.clear();

#pragma region logging

		/*features_out = "Features in image #";
		LOGLN("image_features_[" << i << "].keypoints.size: " << temp_features[i].keypoints.size());
		LOGLN("image_features_[" << i << "].descriptors.size: " << temp_features[i].descriptors.size());
		LOGLN("temp_images size: " << temp_images[i].size);*/

#pragma endregion //logging

		/*temp_features[i].descriptors.create(50, 100000, UMatUsageFlags::USAGE_ALLOCATE_HOST_MEMORY);
		temp_features[i].keypoints.reserve(100000);*/

		detector_desciptor = ORB::create(100000, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, scoreType, patchSize, fastThreshold);

		InputArray mask = noArray();		
		try {
			detector_desciptor->detectAndCompute(temp_images[i], mask, temp_features[i].keypoints, temp_features[i].descriptors); 		
		}
		catch (const std::exception& exception) {
			cout << exception.what() << endl;
			WINPAUSE;
		}

#pragma region logging
		/*LOGLN("AFTER temp_images size: " << inc_images_[i].size);
		LOGLN("AFTER image_features_[" << i << "].keypoints.size: " << temp_features[i].keypoints.size());
		LOGLN("AFTER image_features_[" << i << "].descriptors.size: " << temp_features[i].descriptors.size());*/

		/*temp_features[i].img_idx = i;
		features_out += to_string(i + 1) + ": " + to_string(temp_features[i].keypoints.size());
		CLOG(features_out, Verbosity::INFO);
		LOGLN(features_out);
		/*LOGLN("Current ORB iteration: " << i);*/

#pragma endregion //logging
				
		detector_desciptor->clear();
		detector_desciptor.release();
	}
	set_image_features(temp_features, inc_iteriation);

	detector_desciptor.release();
	match_features_();

}

void FeatureFindMatch::set_images(vector<Mat> images) {
	inc_images_.clear();
	inc_images_.resize(images.size());
	for (size_t i = 0; i < inc_images_.size(); i++)
		images[i].copyTo(inc_images_[i]);
}

MatchedKeyPoint FeatureFindMatch::get_matched_coordinates() {
	return matched_keypoints_;
}

bool FeatureFindMatch::keypoint_area_check_(int desired_occ_rects) {

	RoiCalculator roi_calculator;
	roi_calculator.set_image(inc_images_[0]);
	roi_calculator.set_matched_keypoints(matched_keypoints_);
	roi_calculator.calculate_roi(desired_rectangle_.columns,
		desired_rectangle_.rows, desired_rectangle_.image_overlap);
	cout << "desired_occ_rects = " << desired_occ_rects << endl;
	int accrect = roi_calculator.num_occupied_rects();
	cout << "accrect = " << accrect << endl;
	int return_decider = accrect - desired_occ_rects;
	cout << "return_decider = " << return_decider << endl;

	if (return_decider > 0) {
		cout << "returned true" << endl;
		return true;
	}
	else {
		cout << "returned false" << endl;
		return false;
	}
}

void FeatureFindMatch::match_features_() {

	float match_conf = 0.3f;
	bool try_cuda = false;

	vector<MatchesInfo> pairwise_matches;
	image_data_.img_1 = inc_images_[0];
	image_data_.img_2 = inc_images_[1];
	image_data_.keypoints_1 = image_features_[0].keypoints;
	image_data_.keypoints_2 = image_features_[1].keypoints;

#pragma region logging

	string keypoints_features_1 = "Keypoints 1 from features i: " + to_string(image_data_.keypoints_1.size());
	string keypoints_features_2 = "Keypoints 2 from features i: " + to_string(image_data_.keypoints_2.size());

	CLOG(keypoints_features_1, Verbosity::INFO);
	CLOG(keypoints_features_2, Verbosity::INFO);

	string image_length = "Images length: " + to_string(inc_images_.size());
	string keypoints_length_1 = "Keypoints_1 length: " + to_string(image_data_.keypoints_1.size());
	string keypoints_length_2 = "Keypoints_2 length: " + to_string(image_data_.keypoints_2.size());

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

	image_data_.all_matches = pairwise_matches[1].matches;	
	//display_pairwise_matches_(pairwise_matches);
	filter_matches_();

	current_matcher->collectGarbage();
}

void FeatureFindMatch::filter_matches_() {

	vector<DMatch> filtered_matches;
	bool enough_occupied = false;
	int calculated_threshold = 0;
	int desirec_occupied_rects = desired_rectangle_.desired_occupied;
	/*
	do {
		calculated_threshold = 0;
		matched_keypoints_.image_1.clear();
		matched_keypoints_.image_2.clear();
		filtered_matches.clear();

		calculated_threshold = calculate_treshold_(image_data_.all_matches, threshold_);
		cout << "calculated_threshold: " << calculated_threshold << endl;

		for (size_t i = 0; i < image_data_.all_matches.size(); i++) {

			if (image_data_.all_matches[i].distance <= calculated_threshold)
				filtered_matches.push_back(image_data_.all_matches[i]);
		}
		cout << "filtered_matches.size: " << filtered_matches.size() << endl;

		matched_keypoints_.image_1.resize(filtered_matches.size());
		matched_keypoints_.image_2.resize(filtered_matches.size());

		for (size_t i = 0; i < filtered_matches.size(); i++) {
			matched_keypoints_.image_1[i].x = (image_data_.keypoints_1[filtered_matches[i].queryIdx].pt.x);
			matched_keypoints_.image_1[i].y = (image_data_.keypoints_1[filtered_matches[i].queryIdx].pt.y);

			matched_keypoints_.image_2[i].x = (image_data_.keypoints_2[filtered_matches[i].trainIdx].pt.x);
			matched_keypoints_.image_2[i].y = (image_data_.keypoints_2[filtered_matches[i].trainIdx].pt.y);
			//cout << matched_keypoints_.image_1[i] << endl;
			//cout << matched_keypoints_.image_2[i] << endl;
		}
		enough_occupied = keypoint_area_check_(inc_images, desirec_occupied_rects);
		cout << "enough occupied: " << boolalpha << enough_occupied << endl;		
		//cout << "current_threshold: " << calculated_threshold << endl;
		cout << "threshold_: " << threshold_ << endl;
		threshold_ += 0.1;

		if (enough_occupied == false) {
		threshold_ += 0.1;
		}
		if (threshold_ == 1) {
		cout << "threshold_ IF Statement" << endl;
		enough_occupied = true;
		}
		enough_occupied = true; ///////////

	} while ((!enough_occupied) && (threshold_ <= 1));
	*/

	calculated_threshold = calculate_treshold_(image_data_.all_matches, threshold_);

	for (size_t i = 0; i < image_data_.all_matches.size(); i++) {

		if (image_data_.all_matches[i].distance <= calculated_threshold)
			filtered_matches.push_back(image_data_.all_matches[i]);
	}
	cout << "filtered_matches.size: " << filtered_matches.size() << endl;

	matched_keypoints_.image_1.resize(filtered_matches.size());
	matched_keypoints_.image_2.resize(filtered_matches.size());

	for (size_t i = 0; i < filtered_matches.size(); i++) {
		matched_keypoints_.image_1[i].x = (image_data_.keypoints_1[filtered_matches[i].queryIdx].pt.x);
		matched_keypoints_.image_1[i].y = (image_data_.keypoints_1[filtered_matches[i].queryIdx].pt.y);

		matched_keypoints_.image_2[i].x = (image_data_.keypoints_2[filtered_matches[i].trainIdx].pt.x);
		matched_keypoints_.image_2[i].y = (image_data_.keypoints_2[filtered_matches[i].trainIdx].pt.y);
		//cout << matched_keypoints_.image_1[i] << endl;
		//cout << matched_keypoints_.image_2[i] << endl;
	}
	
	//while ((!enough_occupied) && (threshold_ <= 1)){
	//	matched_keypoints_.image_1.clear();
	//	matched_keypoints_.image_2.clear();
	//	filtered_matches.clear();

	//	calculated_threshold = calculate_treshold_(image_data_.all_matches, threshold_);
	//	cout << "calculated_threshold: " << calculated_threshold << endl;

	//	for (size_t i = 0; i < image_data_.all_matches.size(); i++) {

	//		if (image_data_.all_matches[i].distance <= calculated_threshold)
	//			filtered_matches.push_back(image_data_.all_matches[i]);
	//	}
	//	cout << "filtered_matches.size: " << filtered_matches.size() << endl;

	//	matched_keypoints_.image_1.resize(filtered_matches.size());
	//	matched_keypoints_.image_2.resize(filtered_matches.size());

	//	for (size_t i = 0; i < filtered_matches.size(); i++) {
	//		matched_keypoints_.image_1[i].x = (image_data_.keypoints_1[filtered_matches[i].queryIdx].pt.x);
	//		matched_keypoints_.image_1[i].y = (image_data_.keypoints_1[filtered_matches[i].queryIdx].pt.y);

	//		matched_keypoints_.image_2[i].x = (image_data_.keypoints_2[filtered_matches[i].trainIdx].pt.x);
	//		matched_keypoints_.image_2[i].y = (image_data_.keypoints_2[filtered_matches[i].trainIdx].pt.y);
	//		//cout << matched_keypoints_.image_1[i] << endl;
	//		//cout << matched_keypoints_.image_2[i] << endl;
	//	}
	//	enough_occupied = keypoint_area_check_(desirec_occupied_rects);
	//	cout << "enough occupied: " << boolalpha << enough_occupied << endl;
	//	//cout << "current_threshold: " << calculated_threshold << endl;
	//	cout << "threshold_: " << threshold_ << endl;

	//	if (threshold_ == 1) {
	//		cout << "threshold_ IF Statement" << endl;
	//		enough_occupied = true;
	//	}

	//	if (enough_occupied == false) {
	//		threshold_ += 0.1;
	//	}
	//}

	cout << "Threshold has been set to: " << threshold_ << endl;	
	cout << "Good matches #:" << filtered_matches.size() << endl;
#pragma region logging
	/*cout << "min hamming of good matches: " << filtered_matches[0].distance << endl;
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
	}*/

#pragma endregion // logging	
	
	matches_drawer_(filtered_matches);
}

int FeatureFindMatch::calculate_treshold_(vector<DMatch> my_matches, float desired_percentage) {

	float calculated_tresh = 0;
	vector<float> distances;
	int average = 0;

	for (size_t i = 0; i < my_matches.size(); i++)
		distances.push_back(my_matches[i].distance);

	std::sort(distances.begin(), distances.end(), sort_operator_);

	for (size_t i = 0; i < my_matches.size(); i++)
		average = average + my_matches[i].distance;

	average = average / my_matches.size();

#pragma region logging
	/*string dmatches = "DMatches[i]: " + to_string(my_matches.size());
	CLOG(dmatches, Verbosity::INFO);
	cout << dmatches << endl;
	string avrage = "Avarage: " + to_string(avarage);
	cout << avrage << endl;
	CLOG(avrage, Verbosity::INFO);*/

#pragma endregion //logging

	calculated_tresh = distances[(int)distances.size() * desired_percentage];
	return calculated_tresh;
}

void FeatureFindMatch::matches_drawer_(vector<DMatch> filtered_matches) {

	vector<char> mask(filtered_matches.size(), 1);
	Mat output_img;
	
	drawMatches(image_data_.img_1, image_data_.keypoints_1, image_data_.img_2, image_data_.keypoints_2, filtered_matches, output_img, Scalar::all(-1),
		Scalar::all(-1), mask, DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	
	String output_location = "../opencv_image_stitching/Images/Results/matches_" + to_string(current_iteration_) + ".jpg";
	cv::imwrite(output_location, output_img);
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

void FeatureFindMatch::set_image_features(vector<ImageFeatures> temp_features, int inc_iteriation) {

	/*for (size_t i = 0; i < num_images_; i++) {
		image_features_[i].descriptors = temp_features[i].descriptors.clone();
		image_features_[i].keypoints = temp_features[i].keypoints;
	}*/
	
	if (inc_iteriation == 0) {
		for (size_t i = 0; i < num_images_; i++) {
			image_features_[i].descriptors = temp_features[i].descriptors.clone();
			image_features_[i].keypoints = temp_features[i].keypoints;
		}
	}
	else {
		for (size_t i = 0; i < num_images_; i++) {
			image_features_[i].keypoints.resize(temp_features[i].keypoints.size());
			image_features_[i].descriptors = temp_features[i].descriptors.clone();
		}
		//LOGLN("temp_features[i].keypoints.size(): " << temp_features[i].keypoints.size());
		for (size_t j = 0; j < temp_features[0].keypoints.size(); j++) {
			image_features_[0].keypoints[j].pt.x = temp_features[0].keypoints[j].pt.x;
			image_features_[0].keypoints[j].pt.y = temp_features[0].keypoints[j].pt.y + (inc_images_[0].rows - inc_images_[1].rows);
			//image_features_[0].keypoints[j].pt.y = temp_features[0].keypoints[j].pt.y + (int)(inc_images_[0].rows - (inc_images_[1].rows*0.9));
		}
		for (size_t j = 0; j < temp_features[1].keypoints.size(); j++) {
			image_features_[1].keypoints[j].pt.x = temp_features[1].keypoints[j].pt.x;
			image_features_[1].keypoints[j].pt.y = temp_features[1].keypoints[j].pt.y;
		}
	}

	//LOGLN("inc_images_[0].rows - inc_images_[1].rows: " << (inc_images_[0].rows - inc_images_[1].rows));
	LOGLN("image feature 1keypoint: " << image_features_[0].keypoints[1].pt.y);
	LOGLN("image feature 1keypoint Should be equal to: " << temp_features[0].keypoints[1].pt.y + (int)(inc_images_[0].rows - (inc_images_[1].rows)));
	//LOGLN("temp_features[0].keypoints[1].pt.y: " << temp_features[0].keypoints[1].pt.y);

	/*
	vector<Mat> temp_images(2);
	Rect roi_rectangle;

	roi_rectangle.x = 0;

	roi_rectangle.width = inc_images_[0].cols;
	roi_rectangle.height = inc_images_[1].rows;

	if (current_iteration_ == 0) {
		roi_rectangle.y = 0;
	}
	else {
		roi_rectangle.y = inc_images_[0].rows - inc_images_[1].rows;
		LOGLN("rect start height; " << (inc_images_[0].rows - inc_images_[1].rows));
	}
	//LOGLN("second image starting height : " << second_img_size.y);

	Mat temp_image_holder;
	inc_images_[0](Rect(roi_rectangle)).copyTo(temp_image_holder);

	Mat temp;
	inc_images_[0].copyTo(temp);
	
	rectangle(temp, Rect(roi_rectangle), Scalar(0, 0, 255), 3, LINE_8, 0);

	for (size_t i = 0; i < image_features_.size(); i++) {
		for (size_t j = 0; j < image_features_[i].keypoints.size(); j++) {
			drawMarker(temp, image_features_[i].keypoints[j].pt, Scalar(0, 255, 0), 100, 50, LINE_8);
		}
	}
	
	String output_location = "../opencv_image_stitching/Images/Results/roi_wtih_points" + to_string(current_iteration_) + ".jpg";
	imwrite(output_location, temp);
	*/
}

vector<Mat> FeatureFindMatch::calculate_temp_images() {

	vector<Mat> temp_images(2);
	Rect roi_rectangle;	

	roi_rectangle.x = 0;
	roi_rectangle.y = inc_images_[0].rows - inc_images_[1].rows;
	roi_rectangle.height = inc_images_[1].rows;

	roi_rectangle.width = inc_images_[0].cols;

	//LOGLN("second image starting height : " << second_img_size.y);
	
	Mat temp_image_holder;
	inc_images_[0](Rect(roi_rectangle)).copyTo(temp_image_holder);

	Mat temp;
	inc_images_[0].copyTo(temp);

	cv::rectangle(temp, Rect(roi_rectangle), Scalar(0, 0, 255), 3, LINE_8, 0);

	String output_location = "../opencv_image_stitching/Images/Results/roi" + to_string(current_iteration_ +1) + ".jpg";
	cv::imwrite(output_location, temp);
	
	temp_image_holder.copyTo(temp_images[0]);
	inc_images_[1].copyTo(temp_images[1]);	

	/*LOGLN("temp image size 0: " << temp_images[0].size);
	LOGLN("temp image size 1: " << temp_images[1].size);*/

	
	String output_location1 = "../opencv_image_stitching/Images/Results/PROSAC11_temp_images[0]#" + to_string(current_iteration_) + "_0.5.jpg";
	cv::imwrite(output_location1, temp_images[0]);
	String output_location2 = "../opencv_image_stitching/Images/Results/PROSAC11_temp_images[1]#" + to_string(current_iteration_) + "_0.5.jpg";
	cv::imwrite(output_location2, temp_images[1]);
	
	current_iteration_ = current_iteration_ + 1;

	return temp_images;
}

void FeatureFindMatch::set_rectangle_info(int rows, int columns, float overlap, int desired_occupied) {
	desired_rectangle_.rows = rows;
	desired_rectangle_.columns = columns;
	desired_rectangle_.image_overlap = overlap;
	desired_rectangle_.desired_occupied = desired_occupied;
}

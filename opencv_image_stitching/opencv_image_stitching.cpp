///*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                          License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//
//M*/
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include "opencv2/opencv_modules.hpp"
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/core/utility.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching/detail/autocalib.hpp"
#include "opencv2/stitching/detail/blenders.hpp"
#include "opencv2/stitching/detail/timelapsers.hpp"
#include "opencv2/stitching/detail/camera.hpp"
#include "opencv2/stitching/detail/exposure_compensate.hpp"
#include "opencv2/stitching/detail/matchers.hpp"
#include "opencv2/stitching/detail/motion_estimators.hpp"
#include "opencv2/stitching/detail/seam_finders.hpp"
#include "opencv2/stitching/detail/warpers.hpp"
#include "opencv2/stitching/warpers.hpp"
#include "opencv2/core/ocl.hpp"

#include <algorithm>
#include "Logger.h"

#pragma region macro_definitions
#ifdef _WIN32
#define WINPAUSE system("pause")
#endif

#define OUTPUT_TRUE 1
#define ENABLE_LOG 1
#define LOG(msg) std::cout << msg
#define LOGLN(msg) std::cout << msg << std::endl

#pragma endregion

#pragma region namespaces
using namespace std;
using namespace cv;
using namespace cv::detail;
using namespace clogging;

#pragma endregion

#pragma region param_declerations
INIT_CLOGGING;
// Default command line args
vector<String> img_names;
bool preview = false;
bool try_cuda = true; //for dedicated GPU-s
double work_megapix = 0.6;
double seam_megapix = 0.1;
double compose_megapix = -1;
float conf_thresh = 0.5f; //default 1.f
string features_type = "orb"; //default surf, but it doesn't work
string matcher_type = "affine"; //def: homography
string estimator_type = "homography";
string ba_cost_func = "ray";
string ba_refine_mask = "xxxxx";
bool do_wave_correct = true;
WaveCorrectKind wave_correct = detail::WAVE_CORRECT_HORIZ;
bool save_graph = false;
std::string save_graph_to;
string warp_type = "spherical";
int expos_comp_type = ExposureCompensator::GAIN;
float match_conf = 0.3f;
string seam_find_type = "no";
int blend_type = Blender::MULTI_BAND;
int timelapse_type = Timelapser::AS_IS;
float blend_strength = 5;
string result_name = "result.jpg";
bool timelapse = false;
int range_width = -1;

double work_scale = 1, seam_scale = 1, compose_scale = 1, seam_work_aspect = 1;
bool is_work_scale_set = false, is_seam_scale_set = false, is_compose_scale_set = false;

#pragma endregion

vector<String> readImages() {
	vector<String> photos;
	//glob("C:/photos/T3A/*.jpg", photos, false);
	glob("C:/photos/BLADE/*.JPG", photos, false);
	string file_name = "C:/photos/T4D/KEYPOINTS/test";

	cout << photos.size() << endl;
	WINPAUSE;
	for (int i = 0; i < photos.size(); i++) {
		img_names.push_back(photos[i]);
	}

	return img_names;
}

void uploadImages(vector<Mat>& images, int num_images, vector<Size> &full_img_sizes) {

	Mat full_img, img;
	for (int i = 0; i < num_images; ++i) {
		full_img = imread(img_names[i]);
		full_img_sizes[i] = full_img.size();


		if (work_megapix < 0) {
			img = full_img;
			work_scale = 1;
			is_work_scale_set = true;
		}
		else {
			if (!is_work_scale_set) {
				work_scale = min(1.0, sqrt(work_megapix * 1e6 / full_img.size().area()));
				is_work_scale_set = true;
			}
			resize(full_img, img, Size(), work_scale, work_scale, INTER_LINEAR_EXACT);
		}
		if (!is_seam_scale_set) {
			seam_scale = min(1.0, sqrt(seam_megapix * 1e6 / full_img.size().area()));
			seam_work_aspect = seam_scale / work_scale;
			is_seam_scale_set = true;
		}


		images[i] = img.clone();
	}
	full_img.release();
	img.release();
}

void findFeatures(vector<ImageFeatures> &features, vector<Mat> &images,
	vector<Size> &full_img_sizes, int num_images, vector<vector<KeyPoint>> &key_points_vector,
	vector<KeyPoint> &key_points, vector<ImageFeatures> &features_new) {
	LOGLN("Finding features...");

	Ptr<FeaturesFinder> finder = makePtr<OrbFeaturesFinder>();

	string features_out = "Features in image #";
	string new_features = "Number of features after ORB recustruct: ";
	for (int i = 0; i < num_images; ++i) {

		features_out = "Features in image #";
		new_features = "Number of features after ORB recustruct: ";
		vector<Mat> featureimages(num_images);
		try {
			(*finder)(images[i], features[i]);
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
			detector->detect(images[i], features_new[i].keypoints);
			extractor->compute(images[i], features_new[i].keypoints, features_new[i].descriptors);
		}
		catch (const std::exception& e) {
			cout << e.what() << endl;
		}

		cout << features[i].keypoints.size() << endl;
		cout << features_new[i].keypoints.size() << endl;


		WINPAUSE;
		
		
		/*Mat img_keypoints;
		key_points_vector.push_back(key_points);
		drawKeypoints(images[i], key_points, img_keypoints, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		Mat img_keypoints_small;
		resize(img_keypoints, img_keypoints_small, Size(), 0.4, 0.4, INTER_LINEAR_EXACT);
		imshow("Key points detected", img_keypoints_small);		
		waitKey(0);*/
	}
	finder->collectGarbage();
}

void matchFeatures(vector<Mat> &images, vector<ImageFeatures> &features,
	int num_images, vector<ImageFeatures> &features_new) {

	vector<MatchesInfo> pairwise_matches(num_images);
	Mat output_img;
	String output_location = "C:/photos/matching_output/test_";

	Ptr<FeaturesMatcher> current_matcher;
	Mat img_1 = images[0];
	Mat img_2 = images[1];

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

		string image_length = "Images length: " + to_string(images.size());
		string keypoints_length_1 = "Keypoints_1 length: " + to_string(keypoints_1.size());
		string keypoints_length_2 = "Keypoints_2 length: " + to_string(keypoints_2.size());

		CLOG(image_length, Verbosity::INFO);
		CLOG(keypoints_length_1, Verbosity::INFO);
		CLOG(keypoints_length_2, Verbosity::INFO);

		cout << "Images length: " << images.size() << endl;
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
		WINPAUSE;
		current_matcher->collectGarbage();

#ifdef OUTPUT_TRUE 1
		imwrite(output_location, outImg);
#endif // OUTPUT_TRUE 1

	}
}

int main() {
#if ENABLE_LOG
	int64 app_start_time = getTickCount();
#endif

	ADD_FILE("clogging.log");

	cv::ocl::setUseOpenCL(false);

	vector<String> images_names = readImages();
	int num_images = static_cast <int> (images_names.size());

	vector<ImageFeatures> features(num_images);
	vector<Mat> images(num_images);
	vector<vector<KeyPoint>> key_points_vector;
	vector<Size> full_img_sizes(num_images);
	vector<KeyPoint> key_points;
	vector<ImageFeatures> features_new(num_images);

	uploadImages(images, num_images, full_img_sizes);
	findFeatures(features, images, full_img_sizes, num_images, key_points_vector, key_points, features_new);
	matchFeatures(images, features, num_images, features_new);

}


	LOG("Pairwise matching\n");

	vector<MatchesInfo> pairwise_matches;
	Ptr<FeaturesMatcher> matcher;

	if (matcher_type == "affine") {
		matcher = makePtr<AffineBestOf2NearestMatcher>(false, try_cuda, match_conf);
		cout << "affine" << endl;
		WINPAUSE;
	}
	else if (range_width == -1) {
		matcher = makePtr<BestOf2NearestMatcher>(try_cuda, match_conf);
		cout << "range width -1" << endl;
		WINPAUSE;
	}
	else {
		matcher = makePtr<BestOf2NearestRangeMatcher>(range_width, try_cuda, match_conf);
		WINPAUSE;
	}
	WINPAUSE;
	try {
		(*matcher)(features, pairwise_matches);
		
	}
	catch (const std::exception& e) {
		cout << e.what() << endl;
		WINPAUSE;
	}
	matcher->collectGarbage();


	LOGLN("Pairwise matching, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec");	

	// Check if we should save matches graph
	if (true) {
		LOGLN("Saving matches graph...");
		ofstream f(save_graph_to.c_str());
		f << matchesGraphAsString(img_names, pairwise_matches, conf_thresh);
	}

	// Leave only images we are sure are from the same panorama
	vector<int> indices = leaveBiggestComponent(features, pairwise_matches, conf_thresh);
	vector<Mat> img_subset;
	vector<String> img_names_subset;
	vector<Size> full_img_sizes_subset;

	for (size_t i = 0; i < indices.size(); ++i) {
		img_names_subset.push_back(img_names[indices[i]]);
		img_subset.push_back(images[indices[i]]);
		full_img_sizes_subset.push_back(full_img_sizes[indices[i]]);
	}

	images = img_subset;
	img_names = img_names_subset;
	full_img_sizes = full_img_sizes_subset;

	// Check if we still have enough images
	num_images = static_cast<int>(img_names.size());
	if (num_images < 2) {
		LOGLN("Need more images");
		WINPAUSE;
		return -1;
	}

	Ptr<Estimator> estimator;
	if (estimator_type == "affine")
		estimator = makePtr<AffineBasedEstimator>();
	else
		estimator = makePtr<HomographyBasedEstimator>();

	vector<CameraParams> cameras;
	if (!(*estimator)(features, pairwise_matches, cameras)) {
		cout << "Homography estimation failed.\n";
		WINPAUSE;

		return -1;
	}

	for (size_t i = 0; i < cameras.size(); ++i) {
		Mat R;
		cameras[i].R.convertTo(R, CV_32F);
		cameras[i].R = R;
		LOGLN("Initial camera intrinsics #" << indices[i] + 1 << ":\nK:\n" << cameras[i].K() << "\nR:\n" << cameras[i].R);
	}

	WINPAUSE;
	Ptr<detail::BundleAdjusterBase> adjuster;
	if (ba_cost_func == "reproj") adjuster = makePtr<detail::BundleAdjusterReproj>();
	else if (ba_cost_func == "ray") adjuster = makePtr<detail::BundleAdjusterRay>();
	else if (ba_cost_func == "affine") adjuster = makePtr<detail::BundleAdjusterAffinePartial>();
	else if (ba_cost_func == "no") adjuster = makePtr<NoBundleAdjuster>();
	else {
		cout << "Unknown bundle adjustment cost function: '" << ba_cost_func << "'.\n";
		WINPAUSE;

		return -1;
	}

	try {

		adjuster->setConfThresh(conf_thresh);
		Mat_<uchar> refine_mask = Mat::zeros(3, 3, CV_8U);
		if (ba_refine_mask[0] == 'x') refine_mask(0, 0) = 1;
		if (ba_refine_mask[1] == 'x') refine_mask(0, 1) = 1;
		if (ba_refine_mask[2] == 'x') refine_mask(0, 2) = 1;
		if (ba_refine_mask[3] == 'x') refine_mask(1, 1) = 1;
		if (ba_refine_mask[4] == 'x') refine_mask(1, 2) = 1;
		adjuster->setRefinementMask(refine_mask);
		if (!(*adjuster)(features, pairwise_matches, cameras)) {
			cout << "Camera parameters adjusting failed.\n";
			WINPAUSE;

			return -1;
		}
	}
	catch (const std::exception& e) {
		cout << e.what() << endl;
		WINPAUSE;
	}

	WINPAUSE;
	// Find median focal length

	vector<double> focals;
	for (size_t i = 0; i < cameras.size(); ++i) {
		LOGLN("Camera #" << indices[i] + 1 << ":\nK:\n" << cameras[i].K() << "\nR:\n" << cameras[i].R);
		focals.push_back(cameras[i].focal);
	}

	sort(focals.begin(), focals.end());
	float warped_image_scale;
	if (focals.size() % 2 == 1)
		warped_image_scale = static_cast<float>(focals[focals.size() / 2]);
	else
		warped_image_scale = static_cast<float>(focals[focals.size() / 2 - 1] + focals[focals.size() / 2]) * 0.5f;

	if (do_wave_correct) {
		vector<Mat> rmats;
		for (size_t i = 0; i < cameras.size(); ++i)
			rmats.push_back(cameras[i].R.clone());
		waveCorrect(rmats, wave_correct);
		for (size_t i = 0; i < cameras.size(); ++i)
			cameras[i].R = rmats[i];
	}

	LOGLN("Warping images (auxiliary)... ");
#if ENABLE_LOG
	t = getTickCount();
#endif

	vector<Point> corners(num_images);
	vector<UMat> masks_warped(num_images);
	vector<UMat> images_warped(num_images);
	vector<Size> sizes(num_images);
	vector<UMat> masks(num_images);

	// Prepare images masks
	for (int i = 0; i < num_images; ++i) {
		masks[i].create(images[i].size(), CV_8U);
		masks[i].setTo(Scalar::all(255));
	}

	// Warp images and their masks

	Ptr<WarperCreator> warper_creator;
#ifdef HAVE_OPENCV_CUDAWARPING
	if (try_cuda && cuda::getCudaEnabledDeviceCount() > 0) {
		if (warp_type == "plane")
			warper_creator = makePtr<cv::PlaneWarperGpu>();
		else if (warp_type == "cylindrical")
			warper_creator = makePtr<cv::CylindricalWarperGpu>();
		else if (warp_type == "spherical")
			warper_creator = makePtr<cv::SphericalWarperGpu>();
	}
	else
#endif
	{
		if (warp_type == "plane")
			warper_creator = makePtr<cv::PlaneWarper>();
		else if (warp_type == "affine")
			warper_creator = makePtr<cv::AffineWarper>();
		else if (warp_type == "cylindrical")
			warper_creator = makePtr<cv::CylindricalWarper>();
		else if (warp_type == "spherical")
			warper_creator = makePtr<cv::SphericalWarper>();
		else if (warp_type == "fisheye")
			warper_creator = makePtr<cv::FisheyeWarper>();
		else if (warp_type == "stereographic")
			warper_creator = makePtr<cv::StereographicWarper>();
		else if (warp_type == "compressedPlaneA2B1")
			warper_creator = makePtr<cv::CompressedRectilinearWarper>(2.0f, 1.0f);
		else if (warp_type == "compressedPlaneA1.5B1")
			warper_creator = makePtr<cv::CompressedRectilinearWarper>(1.5f, 1.0f);
		else if (warp_type == "compressedPlanePortraitA2B1")
			warper_creator = makePtr<cv::CompressedRectilinearPortraitWarper>(2.0f, 1.0f);
		else if (warp_type == "compressedPlanePortraitA1.5B1")
			warper_creator = makePtr<cv::CompressedRectilinearPortraitWarper>(1.5f, 1.0f);
		else if (warp_type == "paniniA2B1")
			warper_creator = makePtr<cv::PaniniWarper>(2.0f, 1.0f);
		else if (warp_type == "paniniA1.5B1")
			warper_creator = makePtr<cv::PaniniWarper>(1.5f, 1.0f);
		else if (warp_type == "paniniPortraitA2B1")
			warper_creator = makePtr<cv::PaniniPortraitWarper>(2.0f, 1.0f);
		else if (warp_type == "paniniPortraitA1.5B1")
			warper_creator = makePtr<cv::PaniniPortraitWarper>(1.5f, 1.0f);
		else if (warp_type == "mercator")
			warper_creator = makePtr<cv::MercatorWarper>();
		else if (warp_type == "transverseMercator")
			warper_creator = makePtr<cv::TransverseMercatorWarper>();
	}

	if (!warper_creator) {
		cout << "Can't create the following warper '" << warp_type << "'\n";
		return 1;
	}

	Ptr<RotationWarper> warper = warper_creator->create(static_cast<float>(warped_image_scale * seam_work_aspect));

	for (int i = 0; i < num_images; ++i) {
		Mat_<float> K;
		cameras[i].K().convertTo(K, CV_32F);
		float swa = (float)seam_work_aspect;
		K(0, 0) *= swa; K(0, 2) *= swa;
		K(1, 1) *= swa; K(1, 2) *= swa;

		corners[i] = warper->warp(images[i], K, cameras[i].R, INTER_LINEAR, BORDER_REFLECT, images_warped[i]);
		sizes[i] = images_warped[i].size();

		warper->warp(masks[i], K, cameras[i].R, INTER_NEAREST, BORDER_CONSTANT, masks_warped[i]);
	}

	vector<UMat> images_warped_f(num_images);
	for (int i = 0; i < num_images; ++i)
		images_warped[i].convertTo(images_warped_f[i], CV_32F);

	LOGLN("Warping images, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec");

	////////////////////////////////////////////////////////////////////////////////////////////////////

	Ptr<ExposureCompensator> compensator = ExposureCompensator::createDefault(expos_comp_type);
	compensator->feed(corners, images_warped, masks_warped);


	Ptr<SeamFinder> seam_finder;
	if (seam_find_type == "no") {
		seam_finder = makePtr<detail::NoSeamFinder>();
	}
	else if (seam_find_type == "voronoi")
		seam_finder = makePtr<detail::VoronoiSeamFinder>();
	else if (seam_find_type == "gc_color") {
#ifdef HAVE_OPENCV_CUDALEGACY
		if (try_cuda && cuda::getCudaEnabledDeviceCount() > 0)
			seam_finder = makePtr<detail::GraphCutSeamFinderGpu>(GraphCutSeamFinderBase::COST_COLOR);
		else
#endif
			seam_finder = makePtr<detail::GraphCutSeamFinder>(GraphCutSeamFinderBase::COST_COLOR);
	}
	else if (seam_find_type == "gc_colorgrad") {
#ifdef HAVE_OPENCV_CUDALEGACY
		if (try_cuda && cuda::getCudaEnabledDeviceCount() > 0)
			seam_finder = makePtr<detail::GraphCutSeamFinderGpu>(GraphCutSeamFinderBase::COST_COLOR_GRAD);
		else
#endif
			seam_finder = makePtr<detail::GraphCutSeamFinder>(GraphCutSeamFinderBase::COST_COLOR_GRAD);
	}
	else if (seam_find_type == "dp_color")
		seam_finder = makePtr<detail::DpSeamFinder>(DpSeamFinder::COLOR);
	else if (seam_find_type == "dp_colorgrad")
		seam_finder = makePtr<detail::DpSeamFinder>(DpSeamFinder::COLOR_GRAD);
	if (!seam_finder) {
		cout << "Can't create the following seam finder '" << seam_find_type << "'\n";
		return 1;
	}

	seam_finder->find(images_warped_f, corners, masks_warped);

	// Release unused memory
	images.clear();
	images_warped.clear();
	images_warped_f.clear();
	masks.clear();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	LOGLN("Compositing...");
#if ENABLE_LOG
	t = getTickCount();
#endif

	Mat img_warped, img_warped_s;
	Mat dilated_mask, seam_mask, mask, mask_warped;
	Ptr<Blender> blender;
	Ptr<Timelapser> timelapser;
	double compose_seam_aspect = 1;
	double compose_work_aspect = 1;

	try {
		for (int img_idx = 0; img_idx < num_images; ++img_idx) {
			LOGLN("Compositing image #" << indices[img_idx] + 1);

			// Read image and resize it if necessary
			full_img = imread(img_names[img_idx]);
			if (!is_compose_scale_set) {
				if (compose_megapix > 0)
					compose_scale = min(1.0, sqrt(compose_megapix * 1e6 / full_img.size().area()));
				is_compose_scale_set = true;

				// Compute relative scales
				compose_seam_aspect = compose_scale / seam_scale;
				compose_work_aspect = compose_scale / work_scale;

				// Update warped image scale
				warped_image_scale *= static_cast<float>(compose_work_aspect);
				warper = warper_creator->create(warped_image_scale);

				// Update corners and sizes
				for (int i = 0; i < num_images; ++i) {
					// Update intrinsics
					cameras[i].focal *= compose_work_aspect;
					cameras[i].ppx *= compose_work_aspect;
					cameras[i].ppy *= compose_work_aspect;

					// Update corner and size
					Size sz = full_img_sizes[i];
					if (std::abs(compose_scale - 1) > 1e-1) {
						sz.width = cvRound(full_img_sizes[i].width * compose_scale);
						sz.height = cvRound(full_img_sizes[i].height * compose_scale);
					}

					Mat K;
					cameras[i].K().convertTo(K, CV_32F);
					Rect roi = warper->warpRoi(sz, K, cameras[i].R);
					corners[i] = roi.tl();
					sizes[i] = roi.size();
				}
			}
			if (abs(compose_scale - 1) > 1e-1)
				resize(full_img, img, Size(), compose_scale, compose_scale, INTER_LINEAR_EXACT);
			else
				img = full_img;
			full_img.release();
			Size img_size = img.size();

			Mat K;
			cameras[img_idx].K().convertTo(K, CV_32F);

			// Warp the current image
			warper->warp(img, K, cameras[img_idx].R, INTER_LINEAR, BORDER_REFLECT, img_warped);

			// Warp the current image mask
			mask.create(img_size, CV_8U);
			mask.setTo(Scalar::all(255));
			warper->warp(mask, K, cameras[img_idx].R, INTER_NEAREST, BORDER_CONSTANT, mask_warped);

			// Compensate exposure
			compensator->apply(img_idx, corners[img_idx], img_warped, mask_warped);

			img_warped.convertTo(img_warped_s, CV_16S);
			img_warped.release();
			img.release();
			mask.release();

			dilate(masks_warped[img_idx], dilated_mask, Mat());
			resize(dilated_mask, seam_mask, mask_warped.size(), 0, 0, INTER_LINEAR_EXACT);
			mask_warped = seam_mask & mask_warped;

			if (!blender && !timelapse) {
				blender = Blender::createDefault(blend_type, try_cuda);
				Size dst_sz = resultRoi(corners, sizes).size();
				float blend_width = sqrt(static_cast<float>(dst_sz.area())) * blend_strength / 100.f;
				if (blend_width < 1.f)
					blender = Blender::createDefault(Blender::NO, try_cuda);
				else if (blend_type == Blender::MULTI_BAND) {
					MultiBandBlender* mb = dynamic_cast<MultiBandBlender*>(blender.get());
					mb->setNumBands(static_cast<int>(ceil(log(blend_width) / log(2.)) - 1.));
					LOGLN("Multi-band blender, number of bands: " << mb->numBands());
				}
				else if (blend_type == Blender::FEATHER) {
					FeatherBlender* fb = dynamic_cast<FeatherBlender*>(blender.get());
					fb->setSharpness(1.f / blend_width);
					LOGLN("Feather blender, sharpness: " << fb->sharpness());
				}
				blender->prepare(corners, sizes);
			}
			else if (!timelapser && timelapse) {
				timelapser = Timelapser::createDefault(timelapse_type);
				timelapser->initialize(corners, sizes);
			}

			// Blend the current image
			if (timelapse) {
				timelapser->process(img_warped_s, Mat::ones(img_warped_s.size(), CV_8UC1), corners[img_idx]);
				String fixedFileName;
				size_t pos_s = String(img_names[img_idx]).find_last_of("/\\");
				if (pos_s == String::npos) {
					fixedFileName = "fixed_" + img_names[img_idx];
				}
				else {
					fixedFileName = "fixed_" + String(img_names[img_idx]).substr(pos_s + 1, String(img_names[img_idx]).length() - pos_s);
				}
				imwrite(fixedFileName, timelapser->getDst());
			}
			else {
				blender->feed(img_warped_s, mask_warped, corners[img_idx]);
			}
		}
	}
	catch (const std::exception& e) {
		cout << e.what() << endl;
	}

	

	if (!timelapse) {
		Mat result, result_mask;
		blender->blend(result, result_mask);

		LOGLN("Compositing, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec");
		Mat smallresult;
		resize(result, smallresult, Size(), 0.2, 0.2);
		imwrite(file_name, smallresult);

		//imshow(result_name, smallresult);
		waitKey(0);
	}

	LOGLN("Finished, total time: " << ((getTickCount() - app_start_time) / getTickFrequency()) << " sec");
	waitKey(0);
	while (true) {

	}
	return 0;
}

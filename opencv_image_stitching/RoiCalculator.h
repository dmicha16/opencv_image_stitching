#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>
#include<cstdlib>
#include<ctime>

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

#pragma region namespaces
using namespace std;
using namespace cv;
using namespace cv::detail;
#pragma endregion

#ifdef _WIN32
#define WINPAUSE system("pause")
#endif

typedef struct RowDefiner {
	Point2f left;
	Point2f right;
	Point2f top_left;
};

typedef struct Rectengales {
	vector<Rect> rectangles;
	vector<RowDefiner> row_definitions;
	void desginate_rectangles(int desired_rect);
	void populate_rectangles(int height_offset, int desired_cols);
	void reset_rectangles();
};

typedef struct MatchedKeyPoint {
	vector<Point2f> image_1;
	vector<Point2f> image_2;
};

class RoiCalculator {

public:	
	RoiCalculator();
	~RoiCalculator();

	void set_image(Mat inc_image);
	void calculate_roi(int desired_cols, int desired_rows, float overlap);
	bool check_keypoint();
	void set_matched_keypoints(MatchedKeyPoint inc_matched_keypoints);
	int num_occupied_rects();

private:
	int num_rect_;
	Mat image_;
	int num_images_;

	Rectengales rectangles_s_;	
	MatchedKeyPoint matched_keypoints_;

	RowDefiner populate_row_definer_(int img_width, unsigned int start_height, int offset);
	vector<RowDefiner> row_definitions_;

	void write_roi_(float min_height);
};


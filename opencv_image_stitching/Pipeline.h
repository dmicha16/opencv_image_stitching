#pragma once
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

#pragma region namespaces
using namespace std;
using namespace cv;
using namespace cv::detail;;

#pragma endregion

#ifdef _WIN32
#define WINPAUSE system("pause")
#endif

class Pipeline {
public:

	vector<String> img_names;
	int num_images;
	vector<Mat> images;

	vector<Mat> readImages(string path);
	vector<Mat> uploadImages(vector<Mat> images, int num_images, vector<Size> full_img_sizes);
	Pipeline();
	~Pipeline();
};

 
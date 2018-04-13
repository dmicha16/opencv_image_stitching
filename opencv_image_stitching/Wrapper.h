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
#include "Logger.h"

#pragma region namespaces
using namespace std;
using namespace cv;
using namespace cv::detail;
using namespace clogging;

#pragma endregion

#ifdef _WIN32
#define WINPAUSE system("pause")
#endif

#define OUTPUT_TRUE 1
#define ENABLE_LOG 1
#define LOG(msg) std::cout << msg
#define LOGLN(msg) std::cout << msg << std::endl

class Wrapper {
public:
	INIT_CLOGGING;
	
	vector<String> returnImageNames();
	vector<Mat> readImages(string path);
	vector<Mat> uploadImages(vector<Mat> images, vector<Size> full_img_sizes);
	Wrapper();
	~Wrapper();

private:
	vector<String> img_names;
	int num_images;
};

 
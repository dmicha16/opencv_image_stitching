#include "Pipeline.h"
#include "Features.h"
#include "CameraParameters.h"

INIT_CLOGGING;

int main() {

	ADD_FILE("clogging.log");
	cv::ocl::setUseOpenCL(false);

	Pipeline pipeline;
	string path = "C:/photos/BLADE/*.JPG";
	vector<Mat> images = pipeline.readImages(path);

	CameraParameters camera_params(images);
	vector<Mat> undistorted_images = camera_params.returnUndistortedImages();
	Features features(undistorted_images);
	
}
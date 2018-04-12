#include "Pipeline.h"
#include "Features.h"
#include "CameraParameters.h"
#include "Warping.h"

INIT_CLOGGING;

int main() {

	ADD_FILE("clogging.log");
	cv::ocl::setUseOpenCL(false);

	Pipeline pipeline;
	string path = "C:/photos/BLADE/*.JPG";
	vector<Mat> images = pipeline.readImages(path);

	CameraParameters camera_params(images);
	vector<Mat> undistorted_images = camera_params.returnUndistortedImages();
	//Features features(undistorted_images);

	vector<CameraParams> cameras = camera_params.returnCamera();
	Warping warping(cameras, undistorted_images);
	vector<UMat> warped_images = warping.returnImagesWarped();
	try {
		imshow("warped:", warped_images[1]);
	}
	catch (const std::exception& e) {
		cout << e.what() << endl;
	}
	WINPAUSE;
	waitKey(0);
	
	
}
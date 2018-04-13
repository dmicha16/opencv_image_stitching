#include "Wrapper.h"
#include "FeatureFindMatch.h"
#include "Undistorter.h"
#include "Warping.h"

INIT_CLOGGING;

int main() {

	ADD_FILE("clogging.log");
	cv::ocl::setUseOpenCL(false);

	Wrapper wrapper;
	string path = "C:/photos/BLADE/*.JPG";
	vector<Mat> images = wrapper.readImages(path);
	vector<String> img_names = wrapper.returnImageNames();

	Undistorter camera_params(images);
	vector<Mat> undistorted_images = camera_params.returnUndistortedImages();
	FeatureFindMatch features(undistorted_images);

	vector<CameraParams> cameras = camera_params.returnCamera();
	Warping warping(cameras, undistorted_images);
	vector<UMat> warped_images = warping.returnImagesWarped();
	try {
		imshow("warped:", warped_images[1]);
	}
	catch (const std::exception& e) {
		cout << e.what() << endl;
	}

	Ptr<detail::BundleAdjusterBase> adjuster;
	WINPAUSE;
	waitKey(0);
	
	
}
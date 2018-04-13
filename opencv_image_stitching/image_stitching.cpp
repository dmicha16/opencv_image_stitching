#include "Wrapper.h"
#include "FeatureFindMatch.h"
#include "Undistorter.h"
#include "Warping.h"



int main() {
	INIT_CLOGGING;

	ADD_FILE("clogging.log");
	cv::ocl::setUseOpenCL(false);

	Undistorter undistorter;
	//FeatureFindMatch features(undistorted_images);

	//Warping warping(cameras, undistorted_images);
	//vector<UMat> warped_images = warping.returnImagesWarped();
	/*try {
		imshow("warped:", warped_images[1]);
	}
	catch (const std::exception& e) {
		cout << e.what() << endl;
	}*/

	waitKey(0);
	
	
}
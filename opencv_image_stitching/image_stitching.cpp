#include "Wrapper.h"
#include "FeatureFindMatch.h"
#include "ImageReader.h"
#include "Undistorter.h"
#include "Warping.h"



int main() {
	INIT_CLOGGING;

	ADD_FILE("clogging.log");
	cv::ocl::setUseOpenCL(false);

	CLOG("test");

	/******************************************* WRAPPER *******************************************/
	
	ImageReader image_reader;
	vector<Mat> raw_images = image_reader.get_images();
	


	/**************************************** UNDISTORTION *****************************************/
	
	//Undistorter undistorter;
	//vector<Mat> undist_images = undistorter.undistort_images(raw_images);
	
	

	/****************************************** FEATURES *******************************************/
	
	FeatureFindMatch finder;
	finder.find_features(raw_images);
	
	

	/******************************************* WARPING *******************************************/
	
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
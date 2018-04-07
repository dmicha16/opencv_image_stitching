#include "Pipeline.h"
#include "Features.h"

INIT_CLOGGING;

int main() {

	ADD_FILE("clogging.log");
	cv::ocl::setUseOpenCL(false);

	Pipeline pipeline;
	string path = "C:/photos/BLADE/*.JPG";
	vector<Mat> images = pipeline.readImages(path);

	Features features(images);
}
#pragma once
#include "Pipeline.h"
class Composition :
	public Pipeline {
public:
	Composition(vector<Mat> inc_images, vector<String> img_names, vector<CameraParams> cameras, vector<Point> corners,
		float warped_image_scale, int expos_comp_type, vector<UMat> images_warped, vector<UMat> masks_warped);
	void imageComposition(vector<Mat> images, vector<String> img_names, vector<CameraParams> cameras, vector<Point> corners,
		float warped_image_scale, int expos_comp_type, vector<UMat> images_warped, vector<UMat> masks_warped);
	Mat returnFinalComp();
	~Composition();
};


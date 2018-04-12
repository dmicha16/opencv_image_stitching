#pragma once
#include "Pipeline.h"
class Warping :
	public Pipeline {
public:
	Warping(vector<CameraParams> cameras, vector<Mat> images);
	void total_warper(vector<CameraParams> cameras, vector<Mat> images, double seam_work_aspect);
	vector<Point> returnCorners();
	vector<UMat> returnImagesWarped();
	vector<UMat> returnMasksWarped();

	~Warping();

private:
	vector<Point> corners_temp;
	vector<UMat> images_warped_temp;
	vector<UMat> masks_warped_temp;
	//vector<Size> _update_sizes(vector<Size> sizes, vector<Size> full_img_sizes, vector<CameraParams> cameras, Ptr<RotationWarper> warper);

};


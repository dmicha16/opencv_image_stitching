#pragma once
#include "Pipeline.h"
class Warping :
	public Pipeline {
public:
	Warping();

	void set_variables(double seam_scale, double work_scale, float warped_image_scale, float seam_work_aspect, vector<String> img_names);
	bool total_warper(vector<CameraParams> cameras, vector<Mat> images, Mat full_img);

	~Warping();

private:
	
	bool is_compose_scale_set = false;
	bool timelapse = false;
	int num_images;
	int expos_comp_type = ExposureCompensator::GAIN_BLOCKS;
	int blend_type = Blender::MULTI_BAND;
	int timelapse_type = Timelapser::AS_IS;
	double compose_megapix = -1;
	double compose_seam_aspect = compose_scale / seam_scale;
	double compose_work_aspect = compose_scale / work_scale;
	double work_scale;
	double seam_scale;
	double compose_scale;
	float warped_image_scale;
	float seam_work_aspect;
	float blend_strength = 5;
	vector<String> img_names;
	

	vector<UMat> _warperish_func(Ptr<RotationWarper> warper, vector<CameraParams> cameras, vector<Mat> images);
	void _set_compose_scale(Mat full_img);
	vector<Point> _update_corner(vector<Point> corners, vector<Size> full_img_sizes, vector<CameraParams> cameras, Ptr<RotationWarper> warper);
	vector<Size> _update_sizes(vector<Size> sizes, vector<Size> full_img_sizes, vector<CameraParams> cameras, Ptr<RotationWarper> warper);

};


#include "Warping.h"

Warping::Warping(vector<CameraParams> cameras, vector<Mat> images) {
	double work_scale = 1, seam_scale = 1, compose_scale = 1, seam_work_aspect = 1;
	total_warper(cameras, images, seam_work_aspect);
}

void Warping::total_warper(vector<CameraParams> cameras, vector<Mat> images, double seam_work_aspect) {
	int num_images = static_cast<int>(images.size());
	float warped_image_scale = 0;
	vector<Point> corners(num_images);
	vector<UMat> masks_warped(num_images);
	vector<UMat> images_warped(num_images);
	vector<Size> sizes(num_images);
	vector<UMat> masks(num_images);

	for (int i = 0; i < num_images; ++i) {
		masks[i].create(images[i].size(), CV_8U);
		masks[i].setTo(Scalar::all(255));
	}

	Ptr<WarperCreator> warper_creator;
	warper_creator = makePtr<cv::AffineWarper>();
	Ptr<RotationWarper> warper = warper_creator->create(static_cast<float>(warped_image_scale * seam_work_aspect));
	cout << cameras.size() << endl;
	WINPAUSE;
	for (int i = 0; i < num_images; ++i) {
		Mat_<float> K;
		cameras[i].K().convertTo(K, CV_32F); // Converts an array to another data type with optional scaling. K = output
		float swa = (float)seam_work_aspect;
		K(0, 0) *= swa; K(0, 2) *= swa;
		K(1, 1) *= swa; K(1, 2) *= swa;

		corners[i] = warper->warp(images[i], K, cameras[i].R, INTER_LINEAR, BORDER_REFLECT, images_warped[i]);
		sizes[i] = images_warped[i].size();

		warper->warp(masks[i], K, cameras[i].R, INTER_NEAREST, BORDER_CONSTANT, masks_warped[i]); // Warps the current image
	}
	
	vector<UMat> images_warped_f(num_images);
	for (int i = 0; i < num_images; ++i) {
		images_warped[i].convertTo(images_warped_f[i], CV_32F);
		cout << images_warped[i].size() << endl;
	}

	for (size_t i = 0; i < images_warped.size(); i++) {
		images_warped_temp.push_back(images_warped[i]);
	}

	for (size_t i = 0; i < corners.size(); i++) {
		corners_temp.push_back(corners[i]);
	}

	/*Ptr<ExposureCompensator> compensator = ExposureCompensator::createDefault(this->expos_comp_type);
	compensator->feed(corners, images_warped, masks_warped);

	Ptr<SeamFinder> seam_finder;
	//seam_finder = makePtr<detail::NoSeamFinder>(); // Not sure which one we should use
	seam_finder = makePtr<detail::VoronoiSeamFinder>();
	seam_finder->find(images_warped_f, corners, masks_warped);

	// Release unused memory
	images.clear();
	images_warped.clear();
	images_warped_f.clear();
	masks.clear();


	Mat img_warped, img_warped_s;
	Mat dilated_mask, seam_mask, mask, mask_warped;
	Ptr<Blender> blender;
	Ptr<Timelapser> timelapser;

	for (int img_id = 0; img_id < this->num_images; ++img_id) {

		// Read image and resize it if necessary
		full_img = imread(img_names[img_id]);
		if (!is_compose_scale_set) {
			
			_set_compose_scale(full_img);

			// Compute relative scales
			//compose_seam_aspect = this->compose_scale / this->seam_scale; // is done in the h.file
			//compose_work_aspect = this->compose_scale / this->work_scale; // is done in the h.file

			// Update warped image scale
			warped_image_scale *= static_cast<float>(this->compose_work_aspect);
			warper = warper_creator->create(warped_image_scale);

			// Update corners and sizes
			corners = _update_corner(corners, full_img_sizes, cameras, warper);
			sizes = _update_sizes(sizes, full_img_sizes, cameras, warper);
		}

		Mat img;
		if (abs(this->compose_scale - 1) > 1e-1) {
			resize(full_img, img, Size(), this->compose_scale, this->compose_scale, INTER_LINEAR_EXACT);
		}
		else {
			img = full_img;
		}
		full_img.release();
		Size img_size = img.size();

		Mat K;
		cameras[img_id].K().convertTo(K, CV_32F);

		// Warp the current image
		warper->warp(img, K, cameras[img_id].R, INTER_LINEAR, BORDER_REFLECT, img_warped); // img_warped is the output

		// Warp the current image mask
		mask.create(img_size, CV_8U);
		mask.setTo(Scalar::all(255));
		warper->warp(mask, K, cameras[img_id].R, INTER_NEAREST, BORDER_CONSTANT, mask_warped); // mask_warped is the output

		// Compensate exposure
		compensator->apply(img_id, corners[img_id], img_warped, mask_warped);

		img_warped.convertTo(img_warped_s, CV_16S);
		img_warped.release();
		img.release();
		mask.release();

		dilate(masks_warped[img_id], dilated_mask, Mat());
		resize(dilated_mask, seam_mask, mask_warped.size(), 0, 0, INTER_LINEAR_EXACT);
		mask_warped = seam_mask & mask_warped;

		if (!blender && !this->timelapse) {
			blender = Blender::createDefault(this->blend_type, false);
			Size dst_sz = resultRoi(corners, sizes).size();
			float blend_width = sqrt(static_cast<float>(dst_sz.area())) * this->blend_strength / 100.f;
			if (blend_width < 1.f) {
				blender = Blender::createDefault(Blender::NO, false);
			}
			else if (this->blend_type == Blender::MULTI_BAND) {
				MultiBandBlender* mb = dynamic_cast<MultiBandBlender*>(blender.get());
				mb->setNumBands(static_cast<int>(ceil(log(blend_width) / log(2.)) - 1.));
				LOGLN("Multi-band blender, number of bands: " << mb->numBands());
			}
			else if (this->blend_type == Blender::FEATHER) {
				FeatherBlender* fb = dynamic_cast<FeatherBlender*>(blender.get());
				fb->setSharpness(1.f / blend_width);
				LOGLN("Feather blender, sharpness: " << fb->sharpness());
			}
			blender->prepare(corners, sizes);
		}
		else if (!timelapser && this->timelapse) {
			timelapser = Timelapser::createDefault(this->timelapse_type);
			timelapser->initialize(corners, sizes);
		}

		// Blend the current image
		if (this->timelapse) {
			timelapser->process(img_warped_s, Mat::ones(img_warped_s.size(), CV_8UC1), corners[img_id]);
			String fixedFileName;
			size_t pos_s = String(this->img_names[img_id]).find_last_of("/\\");
			if (pos_s == String::npos) {
				fixedFileName = "fixed_" + this->img_names[img_id];
			}
			else {
				fixedFileName = "fixed_" + String(this->img_names[img_id]).substr(pos_s + 1, String(this->img_names[img_id]).length() - pos_s);
			}
			imwrite(fixedFileName, timelapser->getDst());
		}
		else {
			blender->feed(img_warped_s, mask_warped, corners[img_id]);
		}
	}

	if (!this->timelapse) {
		Mat result, result_mask;
		blender->blend(result, result_mask);

		imwrite("Stitched output", result);
	}*/
}

vector<Point> Warping::returnCorners() {
	return corners_temp;
}

vector<UMat> Warping::returnImagesWarped() {
	return images_warped_temp;
}

/*vector<Point> Warping::returnCorners(vector<Point> corners, vector<Size> img_sizes, vector<CameraParams> cameras, Ptr<RotationWarper> warper) {
	for (int i = 0; i < this->num_images; ++i) {
		// Update intrinsics
		cameras[i].focal *= this->compose_work_aspect;
		cameras[i].ppx *= this->compose_work_aspect;
		cameras[i].ppy *= this->compose_work_aspect;

		// Update corner and size
		Size sz = img_sizes[i];
		if (std::abs(this->compose_scale - 1) > 1e-1) {
			sz.width = cvRound(img_sizes[i].width * this->compose_scale);
			sz.height = cvRound(img_sizes[i].height * this->compose_scale);
		}

		Mat K;
		cameras[i].K().convertTo(K, CV_32F);
		Rect roi = warper->warpRoi(sz, K, cameras[i].R);
		corners[i] = roi.tl();
	}
	return corners;
}

vector<Size> Warping::_update_sizes(vector<Size> sizes, vector<Size> full_img_sizes, vector<CameraParams> cameras, Ptr<RotationWarper> warper) {
	for (int i = 0; i < this->num_images; ++i) {
		// Update intrinsics
		cameras[i].focal *= this->compose_work_aspect;
		cameras[i].ppx *= this->compose_work_aspect;
		cameras[i].ppy *= this->compose_work_aspect;

		// Update corner and size
		Size sz = full_img_sizes[i];
		if (std::abs(this->compose_scale - 1) > 1e-1) {
			sz.width = cvRound(full_img_sizes[i].width * this->compose_scale);
			sz.height = cvRound(full_img_sizes[i].height * this->compose_scale);
		}

		Mat K;
		cameras[i].K().convertTo(K, CV_32F);
		Rect roi = warper->warpRoi(sz, K, cameras[i].R);
		sizes[i] = roi.size();
	}
	return sizes;
}
*/


Warping::~Warping() {
}
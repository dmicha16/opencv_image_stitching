//#include "Composition.h"
//
//Composition::Composition(vector<Mat> inc_images, vector<String> img_names, vector<CameraParams> cameras,
//	vector<Point> corners, float warped_image_scale, int expos_comp_type, vector<UMat> images_warped,
//	vector<UMat> masks_warped) {
//
//	imageComposition(inc_images, img_names, cameras, corners, warped_image_scale, expos_comp_type, images_warped, masks_warped);
//}
//
//void Composition::imageComposition(vector<Mat> images, vector<String> img_names, vector<CameraParams> cameras,
//	vector<Point> corners, float warped_image_scale, int expos_comp_type, vector<UMat> images_warped,
//	vector<UMat> masks_warped) {
//
//	int num_images = static_cast <int>(images.size());
//	Mat img_warped, img_warped_s;
//	Mat dilated_mask, seam_mask, mask, mask_warped;
//	Ptr<Blender> blender;
//	Ptr<Timelapser> timelapser;
//	double compose_seam_aspect = 1, compose_work_aspect = 1, compose_megapix = -1, compose_scale = 1, work_scale = 1;
//	bool is_work_scale_set = false, is_seam_scale_set = false, is_compose_scale_set = false;
//	double seam_work_aspect = 1;
//	bool try_cuda = false;
//	vector<Size> sizes(num_images);
//	Mat full_img, img;
//	vector<Size> full_img_sizes(num_images);
//	bool timelapse = false;
//	int blend_type = Blender::MULTI_BAND;
//	float blend_strength = 5;
//	int timelapse_type = Timelapser::AS_IS;
//	
//	for (int i = 0; i < num_images; i++) {
//		sizes[i] = images_warped[i].size();
//	}
//
//	for (size_t i = 0; i < num_images; i++) {
//		full_img_sizes.push_back(images[i].size());
//	}
//
//	Ptr<WarperCreator> warper_creator;
//	warper_creator = makePtr<cv::AffineWarper>();
//	Ptr<RotationWarper> warper = warper_creator->create(static_cast<float>(warped_image_scale * seam_work_aspect));
//
//	Ptr<ExposureCompensator> compensator = ExposureCompensator::createDefault(expos_comp_type);	
//
//	for (int img_idx = 0; img_idx < num_images; ++img_idx) {
//
//		img = images[img_idx];
//		// Read image and resize it if necessary
//		full_img = imread(img_names[img_idx]);
//		if (!is_compose_scale_set) {
//			if (compose_megapix > 0)
//				compose_scale = min(1.0, sqrt(compose_megapix * 1e6 / full_img.size().area()));
//			is_compose_scale_set = true;
//
//			// Compute relative scales
//			//compose_seam_aspect = compose_scale / seam_scale;
//			compose_work_aspect = compose_scale / work_scale;
//
//			// Update warped image scale
//			warped_image_scale *= static_cast<float>(compose_work_aspect);
//			warper = warper_creator->create(warped_image_scale);
//
//			// Update corners and sizes
//			for (int i = 0; i < num_images; ++i) {
//				// Update intrinsics
//				cameras[i].focal *= compose_work_aspect;
//				cameras[i].ppx *= compose_work_aspect;
//				cameras[i].ppy *= compose_work_aspect;
//
//				// Update corner and size
//				Size sz = full_img_sizes[i];
//				if (std::abs(compose_scale - 1) > 1e-1) {
//					sz.width = cvRound(full_img_sizes[i].width * compose_scale);
//					sz.height = cvRound(full_img_sizes[i].height * compose_scale);
//				}
//
//				Mat K;
//				cameras[i].K().convertTo(K, CV_32F);
//				Rect roi = warper->warpRoi(sz, K, cameras[i].R);
//				corners[i] = roi.tl();
//				sizes[i] = roi.size();
//			}
//		}
//		if (abs(compose_scale - 1) > 1e-1)
//			resize(full_img, img, Size(), compose_scale, compose_scale, INTER_LINEAR_EXACT);
//		else
//			img = full_img;
//		full_img.release();
//		Size img_size = img.size();
//
//		Mat K;
//		cameras[img_idx].K().convertTo(K, CV_32F);
//
//		// Warp the current image
//		warper->warp(img, K, cameras[img_idx].R, INTER_LINEAR, BORDER_REFLECT, img_warped);
//
//		// Warp the current image mask
//		mask.create(img_size, CV_8U);
//		mask.setTo(Scalar::all(255));
//		warper->warp(mask, K, cameras[img_idx].R, INTER_NEAREST, BORDER_CONSTANT, mask_warped);
//
//		// Compensate exposure
//		compensator->apply(img_idx, corners[img_idx], img_warped, mask_warped);
//
//		img_warped.convertTo(img_warped_s, CV_16S);
//		img_warped.release();
//		img.release();
//		mask.release();
//
//		dilate(masks_warped[img_idx], dilated_mask, Mat());
//		resize(dilated_mask, seam_mask, mask_warped.size(), 0, 0, INTER_LINEAR_EXACT);
//		mask_warped = seam_mask & mask_warped;
//
//		if (!blender && !timelapse) {
//			blender = Blender::createDefault(blend_type, try_cuda);
//			Size dst_sz = resultRoi(corners, sizes).size();
//			float blend_width = sqrt(static_cast<float>(dst_sz.area())) * blend_strength / 100.f;
//			if (blend_width < 1.f)
//				blender = Blender::createDefault(Blender::NO, try_cuda);
//			else if (blend_type == Blender::MULTI_BAND) {
//				MultiBandBlender* mb = dynamic_cast<MultiBandBlender*>(blender.get());
//				mb->setNumBands(static_cast<int>(ceil(log(blend_width) / log(2.)) - 1.));
//				LOGLN("Multi-band blender, number of bands: " << mb->numBands());
//			}
//			else if (blend_type == Blender::FEATHER) {
//				FeatherBlender* fb = dynamic_cast<FeatherBlender*>(blender.get());
//				fb->setSharpness(1.f / blend_width);
//				LOGLN("Feather blender, sharpness: " << fb->sharpness());
//			}
//			blender->prepare(corners, sizes);
//		}
//		else if (!timelapser && timelapse) {
//			timelapser = Timelapser::createDefault(timelapse_type);
//			timelapser->initialize(corners, sizes);
//		}
//
//		// Blend the current image
//		if (timelapse) {
//			timelapser->process(img_warped_s, Mat::ones(img_warped_s.size(), CV_8UC1), corners[img_idx]);
//			String fixedFileName;
//			size_t pos_s = String(img_names[img_idx]).find_last_of("/\\");
//			if (pos_s == String::npos) {
//				fixedFileName = "fixed_" + img_names[img_idx];
//			}
//			else {
//				fixedFileName = "fixed_" + String(img_names[img_idx]).substr(pos_s + 1, String(img_names[img_idx]).length() - pos_s);
//			}
//			imwrite(fixedFileName, timelapser->getDst());
//		}
//		else {
//			blender->feed(img_warped_s, mask_warped, corners[img_idx]);
//		}
//	}
//}
//
//Mat Composition::returnFinalComp() {
//	return Mat();
//}
//
//
//Composition::~Composition() {
//}

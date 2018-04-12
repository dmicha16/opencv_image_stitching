#pragma once
#include "Pipeline.h"
class SeamFinding :
	public Pipeline {
public:
	SeamFinding(vector<UMat> images_warped_f, vector<Point> corners, vector<UMat> masks_warped);
	vector<UMat> returnMasksWarped();
	~SeamFinding();

private:
	vector<UMat> masks_warped;
};


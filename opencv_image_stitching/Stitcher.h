#pragma once
#include "Wrapper.h"
#include "Undistorter.h"

class Stitcher :
	public Wrapper {
public:
	Stitcher(Undistorter undistorter);
	~Stitcher();
};


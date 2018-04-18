#include "Stitcher.h"


Stitcher::Stitcher() {
}
Mat Stitcher::merging(Mat &img1, Mat &img2) {
	cout << endl << "Merging() {" << endl << endl;

	cout << "img1.cols = " << img1.cols << endl;
	cout << "img2.cols = " << img2.cols << endl << endl;

	// Get dimension of final image
	int rows = max(img1.rows, img2.rows);
	int cols = img2.cols;

	cout << "rows = " << rows << endl;
	cout << "cols = " << cols << endl << endl;

	// Create a black image
	Mat3b res(rows, cols, Vec3b(0, 0, 0));
	cout << "res.size() = " << res.size() << endl;
	cout << "}" << endl << endl;

	// Copy images in correct position
	img2.copyTo(res(Rect(0, 0, img2.cols, img2.rows)));
	img1.copyTo(res(Rect(0, 0, img1.cols, img1.rows)));

	stitchedImage = res;
	return stitchedImage;
}

Stitcher::~Stitcher() {
}

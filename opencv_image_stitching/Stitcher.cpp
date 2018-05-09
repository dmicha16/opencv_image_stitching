#include "Stitcher.h"

Stitcher::Stitcher() {
}

Mat Stitcher::merging(Mat &img1, Mat &img2) {
	cout << endl << "Merging() {" << endl << endl;

	cout << "img1.cols = " << img1.cols << endl;
	cout << "img2.cols = " << img2.cols << endl << endl;

	// Get dimension of final image
	int rows = max(img1.rows, img2.rows);
	int cols = max(img1.cols, img2.cols);

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

Mat Stitcher::customMerger(Mat &img1, Mat &img2) {
	cout << "customMerging() {" << endl;

	cout << "img1.cols = " << img1.cols << endl;
	cout << "img2.cols = " << img2.cols << endl;
	cout << "img1.rows = " << img1.rows << endl;
	cout << "img2.rows = " << img2.rows << endl;

	// Get dimension of final image
	int rows = max(img1.rows, img2.rows);
	int cols = max(img1.cols, img2.cols);

	//cout << "rows = " << rows << endl;
	//cout << "cols = " << cols << endl;

	// Create a black image
	Mat3b res(rows, cols, Vec3b(0, 0, 0));
	//cout << "res.size() = " << res.size() << endl;
	cout << "}" << endl;


	// Copy images in correct position
	for (int y = 0; y < img1.rows; y++) // loop through the image
	{
		for (int x = 0; x < img1.cols; x++)
		{
			if (img1.at<Vec3b>(y, x) != BLACKPIXEL) {
				img2.at<Vec3b>(y, x) = img1.at<Vec3b>(y, x); // set value
			}
		}
	}

	return img2;
}

Stitcher::~Stitcher() {
}

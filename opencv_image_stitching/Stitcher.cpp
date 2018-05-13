#include "Stitcher.h"

Stitcher::Stitcher() {
}

Mat Stitcher::customMerger(Mat &img1, Mat &img2) {
	cout << "customMerging() {" << endl;

	cout << "img1.cols = " << img1.cols << endl;
	cout << "img1.rows = " << img1.rows << endl;
	cout << "img2.cols = " << img2.cols << endl;
	cout << "img2.rows = " << img2.rows << endl;
	try {
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
	}
	catch (const std::exception& e) {
		cout << e.what() << endl;
	}

	cout << "}" << endl;
	return img2;
}

Mat Stitcher::reduceImage(Mat &img, int offSetX) {
	cout << "reduceImage() {" << endl;
	cout << "offSetX = " << offSetX << endl;

	// Get dimension of final image
	int rows = img.rows;
	int cols = img.cols - offSetX - 1;

	// Create a black image
	Mat3b res(rows, cols, Vec3b(0, 0, 0));
	cout << "res.size() = " << res.size << endl;

	cout << "reducing the image size...." << endl;

	// Copy the image onto the smaller canvas
	for (int y = 0; y < img.rows; y++) // loop through the image
	{
		for (int x = 0; x < cols; x++)
		{
			res.at<Vec3b>(y, x) = img.at<Vec3b>(y, x); 
		}
	}
	cout << "}" << endl;
	return res;
}


Stitcher::~Stitcher() {
}

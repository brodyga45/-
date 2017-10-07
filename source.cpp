#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>

#include "helpers.h"

using namespace cv;
using namespace std;

Mat get_image(String address) {
	Mat image;
	image = imread(address, IMREAD_COLOR);
	return image;
}

void erode_image(Mat& image, int erosion_size) {
	Mat element = getStructuringElement(MORPH_RECT,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	erode(image, image, element);
}

int main(int argc, char** argv)
{
	String path = "dataset/0001.jpg";
	if (argc == 2)
	{
		path = argv[1];
	}
	Mat image, original;
	image = get_image(path); // Read the file
	original = get_image(path);
	if (image.empty()) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	cvtColor(image, image, CV_BGR2HSV);
	for (int x = 0; x < image.cols; x++) {
		for (int y = 0; y < image.rows; y++) {
			vector<char> pixel = get(image, x, y);
			int hue = (unsigned char)pixel[0];
			int sat = (unsigned char)pixel[1];
			int val = (unsigned char)pixel[2];
			if ((hue < 10 || hue > 245) && (sat > 50) && (val > 50)) {
				set(image, x, y, COLOR_WHITE);
			}
			else {
				set(image, x, y, COLOR_BLACK);
			}
		}
	}
	cvtColor(image, image, CV_BGR2GRAY);
	erode_image(image, 1);

	namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Display window", image); // Show our image inside it.
	namedWindow("Display window 2", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Display window 2", original); // Show our image inside it.

	//imwrite("result.jpg", image);

	waitKey(0); // Wait for a keystroke in the window

	return 0;
}
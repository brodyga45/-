#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>

#include "helpers.h"
#include "kassikhin.h"

#define min(a, b) (a<b)?a:b

using namespace cv;
using namespace std;

Mat get_image(String address) {
	Mat image;
	image = imread(address, IMREAD_COLOR);
	return image;
}

void onecolor(Mat &image, int color) {
	for (int x = 0; x < image.cols; x++) {
		for (int y = 0; y < image.rows; y++) {
			auto pixel = get(image, x, y);
			for (int i = 0; i < pixel.size(); i++) {
				if (i != color) {
					pixel[i] = 100;
				}
			}
			set(image, x, y, pixel);
		}
	}
}

void count_color(Mat &image, int color) {
	vector<int> pixs(256);
	for (int x = 0; x < image.cols; x++) {
		for (int y = 0; y < image.rows; y++) {
			auto pixel = get(image, x, y);
			pixs[(unsigned char)pixel[color]]++;
		}
	}
	for (int i = 0; i < 256; i++) {
		cout << i << ' ' << pixs[i] << endl;
	}
}

int main(int argc, char** argv)
{
	String path = "dataset/0005.jpg";
	if (argc == 2)
	{
		path = argv[1];
	}
	Mat image, original;
	image = get_image(path); // Read the file
	original = image.clone();
	if (image.empty()) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	//medianBlur(image, image, 5);
	select_color(image, 170, 190, 160, 255, 70, 255);
	//erode_image(image, 1);
	//dilate_image(image, 1);

	Mat image_2 = original.clone();
	//medianBlur(image_2, image_2, 5);
	//select_color(image_2, 10, 250, 50, 255, 50, 255);
	cut_level(image_2, 2, 0.45);
	//erode_image(image_2, 1);
	//dilate_image(image_2, 1);

	//image_invert(image_2);
	image = min_filter(image, image_2);

	//image = apply_mask(original, image);

	dilate_image(image, 1);
	image = big_white(image);

	erode_image(image, 5);
	dilate_image(image, 5);

	image = apply_mask(original, image);

	//cvtColor(original, original, CV_BGR2HSV);
	//count_color(original, 1);
	//imwrite("result.jpg", original);

	namedWindow("Display window", WINDOW_AUTOSIZE); 
	imshow("Display window", image);
	namedWindow("Display window 2", WINDOW_AUTOSIZE);
	imshow("Display window 2", original);

	//imwrite("result.jpg", image);

	waitKey(0); // Wait for a keystroke in the window

	return 0;
}
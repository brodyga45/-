#pragma comment(linker, "/STACK:100000000")

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>

#include "helpers.h"
#include "kassikhin.h"
#include "gg.h"

#define min(a, b) (a<b)?a:b

using namespace cv;
using namespace std;

const int THRESHOLD = 128;

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

Mat convert_to_grayscale(Mat &image, int color) {
	Mat ans;
	cvtColor(image, ans, CV_BGR2GRAY);

	for (int x = 0; x < image.cols; x++) {
		for (int y = 0; y < image.rows; y++) {
			auto pixel = get(image, x, y);
			set(ans, x, y, { pixel[color] });
		}
	}

	return ans;
}

void normalize(Mat &image) {
	vector<Mat> images(image.step.buf[1]);
	for (int color = 0; color < image.step.buf[1]; color++) {
		images[color] = convert_to_grayscale(image, color);
		equalizeHist(images[color], images[color]);
	}
	for (int x = 0; x < image.cols; x++) {
		for (int y = 0; y < image.rows; y++) {
			vector<char> pixel(image.step.buf[1]);
			for (int color = 0; color < image.step.buf[1]; color++) {
				pixel[color] = get(images[color], x, y)[0];
			}
			set(image, x, y, pixel);
		}
	}
}

void cut_color_level(cv::Mat &image, int color, int threshold) {
	for (int x = 0; x < image.cols; x++) {
		for (int y = 0; y < image.rows; y++) {
			std::vector<char> pixel = get(image, x, y);
			if ((unsigned int)pixel[color] >= threshold) {
				set(image, x, y, COLOR_WHITE);
			}
			else {
				set(image, x, y, COLOR_BLACK);
			}
		}
	}
	cvtColor(image, image, CV_BGR2GRAY);
}

vector<Mat> get_interesting_spots(Mat &mirror, int color) {
	if (color == 3)
	{
		Mat original = mirror.clone();
		auto threshold = count_threshold(mirror, 2 - color);
		cout << threshold[2] - threshold[1] << endl;

		Mat image;
		image = original.clone();
		select_gray(image);

		Mat image_2;

		dilate_image(image, 2);
		image = big_white(image);

		erode_image(image, 6);
		dilate_image(image, 5);
		/*
		namedWindow("Display window 2", WINDOW_AUTOSIZE);
		imshow("Display window 2", original);
		waitKey(0);

		namedWindow("Display window", WINDOW_AUTOSIZE);
		imshow("Display window", image);
		waitKey(0);
		*/
		auto images = chop_image(image, original);

		for (int i = 0; i < images.size(); i++) {
			resize(images[i], images[i], Size(100, 100), 0, 0, 1);
		}

		return images;

	}

	int hue = 0;
	if (color == 0) hue = 180;
	if (color == 1) hue = 60;
	if (color == 2) hue = 120;

	Mat original = mirror.clone();
	auto threshold = count_threshold(mirror, 2 - color);
	cout << threshold[2] - threshold[1] << endl;

	Mat image;
	image = original.clone();
	select_color(image, hue - 10, hue + 10, 130, 255, 50, 255);

	Mat image_2;

	image_2 = original.clone();
	cut_level(image_2, 2 - color, 0.4);
	image = min_filter(image, image_2);

	//image_2 = original.clone();
	//select_color(image_2, 0, 180, 0, 20, 150, 255);
	//image = max_filter(image, image_2);

	dilate_image(image, 2);
	image = big_white(image);

	image_2 = original.clone();
	cut_color_level(image_2, 2 - color, threshold[0]);
	image = min_filter(image, image_2);

	dilate_image(image, 2);
	image = big_white(image);

	erode_image(image, 6);
	dilate_image(image, 5);

	//image_2 = original.clone();
	//select_color(image_2, 0, 180, 0, 20, 150, 255);
	//image_invert(image_2);
	//image = min_filter(image, image_2);
	//erode_image(image, 1);
	//dilate_image(image, 3);
	//image = big_white(image);
	//erode_image(image, 2);

	//erode_image(image, 5);
	//dilate_image(image, 5);

	auto images = chop_image(image, original);

	for (int i = 0; i < images.size(); i++) {
		resize(images[i], images[i], Size(100, 100), 0, 0, 1);
	}

	return images;
}

int pixels_num(Mat &image) {
	int sum = 0;
	for (int x = 0; x < image.cols; x++) {
		for (int y = 0; y < image.rows; y++) {
			if (get(image, x, y)[0] != 0) sum++;
		}
	}
	return sum;
}

string NAME(int x, int y, int k) {
	string sx = "0", sy = "0", sk = "0";
	while (x != 0) {
		sx += char(x % 10 + '0');
		x = x / 10;
	}
	while (y != 0) {
		sy += char(y % 10 + '0');
		y = y / 10;
	}
	while (k != 0) {
		sk += char(k % 10 + '0');
		k = k / 10;
	}
	return sx + sy + sk;
}

int main(int argc, char** argv)
{
	String path = "dataset/0004.jpg";
	if (argc == 2)
	{
		path = argv[1];
	}
	Mat image, original;
	original = get_image(path); // Read the file
	if (original.empty()) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	Mat mirror;
	Mat mirror_copy = mirror.clone();
	vector<Mat> images;
	cvtColor(original, mirror, CV_BGR2HSV);
	for (int x = 0; x < original.cols; x++) {
		for (int y = 0; y < original.rows; y++) {
			int sum = 0;
			image = geo_line(mirror, x, y, sum);
			if (sum < 1000) continue;
			dilate_image(image, 2);
			image = big_white(image);
			//if (sum > 30000) continue;
			images.push_back(apply_mask(original, image));
		}
	}

	for (int i = 0; i < images.size(); ++i)
	{
		imwrite("images/image_" + NAME(i, 0, 0) + ".jpg", images[i]);
		vector<Mat> int_spots = get_interesting_spots(images[i], 0);
		
		for (int j = 0; j < int_spots.size(); ++j)
		{
			imwrite("images/image_" + NAME(i, j + 1, 0) + ".jpg", int_spots[j]);
		}
		
		int_spots = get_interesting_spots(images[i], 2);
		for (int j = 0; j < int_spots.size(); ++j)
		{
			imwrite("images/image_" + NAME(i, j + 1, 2) + ".jpg", int_spots[j]);
		}
		/*
		int_spots = get_interesting_spots(images[i], 3);
		for (int j = 0; j < int_spots.size(); ++j)
		{
			imwrite("images/image_" + NAME(i, j + 1, 2) + ".jpg", int_spots[j]);
		}
		*/
	}

	//cvtColor(original, original, CV_HSV2BGR);
	//dilate_image(image, 1);
	//image = big_white(image);

	//image = apply_mask(original, image);

	//namedWindow("Display window", WINDOW_AUTOSIZE);
	//imshow("Display window", image);

	//waitKey(0);

	//vector<Mat> images;

	/*for (int i = 0; i < 3; i++) {
	vector<Mat> color_images = get_interesting_spots(original, i);
	for (int j = 0; j < color_images.size(); j++) {
	images.push_back(color_images[j]);
	}
	}*/

	namedWindow("Display window", WINDOW_AUTOSIZE);
	imshow("Display window", original);


	//imwrite("result.jpg", image);

	waitKey(0); // Wait for a keystroke in the window

	return 0;
}
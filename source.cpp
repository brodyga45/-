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
	if (color == -1)
	{
		Mat original = mirror.clone();
		auto threshold = count_threshold(mirror, 2 - color);
		cout << threshold[2] - threshold[1] << endl;

		Mat image;
		image = original.clone();
		select_gray(image);
		/*
		namedWindow("Display window 2", WINDOW_AUTOSIZE);
		imshow("Display window 2", original);

		namedWindow("Display window", WINDOW_AUTOSIZE);
		imshow("Display window", image);
		waitKey(0);
		*/
		dilate_image(image, 2);
		

		image = big_white(image);


		erode_image(image, 6);
		dilate_image(image, 5);

		auto images = chop_image(image, original);

		for (int i = 0; i < images.size(); i++) {
			resize(images[i], images[i], Size(100, 100), 0, 0, 1);
		}

		return images;

	}

	if (color == 3)
	{
		color = 1;
		int color2 = 2;
		int hue = 30;
		Mat original = mirror.clone();
		auto threshold = count_threshold(mirror, 2 - color);
		cout << threshold[2] - threshold[1] << endl;

		Mat image;
		image = original.clone();
		select_color(image, hue - 10, hue + 10, 130, 255, 50, 255);

		Mat image_2;

		image_2 = original.clone();
		cut_2level(image_2, 2 - color, 2 - color, 0.4);
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

vector<int> get_pixel_colors(Mat &image) {
	vector<int> colors(8); //RGBCMYWK
	for (int x = 0; x < image.cols; x++) {
		for (int y = 0; y < image.rows; y++) {
			auto pixel = get(image, x, y);
			if (pixel == COLOR_BLACK || pixel == COLOR_GRAY) continue;
			int r = (unsigned char)pixel[2];
			int g = (unsigned char)pixel[1];
			int b = (unsigned char)pixel[0];
			int sum = r + g + b;
			if (r > sum * 0.6) colors[0]++;
			else if (g > sum * 0.6) colors[1]++;
			else if (b > sum * 0.6) colors[2]++;
			else if (g + b > sum * 0.8) colors[3]++;
			else if (r + b > sum * 0.8) colors[4]++;
			else if (r + g > sum * 0.8) colors[5]++;
			else if (sum > 256 * 3 * 0.5) colors[6]++;
			else colors[7]++;
		}
	}
	return colors;
}

int main(int argc, char** argv)
{
	String path = "dataset/0006.jpg";
	if (argc == 2)
	{
		path = argv[1];
	}
	Mat image, original;
	original = get_image(path); // Read the file

	/*Mat sign = get_image("Pdd/pdd2/2.2.jpg");
	auto colors = get_pixel_colors(sign);
	int sum = 0;
	for (int i = 0; i < 7; i++) sum += colors[i];
	cout << "red: " << colors[0] / (double)sum * 100 << " %" << endl;
	cout << "green: " << colors[1] / (double)sum * 100 << " %" << endl;
	cout << "blue: " << colors[2] / (double)sum * 100 << " %" << endl;
	cout << "cyan: " << colors[3] / (double)sum * 100 << " %" << endl;
	cout << "magenta: " << colors[4] / (double)sum * 100 << " %" << endl;
	cout << "yellow: " << colors[5] / (double)sum * 100 << " %" << endl;
	cout << "gray: " << colors[6] / (double)sum * 100 << " %" << endl;
	imwrite("sign.jpg", sign);*/

	vector<pair<string, string>> sign_database;
	sign_database.push_back(make_pair("1.1", "Pdd/pdd1/1.1.jpg"));
	sign_database.push_back(make_pair("1.2", "Pdd/pdd1/1.2.jpg"));
	sign_database.push_back(make_pair("1.3.1", "Pdd/pdd1/1.3.1.jpg"));
	sign_database.push_back(make_pair("1.3.2", "Pdd/pdd1/1.3.2.jpg"));
	sign_database.push_back(make_pair("1.4.1", "Pdd/pdd1/1.4.1.jpg"));
	sign_database.push_back(make_pair("1.4.2", "Pdd/pdd1/1.4.2.jpg"));
	sign_database.push_back(make_pair("1.4.3", "Pdd/pdd1/1.4.3.jpg"));
	sign_database.push_back(make_pair("1.4.4", "Pdd/pdd1/1.4.4.jpg"));
	sign_database.push_back(make_pair("1.4.5", "Pdd/pdd1/1.4.5.jpg"));
	sign_database.push_back(make_pair("1.4.6", "Pdd/pdd1/1.4.6.jpg"));
	sign_database.push_back(make_pair("1.5", "Pdd/pdd1/1.5.jpg"));
	sign_database.push_back(make_pair("1.6", "Pdd/pdd1/1.6.jpg"));
	sign_database.push_back(make_pair("1.7", "Pdd/pdd1/1.7.jpg"));
	sign_database.push_back(make_pair("1.8", "Pdd/pdd1/1.8.jpg"));
	sign_database.push_back(make_pair("1.9", "Pdd/pdd1/1.9.jpg"));
	sign_database.push_back(make_pair("1.10", "Pdd/pdd1/1.10.jpg"));
	sign_database.push_back(make_pair("1.11.1", "Pdd/pdd1/1.11.1.jpg"));
	sign_database.push_back(make_pair("1.11.2", "Pdd/pdd1/1.11.2.jpg"));
	sign_database.push_back(make_pair("1.12.1", "Pdd/pdd1/1.12.1.jpg"));
	sign_database.push_back(make_pair("1.12.2", "Pdd/pdd1/1.12.2.jpg"));
	sign_database.push_back(make_pair("1.13", "Pdd/pdd1/1.13.jpg"));
	sign_database.push_back(make_pair("1.14", "Pdd/pdd1/1.14.jpg"));
	sign_database.push_back(make_pair("1.15", "Pdd/pdd1/1.15.jpg"));
	sign_database.push_back(make_pair("1.16", "Pdd/pdd1/1.16.jpg"));
	sign_database.push_back(make_pair("1.17", "Pdd/pdd1/1.17.jpg"));
	sign_database.push_back(make_pair("1.18", "Pdd/pdd1/1.18.jpg"));
	sign_database.push_back(make_pair("1.19", "Pdd/pdd1/1.19.jpg"));
	sign_database.push_back(make_pair("1.20.1", "Pdd/pdd1/1.20.1.jpg"));
	sign_database.push_back(make_pair("1.20.2", "Pdd/pdd1/1.20.2.jpg"));
	sign_database.push_back(make_pair("1.20.3", "Pdd/pdd1/1.20.3.jpg"));
	sign_database.push_back(make_pair("1.21", "Pdd/pdd1/1.21.jpg"));
	sign_database.push_back(make_pair("1.22", "Pdd/pdd1/1.22.jpg"));
	sign_database.push_back(make_pair("1.23", "Pdd/pdd1/1.23.jpg"));
	sign_database.push_back(make_pair("1.24", "Pdd/pdd1/1.24.jpg"));
	sign_database.push_back(make_pair("1.25", "Pdd/pdd1/1.25.jpg"));
	sign_database.push_back(make_pair("1.26", "Pdd/pdd1/1.26.jpg"));
	sign_database.push_back(make_pair("1.27", "Pdd/pdd1/1.27.jpg"));
	sign_database.push_back(make_pair("1.28", "Pdd/pdd1/1.28.jpg"));
	sign_database.push_back(make_pair("1.29", "Pdd/pdd1/1.29.jpg"));
	sign_database.push_back(make_pair("1.30", "Pdd/pdd1/1.30.jpg"));
	sign_database.push_back(make_pair("1.31", "Pdd/pdd1/1.31.jpg"));
	sign_database.push_back(make_pair("1.32", "Pdd/pdd1/1.32.jpg"));
	sign_database.push_back(make_pair("1.33", "Pdd/pdd1/1.33.jpg"));
	sign_database.push_back(make_pair("1.34.1 B", "Pdd/pdd1/1.34.1_B.jpg"));
	sign_database.push_back(make_pair("1.34.2 B", "Pdd/pdd1/1.34.2_B.jpg"));
	sign_database.push_back(make_pair("1.34.3 B", "Pdd/pdd1/1.34.3_B.jpg"));

	sign_database.push_back(make_pair("2.1", "Pdd/pdd2/2.1.jpg"));
	sign_database.push_back(make_pair("2.2", "Pdd/pdd2/2.2.jpg"));
	sign_database.push_back(make_pair("2.3.1", "Pdd/pdd2/2.3.1.jpg"));
	sign_database.push_back(make_pair("2.3.2", "Pdd/pdd2/2.3.2.jpg"));
	sign_database.push_back(make_pair("2.3.3", "Pdd/pdd2/2.3.3.jpg"));
	sign_database.push_back(make_pair("2.3.4", "Pdd/pdd2/2.3.4.jpg"));
	sign_database.push_back(make_pair("2.3.5", "Pdd/pdd2/2.3.5.jpg"));
	sign_database.push_back(make_pair("2.3.6", "Pdd/pdd2/2.3.6.jpg"));
	sign_database.push_back(make_pair("2.3.7", "Pdd/pdd2/2.3.7.jpg"));
	sign_database.push_back(make_pair("2.4", "Pdd/pdd2/2.4.jpg"));
	sign_database.push_back(make_pair("2.5", "Pdd/pdd2/2.5.jpg"));
	sign_database.push_back(make_pair("2.6", "Pdd/pdd2/2.6.jpg"));
	sign_database.push_back(make_pair("2.7", "Pdd/pdd2/2.7.jpg"));

	for (auto sign : sign_database) {
		cout << sign.first << ": ";
		Mat matrix = get_image(sign.second);
		polarize(matrix, true);
		int sum = 0;
		auto sss = get_pixel_colors(matrix);
		//namedWindow("Display window", WINDOW_AUTOSIZE);
		//imshow("Display window", matrix);
		//waitKey(0);
		for (int i = 0; i < 8; i++) sum += sss[i];
		if (sss[0] > sum * 0.01) cout << "red ";
		if (sss[1] > sum * 0.01) cout << "green ";
		if (sss[2] > sum * 0.01) cout << "blue ";
		if (sss[3] > sum * 0.01) cout << "cyan ";
		if (sss[4] > sum * 0.01) cout << "magenta ";
		if (sss[5] > sum * 0.01) cout << "yellow ";
		if (sss[6] > sum * 0.01) cout << "white ";
		if (sss[7] > sum * 0.01) cout << "black ";
		cout << endl;
	}

	//app_means(original);
	//remove_white_spots(original);

	//namedWindow("Display window", WINDOW_AUTOSIZE);
	//imshow("Display window", original);
	//waitKey(0);

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
	vector<Mat> universal;
	for (int i = 0; i < images.size(); ++i)
	{
		//imwrite("images/image_" + NAME(i, 0, 0) + ".jpg", images[i]);
		vector<Mat> int_spots = get_interesting_spots(images[i], 0);
		for (int j = 0; j < int_spots.size(); ++j)
		{
			polarize(int_spots[j]);
			universal.push_back(int_spots[j].clone());
			//app_means(original);
			//imwrite("images/image_" + NAME(i, j + 1, 0) + ".jpg", universal[universal.size() - 1]);
		}
		
		int_spots = get_interesting_spots(images[i], 2);
		for (int j = 0; j < int_spots.size(); ++j)
		{
			polarize(int_spots[j]);
			universal.push_back(int_spots[j].clone());
			//app_means(original);
			//imwrite("images/image_" + NAME(i, j + 1, 2) + ".jpg", int_spots[j]);
		}
		
		int_spots = get_interesting_spots(images[i], -1);
		for (int j = 0; j < int_spots.size(); ++j)
		{
			cvtColor(int_spots[j], int_spots[j], CV_BGR2GRAY);
			polarize(int_spots[j]);
			universal.push_back(int_spots[j].clone());
			//app_means(original);
			//imwrite("images/image_" + NAME(i, j + 1, 2) + ".jpg", int_spots[j]);
		}
		
		int_spots = get_interesting_spots(images[i], 3);
		for (int j = 0; j < int_spots.size(); ++j)
		{
			polarize(int_spots[j]);
			universal.push_back(int_spots[j].clone());
			//app_means(original);
			//imwrite("images/image_" + NAME(i, j + 1, 4) + ".jpg", int_spots[j]);
		}
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

	vector<pair<Mat, string>> good;

	for (int j = 1; j < universal.size(); j++)
	{
		Mat img = universal[j];
		auto uc = get_pixel_colors(img);
		int ucsum = 0;
		for (int i = 0; i < 8; i++) ucsum += uc[i];

		int max_diff = pixel_cmp(img, img);
		int cur_diff = 0;
		string possible_sign;
		for (int i = 0; i < sign_database.size(); ++i)
		{
			Mat sign = get_image(sign_database[i].second);
			resize(sign, sign, cv::Size(100, 100), 0, 0, 1);
			polarize(sign, true);

			auto sc = get_pixel_colors(sign);
			int scsum = 0;
			for (int i = 0; i < 8; i++) scsum += sc[i];

			bool bad = false;

			for (int i = 0; i < 8; i++) {
				if (uc[i] < ucsum * 0.01 && sc[i] > scsum * 0.01) bad = true;
				if (uc[i] > ucsum * 0.02 && sc[i] < scsum * 0.01) bad = true;
			}

			if (bad) continue;

			int diff = pixel_cmp(img, sign);
			if (diff > cur_diff) {
				cur_diff = diff;
				possible_sign = sign_database[i].first;
			}
		}

		if (cur_diff > max_diff * 0.5) {
			good.push_back(make_pair(universal[j], possible_sign));
			cout << cur_diff / (double)max_diff * 100 << endl;
			imwrite("images/good_" + NAME(j, 0, 0) + "_" + possible_sign + ".jpg", img);
		}
	}

	namedWindow("Display window", WINDOW_AUTOSIZE);
	imshow("Display window", original);

	//imwrite("result.jpg", image);

	waitKey(0); // Wait for a keystroke in the window

	return 0;
}
#include "helpers.h"

std::vector<std::vector<std::vector<char>>> mat_to_vec(cv::Mat &image) {
	int image_rows = image.rows;
	int image_cols = image.cols;
	std::vector<std::vector<std::vector<char>>> vector_image(image_rows, std::vector<std::vector<char>>(image_cols, std::vector<char>(image.step.buf[1])));
	char* ptr = (char*)image.datastart;
	for (int i = 0; i < image_rows; i++) {
		for (int j = 0; j < image_cols; j++) {
			for (int k = 0; k < image.step.buf[1]; k++) {
				vector_image[i][j][k] = *ptr;
				ptr++;
			}
		}
	}
	return vector_image;
}

std::vector<char> get(cv::Mat &image, int x, int y) {
	std::vector<char> pixel;
	char* ptr = (char*)((long long)(image.datastart) + y * image.step.buf[0] + x * image.step.buf[1]);
	for (int k = 0; k < image.step.buf[1]; k++) {
		pixel.push_back(*ptr);
		ptr++;
	}
	return pixel;
}

void set(cv::Mat &image, int x, int y, std::vector<char> new_value) {
	if (new_value.size() < image.step.buf[1])
		return;

	char* ptr = (char*)((long long)(image.datastart) + y * image.step.buf[0] + x * image.step.buf[1]);
	for (int k = 0; k < image.step.buf[1]; k++) {
		*ptr = new_value[k];
		ptr++;
	}
}

void image_invert(cv::Mat &image) {
	for (int x = 0; x < image.cols; x++) {
		for (int y = 0; y < image.rows; y++) {
			auto pixel = get(image, x, y);
			for (int i = 0; i < pixel.size(); i++) {
				pixel[i] ^= -1;
			}
			set(image, x, y, pixel);
		}
	}
}

void erode_image(cv::Mat& image, int erosion_size) {
	cv::Mat element = getStructuringElement(cv::MORPH_RECT,
		cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		cv::Point(erosion_size, erosion_size));

	erode(image, image, element);
}

void dilate_image(cv::Mat& image, int dilation_size) {
	cv::Mat element = getStructuringElement(cv::MORPH_RECT,
		cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		cv::Point(dilation_size, dilation_size));

	dilate(image, image, element);
}

void select_color(cv::Mat &image, int hue_min, int hue_max, int sat_min, int sat_max, int val_min, int val_max) {
	cvtColor(image, image, CV_BGR2HSV);
	for (int x = 0; x < image.cols; x++) {
		for (int y = 0; y < image.rows; y++) {
			std::vector<char> pixel = get(image, x, y);
			int hue = (int)((unsigned char)pixel[0]);
			int sat = (int)((unsigned char)pixel[1]);
			int val = (int)((unsigned char)pixel[2]);
			if (((hue_min <= hue && hue <= hue_max) || (hue_min <= hue + 180 && hue + 180 <= hue_max))
				&& ((sat_min <= sat && sat <= sat_max) || (sat_min <= sat + 256 && sat + 256 <= sat_max))
				&& ((val_min <= val && val <= val_max) || (val_min <= val + 256 && val + 256 <= val_max))) {
				set(image, x, y, COLOR_WHITE);
			}
			else {
				set(image, x, y, COLOR_BLACK);
			}
		}
	}
	cvtColor(image, image, CV_BGR2GRAY);
}

int f_gray(cv::Mat &image, int x, int y)
{
	std::vector<char> pixel = get(image, x, y);
	return abs((int)(unsigned char)pixel[0] - (int)(unsigned char)pixel[1]) + abs((int)(unsigned char)pixel[1] - (int)(unsigned char)pixel[2]) + abs((int)(unsigned char)pixel[2] - (int)(unsigned char)pixel[0]);
}

bool f_sat(cv::Mat &image, int x, int y)
{
	std::vector<char> pixel = get(image, x, y);
	return (
			((int)(unsigned char)pixel[0] > 100) &&
			((int)(unsigned char)pixel[1] > 100) &&
			((int)(unsigned char)pixel[2] > 100)
		   );
}

void select_gray(cv::Mat &image) {
	for (int x = 0; x < image.cols; x++) {
		for (int y = 0; y < image.rows; y++) {
			if (f_gray(image, x, y) < 105 && f_sat(image, x, y)) {
				set(image, x, y, COLOR_WHITE);
			}
			else {
				set(image, x, y, COLOR_BLACK);
			}
		}
	}
	cvtColor(image, image, CV_BGR2GRAY);
}

void cut_level(cv::Mat &image, int color, double threshold) {
	for (int x = 0; x < image.cols; x++) {
		for (int y = 0; y < image.rows; y++) {
			std::vector<char> pixel = get(image, x, y);
			if ((unsigned int)pixel[color] >= ((unsigned int)pixel[0] + (unsigned int)pixel[1] + (unsigned int)pixel[2]) * threshold) {
				set(image, x, y, COLOR_WHITE);
			}
			else {
				set(image, x, y, COLOR_BLACK);
			}
		}
	}
	cvtColor(image, image, CV_BGR2GRAY);
}

void cut_2level(cv::Mat &image, int color, int color2, double threshold) {
	for (int x = 0; x < image.cols; x++) {
		for (int y = 0; y < image.rows; y++) {
			std::vector<char> pixel = get(image, x, y);
			if ((unsigned int)pixel[color] + (unsigned int)pixel[color2] >= ((unsigned int)pixel[0] + (unsigned int)pixel[1] + (unsigned int)pixel[2]) * threshold) {
				set(image, x, y, COLOR_WHITE);
			}
			else {
				set(image, x, y, COLOR_BLACK);
			}
		}
	}
	cvtColor(image, image, CV_BGR2GRAY);
}

cv::Mat min_filter(cv::Mat &image_1, cv::Mat &image_2) {
	cv::Mat image = image_1.clone();
	for (int x = 0; x < image.cols; x++) {
		for (int y = 0; y < image.rows; y++) {
			auto pixel_1 = get(image_1, x, y);
			auto pixel_2 = get(image_2, x, y);
			for (int i = 0; i < pixel_1.size(); i++) {
				pixel_1[i] = std::min((unsigned char)pixel_1[i], (unsigned char)pixel_2[i]);
			}
			set(image, x, y, pixel_1);
		}
	}
	return image;
}

cv::Mat max_filter(cv::Mat &image_1, cv::Mat &image_2) {
	cv::Mat image = image_1.clone();
	for (int x = 0; x < image.cols; x++) {
		for (int y = 0; y < image.rows; y++) {
			auto pixel_1 = get(image_1, x, y);
			auto pixel_2 = get(image_2, x, y);
			for (int i = 0; i < pixel_1.size(); i++) {
				pixel_1[i] = std::max((unsigned char)pixel_1[i], (unsigned char)pixel_2[i]);
			}
			set(image, x, y, pixel_1);
		}
	}
	return image;
}

bool is_white(cv::Mat &image, int x, int y)
{
	std::vector<char> pixel = get(image, x, y);
	return (
		(int)(unsigned char)pixel[0] > 245 &&
		(int)(unsigned char)pixel[0] > 245 &&
		(int)(unsigned char)pixel[0] > 245
		);
}

void remove_white_spots(cv::Mat &image)
{
	cv::Mat copy_ = image.clone();
	for (int x = 0; x < image.cols; x++) {
		for (int y = 0; y < image.rows; y++) {
			if (f_gray(image, x, y) < 105 && f_sat(image, x, y)) {
				set(copy_, x, y, COLOR_WHITE);
			}
			else {
				set(copy_, x, y, COLOR_BLACK);
			}
		}
	}

	erode_image(copy_, 7);
	dilate_image(copy_, 8);
	image_invert(copy_);
	image = min_filter(copy_, image);
}

void polarize(cv::Mat &image, bool precise)
{
	std::vector<std::vector<int>> t(3);
	if (!precise) {
		t[0] = count_threshold(image, 0);
		t[1] = count_threshold(image, 1);
		t[2] = count_threshold(image, 2);
	}
	else {
		t[0] = { 128 };
		t[1] = { 128 };
		t[2] = { 128 };
	}
	for (int i = 0; i < image.cols; ++i)
		for (int j = 0; j < image.rows; ++j)
		{
			std::vector<char> pixel = get(image, i, j);
			if (pixel == COLOR_GRAY) {
				set(image, i, j, COLOR_BLACK);
				continue;
			}
			std::vector<int> bgr;
			std::vector<char> nbgr;
			for (int k = 0; k < pixel.size(); ++k)
			{
				bgr.push_back((int)(unsigned char)pixel[k]);
				if (bgr[k] < t[k][0] * 9 / 10)
					nbgr.push_back(0);
				else if (bgr[k] > t[k][0] * 11 / 10)
					nbgr.push_back(255);
				else
					nbgr.push_back(bgr[k]);
			}
			set(image, i, j, nbgr);
		}
}

cv::Mat apply_mask(cv::Mat &image, cv::Mat &mask) {
	cv::Mat applied_image = image.clone();
	for (int x = 0; x < image.cols; x++) {
		for (int y = 0; y < image.rows; y++) {
			auto pixel = get(mask, x, y);
			auto image_pixel = get(applied_image, x, y);
			for (int i = 0; i < image_pixel.size(); i++) {
				image_pixel[i] = ((int)((unsigned char)image_pixel[i]) * (int)((unsigned char)pixel[0]) / 255);
			}
			set(applied_image, x, y, image_pixel);
		}
	}
	return applied_image;
}

int comparator(std::vector<int> a, std::vector<int> b)
{
	std::vector<double> aa, bb;
	double suma = 0,
		   sumb = 0;
	for (int i = 0; i < 8; ++i)
	{
		suma += a[i];
		sumb += b[i];
	}

	bool flag = 1;
	double count = 0;
	for (int i = 0; i < 8; ++i)
	{
		aa[i] = a[i] / suma;
		bb[i] = b[i] / sumb;
		if ((aa[i] > 0.01 && bb[i] < 0.01) || (aa[i] < 0.01 || bb[i] > 0.01))
			flag = 0;
		count = std::min(aa[i], bb[i]);
	}
	if (flag)
	{
		return count;
	}
	else
	{
		return 0;
	}
}

int symmetric_difference(std::vector<char> a, std::vector<char> b) {
	int sum = 0;
	for (int i = 0; i < a.size(); i++) {
		sum += abs((int)(unsigned char)a[i] - (int)(unsigned char)b[i]);
	}
	return sum;
}

const int SIZE = 100;

double pixel_cmp(cv::Mat &a, cv::Mat &b)
{
	cv::Mat img_a, img_b;

	if (a.rows != SIZE)
		resize(a, img_a, cv::Size(SIZE, SIZE), 0, 0, 1);
	else
		img_a = a;

	if (b.rows != SIZE)
		resize(b, img_b, cv::Size(SIZE, SIZE), 0, 0, 1);
	else
		img_b = b;

	int equal = 0;

	for (int x = 0; x < SIZE; x++) {
		for (int y = 0; y < SIZE; y++) {
			auto pixel_a = get(img_a, x, y);
			auto pixel_b = get(img_b, x, y);
			if (pixel_a == COLOR_BLACK || pixel_b == COLOR_BLACK) continue;
			if (symmetric_difference(pixel_a, pixel_b) < 100) equal++;
		}
	}

	return equal;
}
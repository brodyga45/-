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
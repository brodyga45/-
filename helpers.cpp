#include "helpers.h"

std::vector<std::vector<std::vector<char>>> mat_to_vec(cv::Mat image) {
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

std::vector<char> get(cv::Mat image, int x, int y) {
	std::vector<char> pixel;
	char* ptr = (char*)((long long)(image.datastart) + y * image.step.buf[0] + x * image.step.buf[1]);
	for (int k = 0; k < image.step.buf[1]; k++) {
		pixel.push_back(*ptr);
		ptr++;
	}
	return pixel;
}
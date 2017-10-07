#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

const std::vector<char> COLOR_BLACK = { (char)0, (char)0, (char)0 };
const std::vector<char> COLOR_WHITE = { (char)255, (char)255, (char)255 };

std::vector<std::vector<std::vector<char>>> mat_to_vec(cv::Mat &image);
std::vector<char> get(cv::Mat &image, int x, int y);
void set(cv::Mat &image, int x, int y, std::vector<char> new_value);
void image_invert(cv::Mat &image);
void erode_image(cv::Mat& image, int erosion_size);
void dilate_image(cv::Mat& image, int dilation_size);
void select_color(cv::Mat &image, int hue_min, int hue_max, int sat_min, int sat_max, int val_min, int val_max);
void cut_level(cv::Mat &image, int color, double threshold);
cv::Mat min_filter(cv::Mat &image_1, cv::Mat &image_2);
cv::Mat apply_mask(cv::Mat &image, cv::Mat &mask);

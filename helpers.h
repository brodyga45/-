#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vector>

const std::vector<char> COLOR_BLACK = { (char)0, (char)0, (char)0 };
const std::vector<char> COLOR_WHITE = { (char)255, (char)255, (char)255 };

std::vector<std::vector<std::vector<char>>> mat_to_vec(cv::Mat &image);
std::vector<char> get(cv::Mat &image, int x, int y);
void set(cv::Mat &image, int x, int y, std::vector<char> new_value);

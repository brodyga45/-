#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include "helpers.h"

cv::Mat big_white(cv::Mat &image);
std::vector<cv::Mat> chop_image(cv::Mat &etalon, cv::Mat &image);
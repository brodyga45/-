#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <string>
#include <stdlib.h>
#include "helpers.h"

cv::Mat count_threshold(cv::Mat &image, std::string type);
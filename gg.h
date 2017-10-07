#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <string>
#include <stdlib.h>
#include "helpers.h"

std::vector<int> count_threshold(cv::Mat &image, int type);
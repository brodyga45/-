#pragma once
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "helpers.h"

std::vector < std::vector <long double> > preob(int vl, int vr);
cv::Mat norm_image(cv::Mat im);
#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vector>

std::vector<std::vector<std::vector<char>>> mat_to_vec(cv::Mat image);

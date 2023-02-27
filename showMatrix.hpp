#pragma once
#include <vector>
#include <opencv2/opencv.hpp>
void save(const std::vector<std::vector<int>>& m,const char* path) {
	cv::Mat mat(m.size() * 10 + 20, m.size() * 10 + 20, CV_8UC3);
	
	const auto white = cv::Scalar(255, 255, 255);
	const auto black = cv::Scalar(0, 0, 0);
	const auto gray = cv::Scalar(128, 128, 128);
	mat = white;

	auto get_bit = [&mat](int y, int x)->auto {
		return mat.rowRange(y * 10 + 10, y * 10 + 20).colRange(x * 10 + 10, x * 10 + 20);
	};
	for (int y = 0; y < m.size(); y++) {
		for (int x = 0; x < m.size(); x++) {
			if (m[y][x] == 1)get_bit(y, x) = black;
			else if (m[y][x] == 0)get_bit(y, x) = white;
			else get_bit(y, x) = gray;
		}
	}
	//cv::namedWindow("OpenCV Window");
	//cv::imshow("OpenCV Window", mat);
	//cv::waitKey();
	cv::imwrite(path, mat);
}
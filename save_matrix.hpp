#pragma once
#ifdef __EMSCRIPTEN__
#include <string>
#include <sstream>
#include <vector>
#include <boost/format.hpp>

#include <emscripten.h>
std::string makeSVG(const std::vector<std::vector<int>>& mat) {
	std::ostringstream oss;
	oss << boost::format(R"---(<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" xml:space="preserve" width="%1%" height="%1%" viewBox="0 0 %1% %1%">)---") % (mat.size() * 10 + 20);
	oss << boost::format(R"---(<rect width="%1%" height="%1%" fill="#ffffff" x="0" y="0"/>)---")%(mat.size() * 10 + 20);
	for (int y = 0; y < mat.size(); y++) {
		for (int x = 0; x < mat[0].size(); x++) {
			if (mat[y][x]) {
				oss << boost::format(R"---(<rect width="10" height="10" fill="#000000" x="%1%" y="%2%"/>)---") % (x * 10 + 10) % (y * 10 + 10);
			}
		}
	}
	oss << "</svg>";
	return oss.str();
}
EM_JS(void, show_in_div, (const char* str, const char* path), {
	console.log(UTF8ToString(path));
	document.getElementById(UTF8ToString(path)).innerHTML = UTF8ToString(str);
});
void save(const std::vector<std::vector<int>>& m, const char* path) {
	show_in_div(makeSVG(m).c_str(),path);
	emscripten_sleep(0);
}
#else
#include <vector>
#include <opencv2/opencv.hpp>
void save(const std::vector<std::vector<int>>& m,const char* path) {
	cv::Mat mat((int)m.size() * 10 + 20, (int)m.size() * 10 + 20, CV_8UC3);
	
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
#endif

#include <opencv2/opencv.hpp>
using namespace cv;

void main() {
	Mat src = imread("lenna.jpg", IMREAD_COLOR);
	Mat dst;
	float weights[9] = { -1, -1, -1, -1, 9, -1, -1, -1, -1 };
	Mat mask = Mat(3, 3, CV_32F, weights);

	filter2D(src, dst, -1, mask, Point(-1, -1), 0, BORDER_DEFAULT);
	imshow("src", src);
	imshow("sharpen", dst);
	waitKey(0);
}

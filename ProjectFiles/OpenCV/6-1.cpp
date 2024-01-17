#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

void main()
{
	Mat src = imread("lenna.jpg", IMREAD_GRAYSCALE);
	Mat dst(src.size(), CV_8U, Scalar(0));
	for (int y = 1; y < src.rows - 1; y++) 	// (1, 1)부터 (rows-1, cols-1) 까지
		for (int x = 1; x < src.cols - 1; x++) {   // 의 화소만 처리
			int sum = 0;
			sum += src.at<uchar>(y - 1, x - 1);
			sum += src.at<uchar>(y, x - 1);
			sum += src.at<uchar>(y + 1, x - 1);
			sum += src.at<uchar>(y - 1, x);
			sum += src.at<uchar>(y, x);
			sum += src.at<uchar>(y + 1, x);
			sum += src.at<uchar>(y - 1, x + 1);
			sum += src.at<uchar>(y, x + 1);
			sum += src.at<uchar>(y + 1, x + 1);
			dst.at<uchar>(y, x) = sum / 9;
		}
	imshow("image", src);	imshow("blur", dst);
}

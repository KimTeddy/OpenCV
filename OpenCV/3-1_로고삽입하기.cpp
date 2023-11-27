#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main() {
	Mat A = imread("wave.jpg");
	Mat B = imread("logo.jpg");
	Mat C(A, Rect(A.cols - B.cols, A.rows - B.rows, B.cols, B.rows));
	Mat C1(A, Rect(0, 0, B.cols, B.rows));
	Mat C2(A, Rect(0, A.rows - B.rows, B.cols, B.rows));
	Mat C3(A, Rect(A.cols - B.cols, 0, B.cols, B.rows));
	Mat C4(A, Rect((A.cols - B.cols) / 2, (A.rows - B.rows) / 2, B.cols, B.rows));
	B.copyTo(C);
	B.copyTo(C1);
	B.copyTo(C2);
	B.copyTo(C3);
	B.copyTo(C4);
	line(A, Point(0, 0), Point(A.cols, A.rows), Scalar(255, 255, 255), 3);
	line(A, Point(A.cols, 0), Point(0, A.rows), Scalar(255, 255, 255), 3);
	imshow("result", A);
	waitKey(0);
}

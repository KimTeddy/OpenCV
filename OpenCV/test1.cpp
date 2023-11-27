#include <opencv2/opencv.hpp>
using namespace cv;

int main()
{
	Mat image = Mat(400, 600, CV_8UC3, Scalar(0, 0, 0));

	rectangle(image, Point(100, 100), Point(500, 300), Scalar(0, 255, 0), 5);
	circle(image, Point(300, 200), 100, Scalar(255, 0, 0), 3);
	circle(image, Point(300, 200), 200, Scalar(255, 0, 255), 3);
	imshow("Image", image);
	waitKey(0);
	return(0);
}
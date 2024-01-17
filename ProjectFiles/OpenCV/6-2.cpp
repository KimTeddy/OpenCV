#include <opencv2/opencv.hpp>
using namespace cv;
void main()
{
	Mat image = imread("lenna.jpg", IMREAD_GRAYSCALE);
	float weights[] = {
		1 / 9.0F, 1 / 9.0F, 1 / 9.0F,
		1 / 9.0F, 1 / 9.0F, 1 / 9.0F,
		1 / 9.0F, 1 / 9.0F, 1 / 9.0F };

	Mat mask(3, 3, CV_32F, weights);
	Mat blur;
	filter2D(image, blur, -1, mask);

	imshow("image", image);
	imshow("blur", blur);
	waitKey(0);
}

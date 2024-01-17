#define _CRT_SECURE_NO_WARNINGS

#include <opencv2/opencv.hpp>





using namespace cv;



int main()

{

	Mat img, dst, dst2;

	Ptr<CLAHE> clahe;

	img = imread("lenna.jpg", 0);

	imshow("src", img);



	// create a CLAHE object(Arguments are optional).

	clahe = createCLAHE(2.0, Size(8, 8));

	clahe->apply(img, dst);

	imshow("Adaptive Hist Eq.", dst);



	equalizeHist(img, dst2);

	imshow("Hist Eq.", dst2);



	waitKey(0);

}
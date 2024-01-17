#include "opencv2/opencv.hpp"
using namespace cv;

int main()
{
    VideoCapture cap(0);
        if (!cap.isOpened())
            return -1;
    while (1) {
        Mat frame, imgHSV, imgThresholded;
        cap >> frame;
        cvtColor(frame, imgHSV, COLOR_BGR2HSV);

        inRange(imgHSV, Scalar(120-20, 100, 100), Scalar(120+20, 255, 255), imgThresholded);
        frame.copyTo(imgThresholded, imgThresholded);
        imshow("frame", frame);
        imshow("dst", imgThresholded);
        if (waitKey(30) >= 0) break;
    }
    waitKey(0);
    return 0;
}


//void main()
//{
//	Mat img1 = imread("chroma.jpg", IMREAD_COLOR);
//	Mat img2 = imread("beach.png", IMREAD_COLOR);
//	Mat hsv, msk;
//	cvtColor(img1, hsv, COLOR_BGR2HSV);
//	inRange(hsv, Scalar(60 - 10, 50, 100), Scalar(60 + 10, 255, 255), msk);
//
//	Mat dst, dst1, dst2, inv;
//	bitwise_not(msk, inv);
//	img1.copyTo(img2, inv);
//	//img2.copyTo(dst2, msk);
//	//bitwise_or(dst1, dst2, dst);
//
//	imshow("img1", img1); imshow("img2", img2); imshow("invert", inv);
//	imshow("green", msk); /*imshow("dst1", dst1); imshow("dst2", dst2); imshow("dst", dst);*/
//	waitKey(0);
//}

//int main()
//{
//	Mat img = imread("chroma.jpg", IMREAD_COLOR);
//	Mat img2 = imread("beach.png", IMREAD_COLOR);
//	Mat converted;
//	cvtColor(img, converted, COLOR_BGR2HSV);
//	Mat greenScreen = converted.clone();
//	inRange(converted, Scalar(60 - 10, 100, 100), Scalar(60 + 10, 255, 255), greenScreen);
//
//	Mat dst, dst1, inverted;
//	bitwise_not(greenScreen, inverted);
//	bitwise_and(img, img, dst, inverted);
//	bitwise_or(dst, img2, dst1, greenScreen);
//	bitwise_or(dst, dst1, dst1);
//
//	imshow("img", img);
//	imshow("green", greenScreen);
//	imshow("dst", dst);
//	imshow("dst1", dst1);
//	waitKey(0);
//	return 0;
//}

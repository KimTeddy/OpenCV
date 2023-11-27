//#include "opencv2/opencv.hpp"
//using namespace cv;
//
//void main()
//{
//	Mat img1 = imread("lenna.jpg", IMREAD_COLOR);
//	imshow("Original Image", img1);
//
//	Mat table(1, 256, CV_8UC3, Scalar(0, 0, 0));
//
//	Vec3b* p = (Vec3b*)table.ptr();
//	for (int i = 0; i < 256; ++i) {
//		p[i][0] = i; p[i][1] = 0; p[i][2] = 0;
//	}
//
//	Mat img2;
//	LUT(img1, table, img2);
//
//	imshow("New Image", img2);
//	waitKey(0);
//}

#include <opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;

Mat img, imgclone;
int red, green, blue;
bool drawing = false;

int x1, Y1, x2, y2;

void on_trackbar(int, void*) {     }

void drawSquare(int event, int x, int y, int flags, void* param) {
    if (event == EVENT_LBUTTONDOWN) {
        cout << "( " << x << ", " << y << " )" << endl;
        if (img.channels() == 1)
            cout << "[ " << (int)(img.at<uchar>(y, x)) << " ]" << endl;
        else {
            cout << "[ " << (int)(img.at<Vec3b>(y, x)[0]) << ", "
                << (int)(img.at<Vec3b>(y, x)[1]) << ", "
                << (int)(img.at<Vec3b>(y, x)[2]) << " ]" << endl;
        }
    }
}

int main()
{
    img = imread("lenna.jpg"/*,IMREAD_GRAYSCALE*/);
    if (img.empty()) {
        cout << "영상을 읽을 수 없음" << endl;  return -1;
    }
    //namedWindow("img", 1);
    imshow("img", img);
    setMouseCallback("img", drawSquare);
    waitKey(0);
    return 0;
}
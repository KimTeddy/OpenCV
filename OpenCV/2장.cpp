#include <opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;

Mat img, imgclone;
int red, green, blue;
bool drawing = false;

int x1, Y1, x2, y2, xx, yy;

void on_trackbar(int, void*) {     }

void drawSquare(int event, int x, int y, int flags, void* param) {
    if (event == EVENT_LBUTTONDOWN) {
        x1 = x;
        Y1 = y;
        drawing = true;
        imgclone = img.clone();
    }

    else if (event == EVENT_MOUSEMOVE) {
        if (drawing == true) {
        img = imgclone.clone();
        if (x < x1 && y < Y1){
            xx = x; yy = y;
        }
        else if (x < x1)
        {
            xx = x;  yy = Y1;
        }
        else if (y < Y1)
        {
            xx = x1; yy = y;
        }
        if(x<x1||y<Y1)
        rectangle(img, Rect(xx, yy, abs(x - x1), abs(y - Y1)), Scalar(blue, green, red), 1);
        else
            rectangle(img, Rect(x1, Y1, x - x1, y - Y1), Scalar(blue, green, red), 1);
        }
        //    circle(img, Point(x, y), 3, Scalar(blue, green, red), 10);
    }
    else if (event == EVENT_LBUTTONUP) {
        drawing = false;
        x2 = x;
        y2 = y;
        rectangle(img, Rect(x1, Y1, x2 - x1, y2 - Y1), Scalar(blue, green, red), 3);
        imshow("img", img);
    }
    imshow("img", img);
};

int main()
{
    img = imread("lenna.jpg");
    if (img.empty()) {
        cout << "영상을 읽을 수 없음" << endl;  return -1;
    }
    //namedWindow("img", 1);
    imshow("img", img);
    setMouseCallback("img", drawSquare);
    createTrackbar("R", "img", &red, 255, on_trackbar);
    createTrackbar("G", "img", &green, 255, on_trackbar);
    createTrackbar("B", "img", &blue, 255, on_trackbar);
    waitKey(0);
    return 0;
}
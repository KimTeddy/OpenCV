/*
#include "opencv2/opencv.hpp"
using namespace cv;

void main()
{
    Mat src = imread("lenna.jpg");
    Mat dst;

    blur(src, dst, Size(11, 11));
    imshow("source", src);
    imshow("result", dst);

    waitKey(0);
}
 */ 
/*
#include <opencv2/opencv.hpp>
using namespace cv;

int main()
{
    Mat src = imread("lenna.jpg", 1);
    Mat dst;
    imshow("src", src);

    for (int i = 1; i < 61; i = i + 2) {
        GaussianBlur(src, dst, Size(i, i), 0, 0);
        imshow("Gaussian filter", dst);
        waitKey(1000);
    }
}
*/
#include <opencv2/opencv.hpp>
using namespace cv;

int main()
{
    Mat src = imread("lenna.jpg", IMREAD_GRAYSCALE);
    Mat dst;

    Mat noise_img = Mat::zeros(src.rows, src.cols, CV_8U);
    randu(noise_img, 0, 255);

    Mat black_img = noise_img < 10;
    Mat white_img = noise_img > 245;

    Mat src1 = src.clone();
    src1.setTo(255, white_img);
    src1.setTo(0, black_img);
    medianBlur(src1, dst, 5);
    imshow("source", src1);	imshow("result", dst);
    waitKey(0);
}

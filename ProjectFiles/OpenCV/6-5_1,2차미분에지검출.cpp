/*
#include <opencv2/opencv.hpp>
using namespace cv;

int main()
{
    Mat src, grad, grad_x, grad_y, abs_grad_x, abs_grad_y;
    int scale = 1, delta = 0;
    src = imread("lenna.jpg", IMREAD_GRAYSCALE);

    Sobel(src, grad_x, CV_16S, 1, 0, 3, scale, delta, BORDER_DEFAULT);
    Sobel(src, grad_y, CV_16S, 0, 1, 3, scale, delta, BORDER_DEFAULT);

    convertScaleAbs(grad_x, abs_grad_x);
    convertScaleAbs(grad_y, abs_grad_y);
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
    imshow("Image", src);
    imshow("Sobel", grad);
    waitKey(0);
    return 0;
}
*/
#include <opencv2/opencv.hpp>
using namespace cv;

void main()
{
    Mat src, src_gray, dst, abs_dst;
    int kernel_size = 3, scale = 1, delta = 0, ddepth = CV_16S;

    src = imread("lenna.jpg", IMREAD_GRAYSCALE);

    GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);
    Laplacian(src, dst, ddepth, kernel_size, scale, delta,
        BORDER_DEFAULT);
    convertScaleAbs(dst, abs_dst);

    imshow("Image", src);    imshow("Laplacian", abs_dst);
    waitKey(0);
}

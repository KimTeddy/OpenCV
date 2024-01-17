//#include <opencv2/opencv.hpp>
//#include <iostream>
//using namespace cv;
//using namespace std;
//
//void main()
//{
//	Mat R = Mat(400, 600, CV_8UC3);
//	randu(R, Scalar(0, 255, 0), Scalar(0, 0, 0));
//	Mat R2 = Mat(400, 600, CV_8UC3);
//	randu(R2, Scalar(0, 255, 100), Scalar(0, 0, 0));
//	Mat B = R2.clone();
//	R.copyTo(B);
//	imshow("img", R);
//	imshow("img3", R2);
//	imshow("img2", B);
//	waitKey(0);
//}

//#include <opencv2/opencv.hpp>
//#include <iostream>
//using namespace cv;
//using namespace std;
//
//void sub(Mat img)
//{
//    img = Scalar(0, 0, 0);  // 영상의 모든 화소를 0으로 만든다. 
//}
//void main()
//{
//    Mat A, B;
//    A = imread("lenna.jpg", IMREAD_COLOR);
//    B = A;
//    imshow("before", A);   // 함수 호출 전에 영상을 표시한다. 
//    sub(A);		       // 함수에 영상을 전달한다. 
//    imshow("after", B);      // 함수 호출 후에 영상을 표시한다. 
//    waitKey();
//}

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;
void main()
{
    Mat A;
    A = imread("lenna.jpg", IMREAD_COLOR);

    Rect r(10, 10, 100, 100);
    Mat D = A(r);     // 사각형 관심 영역을 얕은 복사한다. 
    D = Scalar(0, 0, 0); // 관심 영역의 모든 화소를 (0, 0, 0) 으로 만든다. 
    imshow("src", A);
    waitKey();
}

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
//    img = Scalar(0, 0, 0);  // ������ ��� ȭ�Ҹ� 0���� �����. 
//}
//void main()
//{
//    Mat A, B;
//    A = imread("lenna.jpg", IMREAD_COLOR);
//    B = A;
//    imshow("before", A);   // �Լ� ȣ�� ���� ������ ǥ���Ѵ�. 
//    sub(A);		       // �Լ��� ������ �����Ѵ�. 
//    imshow("after", B);      // �Լ� ȣ�� �Ŀ� ������ ǥ���Ѵ�. 
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
    Mat D = A(r);     // �簢�� ���� ������ ���� �����Ѵ�. 
    D = Scalar(0, 0, 0); // ���� ������ ��� ȭ�Ҹ� (0, 0, 0) ���� �����. 
    imshow("src", A);
    waitKey();
}

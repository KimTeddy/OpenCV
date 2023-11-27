//#include <opencv2/opencv.hpp>
//#include<iostream>
//using namespace std;
//using namespace cv;
//
//int main() {
//	Mat m1(2, 2, CV_8UC1, 255),
//		m2(2, 2, CV_8UC1, 1),
//		m3(2, 2, CV_8UC1),
//		m4(2, 2, CV_8UC1);
//
//	m3 = m1 + 10;
//	m4 = m2 - 10;
//	cout << m3 << endl;
//	cout << m4 << endl;
//	waitKey(0);
//}

//#include <opencv2/opencv.hpp>
//#include<iostream>
//using namespace std;
//using namespace cv;
//
//int main() {
//	Mat m1(2, 2, CV_8UC1, 1),
//		m2(2, 2, CV_8UC1, 2),
//		m3(2, 2, CV_8UC1, 3),
//		m4(2, 2, CV_8UC1, 4);
//
//	m2 = m1;
//	m2 = m2 + 1;
//	m3 = m1.clone();
//	m3.copyTo(m4);
//	cout << m1 << endl;
//	cout << m2 << endl;
//	cout << m3 << endl;
//	cout << m4 << endl;
//	waitKey(0);
//}

#include <opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;
int main() {
	Point p1(10, 10), p2(20, 20);
	Size sz1(10, 10);
	Rect rect1(p1, p2), rect2, rect3, rect4, rect5;

	rect2 = rect1 + p1;
	rect3 = rect1 + sz1;
	rect4 = rect1 | rect2;
	rect5 = rect1 & rect3;

	cout << rect1.width << "x" << rect1.height << endl;
	cout << rect2.width << "x" << rect2.height << endl;
	cout << rect3.width << "x" << rect3.height << endl;
	cout << rect4.width << "x" << rect4.height << endl;
	cout << rect5.width << "x" << rect5.height << endl;
}
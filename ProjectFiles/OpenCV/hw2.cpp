#include <iostream>
#include <opencv2/opencv.hpp>
#include <GL/glui.h>
#include "OpenFileDialog.h"
#include "SaveFileDialog.h"

using namespace cv;
using namespace std;

int main_window;
String Filename;

GLUI* glui;
GLUI_Button* btn_save;
GLUI_Rollout* rollout_th, * rollout_canny, * rollout_findcolor, * rollout_openclose, * rollout_wrap, * rollout_corner, * rollout_label;
GLUI_Panel* panel_canny;
GLUI_Panel* panel_threshold;
GLUI_Panel* panel_histeq;
GLUI_Panel* panel_mv;
GLUI_Panel* panel_findcolor;
GLUI_Panel* panel_openclose;
GLUI_Panel* panel_hpf;
GLUI_Panel* panel_labeling;
GLUI_Panel* panel_gaussian;
GLUI_Panel* panel_corner;

GLUI_Panel* movedots;
GLUI_Panel* radiobutton;
GLUI_RadioGroup* radiogroup;
GLUI_Translation* dots[4];

void Canny_ui(GLUI_Panel* panel_canny_in);
void Threshold_ui(GLUI_Panel* panel_threshold);
void Wrap_ui(GLUI_Panel* panel_move);
void Findcolor_ui(GLUI_Panel* panel_find);
void openclose_ui(GLUI_Panel* panel_oc);
void corner_ui(GLUI_Panel* panel_c);
void labeling_ui(GLUI_Panel* panel_l);

void just_sync(int id);
void CannyThreshold(int id);
void HistogramEqualize(int id);
void Threshold(int id);
void translation(int id);
void radioButtonCallback(int id);


void idle();
void checkbox(int id);

int isclose = 0;

Mat src, dst, edges, gray;
int input_width, input_height;
//canny
int lowTh = 40, ratioo = 3, kernel_size = 3, max_lowTh = 100;
int blur_size = 3;
//transition
float trans_dot_pos[4][3] = { 0.0 };
float now_dot_pos[4][3] = { 0.0 };
float prev_pos[4][3] = { 0.0 };
float set_dots[4][2] = { 0 };
int selected_point;

void just_sync(int id) {
	GLUI_Master.sync_live_all();
}
#define NONE 0
#define CANNY 1
#define THRESHOLD 2
#define HISTEQ 3
#define WRAP 4
#define FINDCOLOR 5
#define OPENCLOSE 6
#define HPF 7
#define LABELING 8
#define GAUSSIAN 9
#define CORNER 10

int mode = NONE;

#define CHECHBOX_SIZE 11
int v[CHECHBOX_SIZE] = { 1 };
GLUI_Checkbox* check[CHECHBOX_SIZE];

const char* checkbox_names[CHECHBOX_SIZE] = {
	"None",//0
	"CannyThreshold",//1
	"Threshold",//2
	"HistogramEqualize",//3
	"Wrap",//4
	"FindColor",//5
	"OpenClose",//6
	"HPF",//7
	"Labeling",//8
	"Gaussian",//9
	"Corner"//10
};
//4,5,6,7,8,9,10,11,12
//---------------------------------------------------------------------------------------------------------------
int open() {
	destroyAllWindows();
	OpenFileDialog* openFileDialog = new OpenFileDialog();
	if (openFileDialog->ShowDialog()) {
		Filename = openFileDialog->FileName;
		return(1);
	}
	else return(-1);
}

bool isopen = false;
void OpenImage(int id) {
	if (open() == 1) {
		namedWindow("Input", 1);
		src = imread(Filename);
		gray = imread(Filename, IMREAD_GRAYSCALE);
		src.copyTo(dst);
		imshow("Input", src);
		imshow("Output", dst);

		input_width = src.size().width;
		input_height = src.size().height;
		cout << "input_width: " << input_width << "input_height: " << input_height << endl;

		//모든 기능 활성화
		//check[NONE]->enable();
		check[CANNY]->enable();
		check[THRESHOLD]->enable();
		check[HISTEQ]->enable();
		check[WRAP]->enable();
		check[FINDCOLOR]->enable();
		check[OPENCLOSE]->enable();
		check[HPF]->enable();
		check[LABELING]->enable();
		check[GAUSSIAN]->enable();
		check[CORNER]->enable();

		btn_save->enable();
		rollout_canny->enable();
		rollout_th->enable();
		rollout_findcolor->enable();
		rollout_openclose->enable();
		rollout_corner->enable();
		rollout_label->enable();
		rollout_wrap->enable();

		radioButtonCallback(0);
		radiobutton->enable();

		now_dot_pos[0][0] = 0; now_dot_pos[0][1] = 0;				now_dot_pos[2][0] = input_width; now_dot_pos[2][1] = 0;
		now_dot_pos[1][0] = 0; now_dot_pos[1][1] = input_height;	now_dot_pos[3][0] = input_width; now_dot_pos[3][1] = input_height;

		set_dots[0][0] = 0; set_dots[0][1] = 0;						set_dots[2][0] = input_width; set_dots[2][1] = 0;
		set_dots[1][0] = 0; set_dots[1][1] = -input_height;			set_dots[3][0] = input_width; set_dots[3][1] = -input_height;

		//setMouseCallback(Filename, drawCircle);

		GLUI_Master.set_glutIdleFunc(NULL);
		isopen = true;
	}
}

String SaveFilename;
int Save() {//all record 추가하기
	SaveFileDialog* openFileDialog = new SaveFileDialog();
	if (openFileDialog->ShowDialog()) {
		SaveFilename = openFileDialog->FileName;
		imwrite(SaveFilename, dst);
		return 0;
	}
	return -1;
}
void SaveImage(int id) {
	Save();
}


//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
void drawHist(Mat& out, char mode, int histB[], int histG[], int histR[])
{
	int hist_h = input_height / 4;  // 히스토그램 영상의 높이
	Mat histImg(hist_h, 512, CV_8UC3, Scalar(0, 0, 0)); // 영상버퍼

	Scalar colorB, colorG, colorR; // 히스토그램 색상
	colorB = Scalar(255, 0, 0);  // 파란색
	colorG = Scalar(0, 255, 0);  // 초록색
	colorR = Scalar(0, 0, 255); // 빨간색
	// 히스토그램에서 최대값을 찾는다. 
	int max = histB[1];
	for (int i = 1; i < 256; i++) {
		if (max < histB[i])
			max = histB[i];
		if (max < histG[i])
			max = histG[i];
		if (max < histR[i])
			max = histR[i];
	}
	// 히스토그램 배열을 최대값(최대 높이)으로 정규화. 
	for (int i = 0; i <= 255; i++) {
		histB[i] = floor(((double)histB[i] / max) * hist_h);
		histG[i] = floor(((double)histG[i] / max) * hist_h);
		histR[i] = floor(((double)histR[i] / max) * hist_h);
	}
	// 히스토그램을 막대그래프로 그린다. 
	for (int i = 1; i <= 255 * 3; i++) {
		line(histImg, Point(3 * i - 2, hist_h), Point(3 * i - 2, hist_h - histB[i]), colorB);
		line(histImg, Point(3 * i - 1, hist_h), Point(3 * i - 1, hist_h - histG[i]), colorG);
		line(histImg, Point(3 * i, hist_h), Point(3 * i, hist_h - histR[i]), colorR);
	}

	//Mat C(out, Rect(0, 0, histImg.cols, histImg.rows));
	histImg.copyTo(out);
	//imshow("Histogram", histImg);
	//switch (mode) {
	//case 'B': imshow("Histogram Blue", histImg); break;
	//case 'G': imshow("Histogram Green", histImg); break;
	//case 'R': imshow("Histogram Red", histImg); break;
	//case 'K': imshow("Histogram", histImg); break;
	//}
}
int ishist = false;
void showHist(Mat& in, int index) {
	if (ishist) {
		Mat histogram, hist_after;
		if (in.channels() == 1) { // GrayScale 영상 경우
			int hist[3][256] = { 0 };
			for (int y = 0; y < in.rows; y++)
				for (int x = 0; x < in.cols; x++)
					hist[0][(int)in.at<uchar>(y, x)]++;
			drawHist(histogram, 'K', hist[0], hist[0], hist[0]);
		}
		else {  // Color 영상 경우
			int hist[3][256] = { 0 };
			for (int y = 0; y < in.rows; y++)
				for (int x = 0; x < in.cols; x++)
					for (int c = 0; c < 3; c++)
						hist[c][(int)in.at<Vec3b>(y, x)[c]]++;
			drawHist(histogram, 'C', hist[0], hist[1], hist[2]);
		}
		//Mat C(frame, Rect(0, 0, histogram.cols, histogram.rows));
		//bitwise_or(histogram, C, frame);
		//histogram.copyTo(C);
		if (index == 0)
			imshow("Before Histogram", histogram);
		else
			imshow("After Histogram", histogram);
		//imshow("Before Histogram", C);
	}
}

//---------------------------------------------------------------------------------------------------------------
void CannyThreshold(int id) {//6장
	if (mode == CANNY) {
		dst.create(src.size(), src.type());
		//edittext_lowTh->set_int_val(lowTh);
		blur(src, dst, Size(blur_size, blur_size));
		Canny(dst, edges, lowTh, lowTh * ratioo, kernel_size);
		dst = Scalar::all(0);
		src.copyTo(dst, edges);
		imshow("Input", src);
		imshow("Output", dst);
	}
	GLUI_Master.sync_live_all();
}

//---------------------------------------------------------------------------------------------------------------
int threshold_val = 100;
int threshold_type = 0;
void Threshold(int id) {//4장
	if (mode == THRESHOLD) {
		threshold(gray, dst, threshold_val, 255, threshold_type);
		imshow("Output", dst);
	}
	GLUI_Master.sync_live_all();
}

//---------------------------------------------------------------------------------------------------------------
Mat src_ycrcb, ycrcb_planes[3];
void HistogramEqualize(int id) {//5장
	if (mode == HISTEQ) {
		cvtColor(src, src_ycrcb, COLOR_BGR2YCrCb);
		split(src_ycrcb, ycrcb_planes);
		equalizeHist(ycrcb_planes[0], ycrcb_planes[0]);
		merge(ycrcb_planes, 3, src_ycrcb);
		cvtColor(src_ycrcb, dst, COLOR_YCrCb2BGR);
		imshow("Output", dst);
	}
}

//---------------------------------------------------------------------------------------------------------------

void Wrap(int id) {//7장
	if (mode == WRAP) {
		Point2f inputp[4], outputp[4];
		inputp[0] = Point2f(0, 0);			inputp[2] = Point2f(dst.cols, 0);
		inputp[1] = Point2f(0, dst.rows);	inputp[3] = Point2f(dst.cols, dst.rows);

		outputp[0] = Point2f(now_dot_pos[0][0], now_dot_pos[0][1]);	outputp[2] = Point2f(now_dot_pos[2][0], now_dot_pos[2][1]);
		outputp[1] = Point2f(now_dot_pos[1][0], now_dot_pos[1][1]);	outputp[3] = Point2f(now_dot_pos[3][0], now_dot_pos[3][1]);

		Mat h = getPerspectiveTransform(inputp, outputp);
		warpPerspective(src, dst, h, dst.size());
		imshow("Output", dst);
	}
	GLUI_Master.sync_live_all();
}
void translation(int id) {
	switch (id) {
	case 0:
		now_dot_pos[0][0] = trans_dot_pos[0][0];
		now_dot_pos[0][1] = -trans_dot_pos[0][1];
		cout << trans_dot_pos[0][0] << ", " << trans_dot_pos[0][1] << endl;
		break;
	case 1:
		now_dot_pos[1][0] = trans_dot_pos[1][0];
		now_dot_pos[1][1] = -trans_dot_pos[1][1];
		cout << trans_dot_pos[1][0] << ", " << trans_dot_pos[1][1] << endl;
		break;
	case 2:
		now_dot_pos[2][0] = trans_dot_pos[2][0];
		now_dot_pos[2][1] = -trans_dot_pos[2][1];
		cout << trans_dot_pos[2][0] << ", " << trans_dot_pos[2][1] << endl;
		break;
	case 3:
		now_dot_pos[3][0] = trans_dot_pos[3][0];
		now_dot_pos[3][1] = -trans_dot_pos[3][1];
		cout << trans_dot_pos[3][0] << ", " << trans_dot_pos[3][1] << endl;
		break;
	}
	Wrap(mode);
}
void radioButtonCallback(int id) {
	switch (selected_point) {
	case 0:
		dots[1]->disable(); dots[2]->disable(); dots[3]->disable(); break;
	case 1:
		dots[0]->disable(); dots[2]->disable(); dots[3]->disable(); break;
	case 2:
		dots[0]->disable(); dots[1]->disable(); dots[3]->disable(); break;
	case 3:
		dots[0]->disable(); dots[1]->disable(); dots[2]->disable(); break;
	}
	dots[selected_point]->set_float_array_val(set_dots[selected_point]);
	dots[selected_point]->enable();
	GLUI_Master.sync_live_all();
}

//---------------------------------------------------------------------------------------------------------------
int H = 90, S = 100, V = 100, range = 10;
void Findcolor(int id) {//9장
	if (mode == FINDCOLOR) {
		Mat imgHSV, imgThresholded;
		cvtColor(src, imgHSV, COLOR_BGR2HSV);

		inRange(imgHSV, Scalar(H - range, S, V), Scalar(H + range, 255, 255), imgThresholded);
		//imshow("inRange", imgThresholded);
		src.copyTo(imgThresholded, imgThresholded);
		imgThresholded.copyTo(dst);
		imshow("Output", dst);
	}
	GLUI_Master.sync_live_all();
}
//---------------------------------------------------------------------------------------------------------------

Mat opening, close;
void OpenClose(int id) {//8장
	if (mode == OPENCLOSE) {
		Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));

		threshold(gray, dst, threshold_val, 255, threshold_type);

		morphologyEx(dst, opening, MORPH_OPEN, element);
		morphologyEx(opening, close, MORPH_CLOSE, element);
		if (isclose) {
			close.copyTo(dst);
		}
		else {
			opening.copyTo(dst);
		}
		imshow("Output", dst);
	}
	GLUI_Master.sync_live_all();
}
//---------------------------------------------------------------------------------------------------------------
//10장
void shuffleDFT(Mat& src)
{
	int cX = src.cols / 2;
	int cY = src.rows / 2;

	Mat q1(src, Rect(0, 0, cX, cY));      Mat q2(src, Rect(cX, 0, cX, cY));
	Mat q3(src, Rect(0, cY, cX, cY));    Mat q4(src, Rect(cX, cY, cX, cY));
	Mat tmp;
	// q1과 q4를 교환
	q1.copyTo(tmp);
	q4.copyTo(q1);
	tmp.copyTo(q4);
	// q2와 q3을 교환
	q2.copyTo(tmp);
	q3.copyTo(q2);
	tmp.copyTo(q3);
}
Mat getFilter(Size size)
{
	Mat filter = Mat::ones(size, CV_32FC2);
	circle(filter, size / 2, 10, Vec2f(0, 0), -1);
	return filter;

}
void hpf(int id) {
	if (mode == HPF) {
		Mat src_float, dft_image;

		gray.convertTo(src_float, CV_32FC1, 1.0 / 255.0);
		dft(src_float, dft_image, DFT_COMPLEX_OUTPUT);
		shuffleDFT(dft_image);
		Mat highpass = getFilter(dft_image.size());
		Mat result, inverted_image;

		multiply(dft_image, highpass, result);

		shuffleDFT(result);
		idft(result, inverted_image, DFT_SCALE | DFT_REAL_OUTPUT);
		inverted_image.copyTo(dst);
		imshow("Output", dst);
	}
}
//---------------------------------------------------------------------------------------------------------------
void labeling(int id) {//11장
	if (mode == LABELING) {
		Mat img, thimg, labels, centroids, img_color, stats;

		//threshold(gray, thimg, 128, 255, THRESH_BINARY_INV);
		threshold(gray, thimg, threshold_val, 255, THRESH_BINARY_INV);
		//imshow("Image after threshold", thimg);

		int n = connectedComponentsWithStats(thimg, labels, stats, centroids);

		vector<Vec3b> colors(n + 1);  // 컬러 버퍼
		colors[0] = Vec3b(0, 0, 0);  // 배경은 검은색
		for (int i = 1; i <= n; i++)  // 각 label에 랜덤컬러 생성
			colors[i] = Vec3b(rand() % 256, rand() % 256, rand() % 256);
		img_color = Mat::zeros(gray.size(), CV_8UC3);
		// label된 각 화소에 컬러값 설정
		for (int y = 0; y < img_color.rows; y++)
			for (int x = 0; x < img_color.cols; x++) {
				int label = labels.at<int>(y, x);
				img_color.at<Vec3b>(y, x) = colors[label];
			}
		img_color.copyTo(dst);
		//imshow("Labeled map", img_color);
		imshow("Output", dst);
	}
	GLUI_Master.sync_live_all();
}
//---------------------------------------------------------------------------------------------------------------
void gaussian(int id) {//11장
	if (mode == GAUSSIAN) {
		Mat img, th;
		medianBlur(gray, img, 5);
		adaptiveThreshold(img, th, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 11, 2);
		th.copyTo(dst);
		imshow("Output", dst);
	}
}
//---------------------------------------------------------------------------------------------------------------
void corner(int id) {//12장
	if (mode == CORNER) {
		if (id == 0) src.copyTo(dst);
		Mat dst2, dst_norm, dst_norm_scaled;
		int thresh = 150, blockSize = 2, Ksize = 3;
		double k = 0.04;
		dst2 = Mat::zeros(src.size(), CV_32FC1);

		cornerHarris(gray, dst2, blockSize, Ksize, k);
		normalize(dst2, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());

		for (int j = 0; j < dst_norm.rows; j++) {
			for (int i = 0; i < dst_norm.cols; i++) {
				if ((int)dst_norm.at<float>(j, i) > threshold_val) {
					circle(dst, Point(i, j), 5, Scalar(0, 0, 255), 2, 8, 0);
				}
			}
		}
		//dst2.copyTo(dst);
		imshow("Output", dst);
	}
	GLUI_Master.sync_live_all();
}

//---------------------------------------------------------------------------------------------------------------
void idle() {
	switch (mode) {
	case CANNY: CannyThreshold(mode); break;
	case THRESHOLD: Threshold(mode); break;
	case HISTEQ: HistogramEqualize(mode); break;
	case WRAP: Wrap(mode); break;
	case FINDCOLOR: Findcolor(mode); break;
	case OPENCLOSE:OpenClose(mode); break;
	case HPF: hpf(mode); break;
	case LABELING:labeling(mode); break;
	case GAUSSIAN: gaussian(mode); break;
	case CORNER:corner(mode); break;
	default: break;
	}
}
//---------------------------------------------------------------------------------------------------------------
void checkbox(int id) {
	mode = id;
	src.copyTo(dst);
	idle();
	for (int i = 0; i < CHECHBOX_SIZE; i++) {
		if (i == id) v[id] = 1;
		else v[i] = 0;
	}
	if (mode == NONE)
		imshow("Output", dst);
	GLUI_Master.sync_live_all();
	check[id]->set_int_val(1);
}
//---------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
	glui = GLUI_Master.create_glui("Menu", 0);
	main_window = glui->get_glut_window_id();
	GLUI_Master.set_glutIdleFunc(NULL);
	cout << main_window << endl;

	glui->add_statictext("60191798 KYC");
	glui->add_separator();

	check[NONE] = glui->add_checkbox("NONE", &v[NONE], NONE, checkbox);
	check[NONE]->set_alignment(GLUI_ALIGN_CENTER);
	//--------------------------------------------------------------------------------------
	panel_canny = glui->add_panel("CANNY", GLUI_PANEL_NONE);
	check[CANNY] = glui->add_checkbox_to_panel(panel_canny, "CANNY", &v[CANNY], CANNY, checkbox);
	glui->add_column_to_panel(panel_canny, false);
	Canny_ui(panel_canny);
	panel_canny->set_alignment(GLUI_ALIGN_LEFT);
	//--------------------------------------------------------------------------------------
	panel_threshold = glui->add_panel("THRESHOLD", GLUI_PANEL_NONE);
	check[THRESHOLD] = glui->add_checkbox_to_panel(panel_threshold, "THRESHOLD", &v[THRESHOLD], THRESHOLD, checkbox);
	glui->add_column_to_panel(panel_threshold, false);
	Threshold_ui(panel_threshold);
	panel_threshold->set_alignment(GLUI_ALIGN_LEFT);

	//--------------------------------------------------------------------------------------
	panel_histeq = glui->add_panel("HISTEQ", GLUI_PANEL_NONE);
	check[HISTEQ] = glui->add_checkbox_to_panel(panel_histeq, checkbox_names[HISTEQ], &v[HISTEQ], HISTEQ, checkbox);
	panel_histeq->set_alignment(GLUI_ALIGN_LEFT);

	//--------------------------------------------------------------------------------------
	panel_mv = glui->add_panel("WRAP", GLUI_PANEL_NONE);
	check[WRAP] = glui->add_checkbox_to_panel(panel_mv, checkbox_names[WRAP], &v[WRAP], WRAP, checkbox);
	glui->add_column_to_panel(panel_mv, false);
	Wrap_ui(panel_mv);
	panel_mv->set_alignment(GLUI_ALIGN_LEFT);

	//--------------------------------------------------------------------------------------
	panel_findcolor = glui->add_panel("FINDCOLOR", GLUI_PANEL_NONE);
	check[FINDCOLOR] = glui->add_checkbox_to_panel(panel_findcolor, checkbox_names[FINDCOLOR], &v[FINDCOLOR], FINDCOLOR, checkbox);
	glui->add_column_to_panel(panel_findcolor, false);
	Findcolor_ui(panel_findcolor);
	panel_findcolor->set_alignment(GLUI_ALIGN_LEFT);

	//--------------------------------------------------------------------------------------
	panel_openclose = glui->add_panel("OPENCLOSE", GLUI_PANEL_NONE);
	check[OPENCLOSE] = glui->add_checkbox_to_panel(panel_openclose, checkbox_names[OPENCLOSE], &v[OPENCLOSE], OPENCLOSE, checkbox);
	glui->add_column_to_panel(panel_openclose, false);
	openclose_ui(panel_openclose);
	panel_openclose->set_alignment(GLUI_ALIGN_LEFT);

	//--------------------------------------------------------------------------------------
	panel_hpf = glui->add_panel("HPF", GLUI_PANEL_NONE);
	check[HPF] = glui->add_checkbox_to_panel(panel_hpf, checkbox_names[HPF], &v[HPF], HPF, checkbox);
	panel_hpf->set_alignment(GLUI_ALIGN_LEFT);

	//--------------------------------------------------------------------------------------
	panel_labeling = glui->add_panel("LABELING", GLUI_PANEL_NONE);
	check[LABELING] = glui->add_checkbox_to_panel(panel_labeling, checkbox_names[LABELING], &v[LABELING], LABELING, checkbox);
	glui->add_column_to_panel(panel_labeling, false);
	labeling_ui(panel_labeling);
	panel_labeling->set_alignment(GLUI_ALIGN_LEFT);

	//--------------------------------------------------------------------------------------
	panel_gaussian = glui->add_panel("GAUSSIAN", GLUI_PANEL_NONE);
	check[GAUSSIAN] = glui->add_checkbox_to_panel(panel_gaussian, checkbox_names[GAUSSIAN], &v[GAUSSIAN], GAUSSIAN, checkbox);
	panel_gaussian->set_alignment(GLUI_ALIGN_LEFT);

	panel_corner = glui->add_panel("CORNER", GLUI_PANEL_NONE);
	check[CORNER] = glui->add_checkbox_to_panel(panel_corner, checkbox_names[CORNER], &v[CORNER], CORNER, checkbox);
	glui->add_column_to_panel(panel_corner, false);
	corner_ui(panel_corner);
	panel_corner->set_alignment(GLUI_ALIGN_LEFT);


	check[CANNY]->disable();
	check[THRESHOLD]->disable();
	check[HISTEQ]->disable();
	check[WRAP]->disable();
	check[FINDCOLOR]->disable();
	check[OPENCLOSE]->disable();
	check[HPF]->disable();
	check[LABELING]->disable();
	check[GAUSSIAN]->disable();
	check[CORNER]->disable();


	//buttons
	glui->add_column(false);
	glui->add_button("Open", 0, OpenImage);
	btn_save = glui->add_button("Save", 0, SaveImage);
	btn_save->disable();
	glui->add_button("Quit", 0, (GLUI_Update_CB)exit);

	//CannyThreshold(0);
	glutIdleFunc(idle);

	glui->set_main_gfx_window(main_window);
	glutMainLoop();
	return EXIT_SUCCESS;
}

void Canny_ui(GLUI_Panel* panel_canny_in) {
	rollout_canny = glui->add_rollout_to_panel(panel_canny_in, "Canny", false);
	GLUI_Panel* panel_cannyth = glui->add_panel_to_panel(rollout_canny, "Canny Settings");

	GLUI_Spinner* spinner_lowTh = glui->add_spinner_to_panel(panel_cannyth, "LowTh : ", GLUI_SPINNER_INT, &lowTh, 0, CannyThreshold);
	spinner_lowTh->set_int_limits(1, max_lowTh, GLUI_LIMIT_CLAMP);
	spinner_lowTh->set_speed(0.001);
	GLUI_Spinner* spinner_blursize = glui->add_spinner_to_panel(panel_cannyth, "BlurSize : ", GLUI_SPINNER_INT, &blur_size, 0, CannyThreshold);
	spinner_blursize->set_int_limits(1, 10, GLUI_LIMIT_CLAMP);
	spinner_blursize->set_speed(0.001);

	glui->add_column_to_panel(panel_cannyth, false);

	GLUI_Scrollbar* scrollbar_lowTh = new GLUI_Scrollbar(panel_cannyth, "lowTh", GLUI_SCROLL_HORIZONTAL, &lowTh, 0, CannyThreshold);
	scrollbar_lowTh->set_int_limits(1, max_lowTh, GLUI_LIMIT_CLAMP);
	scrollbar_lowTh->set_speed(0.001);
	GLUI_Scrollbar* scrollbar_blursize = new GLUI_Scrollbar(panel_cannyth, "BlurSize", GLUI_SCROLL_HORIZONTAL, &blur_size, 0, CannyThreshold);
	scrollbar_blursize->set_int_limits(1, 10, GLUI_LIMIT_CLAMP);
	scrollbar_blursize->set_speed(0.001);

	rollout_canny->disable();
}
void Threshold_ui(GLUI_Panel* panel_threshold) {
	rollout_th = glui->add_rollout_to_panel(panel_threshold, "Threshold", false);
	GLUI_Panel* panel_th = glui->add_panel_to_panel(rollout_th, "Threshold Settings");

	GLUI_Spinner* spinner_thVal = glui->add_spinner_to_panel(panel_th, "thVal : ", GLUI_SPINNER_INT, &threshold_val, 0, Threshold);
	spinner_thVal->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	spinner_thVal->set_speed(0.001);
	GLUI_Listbox* listbox_thType = glui->add_listbox_to_panel(panel_th, "thType : ", &threshold_type, 0, Threshold);
	listbox_thType->add_item(THRESH_BINARY, "BINARY");
	listbox_thType->add_item(THRESH_BINARY_INV, "THRESH_BINARY_INV");
	listbox_thType->add_item(THRESH_TRUNC, "THRESH_TRUNC");
	listbox_thType->add_item(THRESH_TOZERO, "THRESH_TOZERO");
	listbox_thType->add_item(THRESH_TOZERO_INV, "THRESH_TOZERO_INV");
	//listbox_thType->add_item(THRESH_MASK,		"THRESH_MASK");
	//listbox_thType->add_item(THRESH_OTSU,		"THRESH_OTSU");
	//listbox_thType->add_item(THRESH_TRIANGLE,	"THRESH_TRIANGLE");

	glui->add_column_to_panel(panel_th, false);

	GLUI_Scrollbar* scrollbar_thVal = new GLUI_Scrollbar(panel_th, "thVal", GLUI_SCROLL_HORIZONTAL, &threshold_val, 0, Threshold);
	scrollbar_thVal->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	scrollbar_thVal->set_speed(0.001);
	GLUI_Scrollbar* scrollbar_thType = new GLUI_Scrollbar(panel_th, "thType", GLUI_SCROLL_HORIZONTAL, &threshold_type, 0, Threshold);
	scrollbar_thType->set_int_limits(0, 4, GLUI_LIMIT_CLAMP);
	scrollbar_thType->set_speed(0.001);

	rollout_th->disable();
}
void Wrap_ui(GLUI_Panel* panel_move) {
	rollout_wrap = glui->add_rollout_to_panel(panel_move, "Wrap", false);
	GLUI_Panel* move = glui->add_panel_to_panel(rollout_wrap, "Move", GLUI_PANEL_NONE);
	movedots = glui->add_panel_to_panel(move, "Move", GLUI_PANEL_EMBOSSED);
	dots[0] = glui->add_translation_to_panel(movedots, "Left Top", GLUI_TRANSLATION_XY, trans_dot_pos[0], 0, translation);
	dots[1] = glui->add_translation_to_panel(movedots, "Left Bottom", GLUI_TRANSLATION_XY, trans_dot_pos[1], 1, translation);
	glui->add_column_to_panel(movedots, false);
	dots[2] = glui->add_translation_to_panel(movedots, "Right Top", GLUI_TRANSLATION_XY, trans_dot_pos[2], 2, translation);
	dots[3] = glui->add_translation_to_panel(movedots, "Right Bottom", GLUI_TRANSLATION_XY, trans_dot_pos[3], 3, translation);

	movedots->disable();

	glui->add_column_to_panel(move, false);

	radiobutton = glui->add_panel_to_panel(move, "WRAP", GLUI_PANEL_EMBOSSED);
	radiogroup = glui->add_radiogroup_to_panel(radiobutton, &selected_point, 0, radioButtonCallback);
	glui->add_radiobutton_to_group(radiogroup, "Left Top");
	glui->add_radiobutton_to_group(radiogroup, "Left Bottom");
	glui->add_column_to_panel(radiobutton, false);
	glui->add_radiobutton_to_group(radiogroup, "Right Top");
	glui->add_radiobutton_to_group(radiogroup, "Right Bottom");

	radiobutton->disable();
	rollout_wrap->disable();
}
void Findcolor_ui(GLUI_Panel* panel_find) {
	rollout_findcolor = glui->add_rollout_to_panel(panel_find, "FindColor", false);
	GLUI_Panel* panel_findcol = glui->add_panel_to_panel(rollout_findcolor, "HSV Settings");

	GLUI_Panel* panel_drawr = glui->add_panel_to_panel(panel_findcol, "Find Color", GLUI_PANEL_NONE);
	GLUI_Panel* panel_drawg = glui->add_panel_to_panel(panel_findcol, "Find Color", GLUI_PANEL_NONE);
	GLUI_Panel* panel_drawb = glui->add_panel_to_panel(panel_findcol, "Find Color", GLUI_PANEL_NONE);
	GLUI_Panel* panel_drawrange = glui->add_panel_to_panel(panel_findcol, "Find Color", GLUI_PANEL_NONE);

	GLUI_Spinner* spinner_r = glui->add_spinner_to_panel(panel_drawr, "H : ", GLUI_SPINNER_INT, &H, 0, Findcolor);
	spinner_r->set_int_limits(0, 180, GLUI_LIMIT_CLAMP);
	spinner_r->set_speed(0.001);
	glui->add_column_to_panel(panel_drawr, false);
	GLUI_Scrollbar* scrollbar_r = new GLUI_Scrollbar(panel_drawr, "H", GLUI_SCROLL_HORIZONTAL, &H, 0, Findcolor);
	scrollbar_r->set_int_limits(0, 180, GLUI_LIMIT_CLAMP);
	scrollbar_r->set_speed(0.001);

	GLUI_Spinner* spinner_g = glui->add_spinner_to_panel(panel_drawg, "S : ", GLUI_SPINNER_INT, &S, 0, Findcolor);
	spinner_g->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	spinner_g->set_speed(0.001);
	glui->add_column_to_panel(panel_drawg, false);
	GLUI_Scrollbar* scrollbar_g = new GLUI_Scrollbar(panel_drawg, "S", GLUI_SCROLL_HORIZONTAL, &S, 0, Findcolor);
	scrollbar_g->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	scrollbar_g->set_speed(0.001);

	GLUI_Spinner* spinner_b = glui->add_spinner_to_panel(panel_drawb, "V : ", GLUI_SPINNER_INT, &V, 0, Findcolor);
	spinner_b->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	spinner_b->set_speed(0.001);
	glui->add_column_to_panel(panel_drawb, false);
	GLUI_Scrollbar* scrollbar_b = new GLUI_Scrollbar(panel_drawb, "V", GLUI_SCROLL_HORIZONTAL, &V, 0, Findcolor);
	scrollbar_b->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	scrollbar_b->set_speed(0.001);

	GLUI_Spinner* spinner_range = glui->add_spinner_to_panel(panel_drawrange, "Range : ", GLUI_SPINNER_INT, &range, 0, Findcolor);
	spinner_range->set_int_limits(0, 100, GLUI_LIMIT_CLAMP);
	spinner_range->set_speed(0.001);
	glui->add_column_to_panel(panel_drawrange, false);
	GLUI_Scrollbar* scrollbar_range = new GLUI_Scrollbar(panel_drawrange, "Range", GLUI_SCROLL_HORIZONTAL, &range, 0, Findcolor);
	scrollbar_range->set_int_limits(0, 100, GLUI_LIMIT_CLAMP);
	scrollbar_range->set_speed(0.001);

	rollout_findcolor->disable();
}
void openclose_ui(GLUI_Panel* panel_oc) {
	rollout_openclose = glui->add_rollout_to_panel(panel_oc, "OpenClose", false);
	GLUI_Panel* panel_th = glui->add_panel_to_panel(rollout_openclose, "Threshold Settings");

	GLUI_Spinner* spinner_thVal = glui->add_spinner_to_panel(panel_th, "thVal : ", GLUI_SPINNER_INT, &threshold_val, 0, OpenClose);
	spinner_thVal->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	spinner_thVal->set_speed(0.001);
	GLUI_Listbox* listbox_thType = glui->add_listbox_to_panel(panel_th, "thType : ", &threshold_type, 0, OpenClose);
	listbox_thType->add_item(THRESH_BINARY, "BINARY");
	listbox_thType->add_item(THRESH_BINARY_INV, "THRESH_BINARY_INV");
	listbox_thType->add_item(THRESH_TRUNC, "THRESH_TRUNC");
	listbox_thType->add_item(THRESH_TOZERO, "THRESH_TOZERO");
	listbox_thType->add_item(THRESH_TOZERO_INV, "THRESH_TOZERO_INV");

	glui->add_column_to_panel(panel_th, false);

	GLUI_Scrollbar* scrollbar_thVal = new GLUI_Scrollbar(panel_th, "thVal", GLUI_SCROLL_HORIZONTAL, &threshold_val, 0, OpenClose);
	scrollbar_thVal->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	scrollbar_thVal->set_speed(0.001);
	GLUI_Scrollbar* scrollbar_thType = new GLUI_Scrollbar(panel_th, "thType", GLUI_SCROLL_HORIZONTAL, &threshold_type, 0, OpenClose);
	scrollbar_thType->set_int_limits(0, 4, GLUI_LIMIT_CLAMP);
	scrollbar_thType->set_speed(0.001);

	glui->add_column_to_panel(panel_th, false);

	glui->add_checkbox_to_panel(panel_th, "0:Open, 1:Close", &isclose, 0, OpenClose);

	rollout_openclose->disable();
}
void corner_ui(GLUI_Panel* panel_c) {
	rollout_corner = glui->add_rollout_to_panel(panel_c, "FindCorner", false);
	GLUI_Panel* panel_th = glui->add_panel_to_panel(rollout_corner, "Threshold Settings");

	GLUI_Spinner* spinner_thVal = glui->add_spinner_to_panel(panel_th, "thVal : ", GLUI_SPINNER_INT, &threshold_val, 0, corner);
	spinner_thVal->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	spinner_thVal->set_speed(0.001);

	glui->add_column_to_panel(panel_th, false);

	GLUI_Scrollbar* scrollbar_thVal = new GLUI_Scrollbar(panel_th, "thVal", GLUI_SCROLL_HORIZONTAL, &threshold_val, 0, corner);
	scrollbar_thVal->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	scrollbar_thVal->set_speed(0.001);

	glui->add_column_to_panel(panel_th, false);

	//glui->add_button("DeleteCorner", 1, corner);

	rollout_corner->disable();
}
void labeling_ui(GLUI_Panel* panel_l) {
	rollout_label = glui->add_rollout_to_panel(panel_l, "Labeling", false);
	GLUI_Panel* panel_th = glui->add_panel_to_panel(rollout_label, "Threshold Settings");

	GLUI_Spinner* spinner_thVal = glui->add_spinner_to_panel(panel_th, "thVal : ", GLUI_SPINNER_INT, &threshold_val, 0, labeling);
	spinner_thVal->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	spinner_thVal->set_speed(0.001);

	glui->add_column_to_panel(panel_th, false);

	GLUI_Scrollbar* scrollbar_thVal = new GLUI_Scrollbar(panel_th, "thVal", GLUI_SCROLL_HORIZONTAL, &threshold_val, 0, labeling);
	scrollbar_thVal->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	scrollbar_thVal->set_speed(0.001);

	glui->add_column_to_panel(panel_th, false);
	//glui->add_button("DeleteCorner", 1, corner);
	rollout_label->disable();
}

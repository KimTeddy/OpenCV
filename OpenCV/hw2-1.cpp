#include <iostream>
#include <opencv2/opencv.hpp>
#include <GL/glui.h>
#include "OpenFileDialog.h"
#include "SaveFileDialog.h"

#define CHECHBOX_SIZE 11

#define NONE			0
#define CANNY			1
#define WRAP			2
#define DRAWING			3
#define HISTOGRAM		4
#define THRESHOLD		5
#define HOUGHLINES		6
#define HOUGHCIRCLES	7
#define COLORMAP		8
#define HISTEQ			9
#define CLAHEHISTEQ		10

using namespace cv;
using namespace std;

int main_window;
String Filename;
Mat input, dst, output, frame_cd, input_copy, gray;

GLUI* glui;
GLUI_Button* btn_save;
GLUI_Rollout* rollout_th, * rollout_canny, * panel_colormap, * rollout_draw;
GLUI_Checkbox* houghlines, * houghcircles, * clahehisteq;
GLUI_Panel* movedots;
GLUI_Translation* dots[4];
GLUI_Panel* radiobutton;
GLUI_RadioGroup* radiogroup;

GLUI_Checkbox* checkboxes[CHECHBOX_SIZE];


int v[CHECHBOX_SIZE] = { 0 };

void just_sync(int id);
void CannyThreshold(int id);


void idle();
void checkbox(int id);
void radioButtonCallback(int id);


void Canny_ui();
void Threshold_ui();

void drawCircle(int event, int x, int y, int, void* param);

int mode = NONE;
int red, green, blue, drawing = false;

int framebar;
float trans_dot_pos[4][3] = { 0.0 };
float now_dot_pos[4][3] = { 0.0 };
float prev_pos[4][3] = { 0.0 };
int iswrap = true;
int iscanny = false;
int isrmbg = false;
int ishist = false;
int isline = true;
int iscircle = false;
int iscolormap = false;
int isgray = false;
int ishisteq = false;
int isclahe = false;
bool isopen = false;
int isdrawing = false;

float set_dots[4][2] = { 0 };
int selected_point;
int colormap_type = COLORMAP_RAINBOW;


//canny
int lowTh = 40, ratioo = 3, kernel_size = 3, max_lowTh = 100;
int blur_size = 5;

void effectpack(int id);

int open() {
	destroyAllWindows();
	OpenFileDialog* openFileDialog = new OpenFileDialog();
	if (openFileDialog->ShowDialog()) {
		Filename = openFileDialog->FileName;
		return(1);
	}
	else return(-1);
}


int input_width, input_height;
void OpenImage(int id) {
	if (open() == 1) {
		namedWindow(Filename, 1);
		input = imread(Filename);
		input.copyTo(input_copy);
		imshow("Input", input);
		imshow("Output", input_copy);

		dst.create(input.size(), input.type());

		input_width = input.size().width;
		input_height = input.size().height;
		cout << "input_width: " << input_width << "input_height: " << input_height << endl;

		//모든 기능 활성화
		btn_save->enable();





		checkboxes[NONE]->enable();
		checkboxes[CANNY]->enable();

		checkboxes[THRESHOLD]->enable();
		movedots->enable();
		/*for (int i = 0; i < CHECHBOX_SIZE; i++) {
			checkboxes[i]->enable();
		}*/
		radiobutton->enable();

		////rollout_canny->enable();
		////panel_colormap->enable();

		radioButtonCallback(0);

		now_dot_pos[0][0] = 0; now_dot_pos[0][1] = 0;			now_dot_pos[2][0] = input_width; now_dot_pos[2][1] = 0;
		now_dot_pos[1][0] = 0; now_dot_pos[1][1] = input_height;	now_dot_pos[3][0] = input_width; now_dot_pos[3][1] = input_height;

		set_dots[0][0] = 0; set_dots[0][1] = 0;				set_dots[2][0] = input_width; set_dots[2][1] = 0;
		set_dots[1][0] = 0; set_dots[1][1] = -input_height;	set_dots[3][0] = input_width; set_dots[3][1] = -input_height;

		setMouseCallback(Filename, drawCircle);

		GLUI_Master.set_glutIdleFunc(NULL);
		isopen = true;
	}
}

String SaveFilename;
int Save() {//all record 추가하기
	SaveFileDialog* openFileDialog = new SaveFileDialog();
	if (openFileDialog->ShowDialog()) {
		SaveFilename = openFileDialog->FileName;
		//오류 처리 필요
		namedWindow(SaveFilename);
		input.copyTo(output);
		GLUI_Master.sync_live_all();

		imwrite(SaveFilename, input_copy);
		return 0;
	}
	return -1;
}
void SaveImage(int id) {
	Save();
}


void just_sync(int id) {
	GLUI_Master.sync_live_all();
}

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

void showHist(Mat& in, int index) {
	if (ishist) {
		Mat histogram, hist_after;
		if (in.channels() == 1) { // GrayScale 영상 경우
			int hist[256] = { 0 };
			for (int y = 0; y < in.rows; y++)
				for (int x = 0; x < in.cols; x++)
					hist[(int)in.at<uchar>(y, x)]++;
			drawHist(histogram, 'K', hist, hist, hist);
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


void drawCircle(int event, int x, int y, int, void* param) {
	if (mode == DRAWING) {
		if (event == EVENT_LBUTTONDOWN)
			drawing = true;
		else if (event == EVENT_MOUSEMOVE) {
			if (drawing == true)
				circle(input_copy, Point(x, y), 3, Scalar(blue, green, red), 10);
		}
		else if (event == EVENT_LBUTTONUP)
			drawing = false;
		imshow(Filename, input_copy);
	}
}

void CannyThreshold(Mat& frame_c) {
	if (mode == CANNY) {
		dst.create(frame_c.size(), frame_c.type());
		blur(frame_c, dst, Size(blur_size, blur_size));
		Canny(dst, frame_c, lowTh, lowTh * ratioo, kernel_size);
		dst = Scalar::all(0);
		frame_c.copyTo(dst, frame_c);

		if (isline) {
			cvtColor(frame_c, frame_cd, COLOR_GRAY2BGR); // 허프변환에 의해 검출된 선을 붉은색
			// 으로 표시하기 위해 컬러로 변환
			vector<Vec4i> lines;  // 검출된 직선의 양끝점 좌표를 저장하기 위한 버퍼
			HoughLinesP(frame_c, lines, 1, CV_PI / 180, 50, 100, 20);
			for (size_t i = 0; i < lines.size(); i++) {
				Vec4i l = lines[i];
				line(frame_cd, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, LINE_AA);
			}
			frame_cd.copyTo(frame_c);
		}
	}
	GLUI_Master.sync_live_all();
}

void Canny_ui(GLUI_Panel* panel_canny_in) {
	rollout_canny = glui->add_rollout_to_panel(panel_canny_in, "Canny");
	GLUI_Panel* panel_canny = glui->add_panel_to_panel(rollout_canny, "Canny Settings");

	GLUI_Spinner* spinner_lowTh = glui->add_spinner_to_panel(panel_canny, "LowTh : ", GLUI_SPINNER_INT, &lowTh, 0, effectpack);
	spinner_lowTh->set_int_limits(1, max_lowTh, GLUI_LIMIT_CLAMP);
	spinner_lowTh->set_speed(0.001);
	GLUI_Spinner* spinner_blursize = glui->add_spinner_to_panel(panel_canny, "BlurSize : ", GLUI_SPINNER_INT, &blur_size, 0, effectpack);
	spinner_blursize->set_int_limits(1, 10, GLUI_LIMIT_CLAMP);
	spinner_blursize->set_speed(0.001);

	glui->add_column_to_panel(panel_canny, false);

	GLUI_Scrollbar* scrollbar_lowTh = new GLUI_Scrollbar(panel_canny, "lowTh", GLUI_SCROLL_HORIZONTAL, &lowTh, 0, effectpack);
	scrollbar_lowTh->set_int_limits(1, max_lowTh, GLUI_LIMIT_CLAMP);
	scrollbar_lowTh->set_speed(0.001);
	GLUI_Scrollbar* scrollbar_blursize = new GLUI_Scrollbar(panel_canny, "BlurSize", GLUI_SCROLL_HORIZONTAL, &blur_size, 0, effectpack);
	scrollbar_blursize->set_int_limits(1, 10, GLUI_LIMIT_CLAMP);
	scrollbar_blursize->set_speed(0.001);

	rollout_canny->disable();
}

void ClaheHistequlize(Mat& frame_ch) {
	if (mode == CLAHEHISTEQ) {
		Mat clahehe;
		Ptr<CLAHE> clahe;
		clahe = createCLAHE(2.0, Size(8, 8));
		clahe->apply(frame_ch, clahehe);
		clahehe.copyTo(frame_ch);
	}
}

Mat src_ycrcb, ycrcb_planes[3];
void Histeqalize(Mat& frame_h) {
	if (mode == HISTEQ) {
		cvtColor(frame_h, src_ycrcb, COLOR_BGR2YCrCb);
		split(src_ycrcb, ycrcb_planes);

		//밝기 성분에 대해 히스토그램 평활화 수행
		if (isclahe) ClaheHistequlize(ycrcb_planes[0]);
		else equalizeHist(ycrcb_planes[0], ycrcb_planes[0]);

		merge(ycrcb_planes, 3, src_ycrcb);
		cvtColor(src_ycrcb, frame_h, COLOR_YCrCb2BGR);
	}
}



int threshold_val = 100;
int threshold_type = 0;
void Threshold(Mat& input_ch) {
	if (mode == THRESHOLD) {
		cvtColor(input_ch, gray, COLOR_BGR2GRAY);
		threshold(gray, gray, threshold_val, 255, threshold_type);
		cvtColor(gray, input_ch, COLOR_GRAY2BGR);
	}
	GLUI_Master.sync_live_all();
}

void Threshold_ui(GLUI_Panel* panel_threshold) {
	rollout_th = glui->add_rollout_to_panel(panel_threshold, "Threshold");
	GLUI_Panel* panel_th = glui->add_panel_to_panel(rollout_th, "Threshold Settings");

	GLUI_Spinner* spinner_thVal = glui->add_spinner_to_panel(panel_th, "thVal : ", GLUI_SPINNER_INT, &threshold_val, 0, effectpack);
	spinner_thVal->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	spinner_thVal->set_speed(0.001);
	GLUI_Listbox* listbox_thType = glui->add_listbox_to_panel(panel_th, "thType : ", &threshold_type, 0, effectpack);
	listbox_thType->add_item(THRESH_BINARY, "BINARY");
	listbox_thType->add_item(THRESH_BINARY_INV, "THRESH_BINARY_INV");
	listbox_thType->add_item(THRESH_TRUNC, "THRESH_TRUNC");
	listbox_thType->add_item(THRESH_TOZERO, "THRESH_TOZERO");
	listbox_thType->add_item(THRESH_TOZERO_INV, "THRESH_TOZERO_INV");
	//listbox_thType->add_item(THRESH_MASK,		"THRESH_MASK");
	//listbox_thType->add_item(THRESH_OTSU,		"THRESH_OTSU");
	//listbox_thType->add_item(THRESH_TRIANGLE,	"THRESH_TRIANGLE");

	glui->add_column_to_panel(panel_th, false);

	GLUI_Scrollbar* scrollbar_thVal = new GLUI_Scrollbar(panel_th, "thVal", GLUI_SCROLL_HORIZONTAL, &threshold_val, 0, effectpack);
	scrollbar_thVal->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	scrollbar_thVal->set_speed(0.001);
	GLUI_Scrollbar* scrollbar_thType = new GLUI_Scrollbar(panel_th, "thType", GLUI_SCROLL_HORIZONTAL, &threshold_type, 0, effectpack);
	scrollbar_thType->set_int_limits(0, 4, GLUI_LIMIT_CLAMP);
	scrollbar_thType->set_speed(0.001);
}

void Hough_circles(Mat& frame_cir) {
	if (iscircle) {
		cvtColor(frame_cir, gray, COLOR_BGR2GRAY);
		GaussianBlur(gray, gray, Size(9, 9), 2, 2);
		vector<Vec3f> circles;
		HoughCircles(gray, circles, HOUGH_GRADIENT, 1, gray.rows / 8, 200, 50);
		// 원을 영상 위에 그린다. 
		for (size_t i = 0; i < circles.size(); i++) {
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			circle(frame_cir, center, 3, Scalar(0, 255, 0), -1, 8, 0); // 원의 중심을 그린다. 
			circle(frame_cir, center, radius, Scalar(0, 0, 255), 3, 8, 0); // 원을 그린다.
		}
	}
}

//const char* checkbox_names[CHECHBOX_SIZE] = {
//	"none",
//	"CannyThreshold",
//	"Threshold",
//	"HistogramEqualize",
//	"qwer"
//};
void wrap(Mat& frame_w) {
	if (mode == WRAP) {
		Point2f inputp[4], outputp[4];
		inputp[0] = Point2f(0, 0);			inputp[2] = Point2f(frame_w.cols, 0);
		inputp[1] = Point2f(0, frame_w.rows);	inputp[3] = Point2f(frame_w.cols, frame_w.rows);

		outputp[0] = Point2f(now_dot_pos[0][0], now_dot_pos[0][1]);	outputp[2] = Point2f(now_dot_pos[2][0], now_dot_pos[2][1]);
		outputp[1] = Point2f(now_dot_pos[1][0], now_dot_pos[1][1]);	outputp[3] = Point2f(now_dot_pos[3][0], now_dot_pos[3][1]);

		Mat h = getPerspectiveTransform(inputp, outputp);
		warpPerspective(frame_w, frame_w, h, frame_w.size());
	}
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
void idle() {
	/*switch (mode) {
	case CANNY: CannyThreshold(mode);break;
	case THRESHOLD: Threshold(mode); break;
	case HISTEQ: HistogramEqualize(mode); break;
	case :  break;
	default: break;
	}*/
	effectpack(0);
	GLUI_Master.sync_live_all();
}

void checkbox(int id) {
	mode = id;
	idle();
	for (int i = 0; i < CHECHBOX_SIZE; i++) {
		if (i == id) {
			v[id] = 1;
		}
		else {
			v[i] = 0;
		}
	}

	if (CANNY) {
		rollout_canny->enable(); houghcircles->disable();
	}
	else {
		rollout_canny->disable(); houghcircles->enable();
	}

	if (HOUGHLINES) {
	}
	else {
	}

	if (HOUGHCIRCLES) {}
	else {
	}

	if (COLORMAP) { panel_colormap->enable(); }
	else {
		panel_colormap->disable();
	}

	if (WRAP) { radiobutton->enable(); dots[selected_point]->enable(); }
	else {
		radiobutton->disable();
		dots[selected_point]->disable();
	}

	if (DRAWING) { rollout_draw->enable(); }
	else { rollout_draw->disable(); }

	if (HISTOGRAM) {}
	else {
		destroyWindow("Before Histogram"); destroyWindow("After Histogram");
	}
	if (HISTEQ) { clahehisteq->enable(); }
	else { clahehisteq->disable(); }

	GLUI_Master.sync_live_all();
}

void effectpack(int id) {
	if (isopen) {
		input.copyTo(input_copy);

		showHist(input_copy, input_copy.type());
		CannyThreshold(input_copy);
		Histeqalize(input_copy);
		Threshold(input_copy);
		Hough_circles(input_copy);

		wrap(input_copy);

		imshow("Input", input);
		imshow("Output", input_copy);
	}
}

int main(int argc, char* argv[]) {
	glui = GLUI_Master.create_glui("Menu", 0);
	main_window = glui->get_glut_window_id();
	GLUI_Master.set_glutIdleFunc(NULL);
	cout << main_window << endl;

	glui->add_statictext("60191798 KYC");

	checkboxes[NONE] = glui->add_checkbox("NONE", &v[NONE], NONE, checkbox);
	checkboxes[NONE]->set_alignment(GLUI_ALIGN_CENTER);

	GLUI_Panel* panel_canny = glui->add_panel("CANNY", GLUI_PANEL_NONE);
	checkboxes[CANNY] = glui->add_checkbox_to_panel(panel_canny, "CANNY", &v[CANNY], CANNY, checkbox);
	glui->add_column_to_panel(panel_canny, false);
	Canny_ui(panel_canny);
	panel_canny->set_alignment(GLUI_ALIGN_LEFT);

	GLUI_Panel* panel_threshold = glui->add_panel("THRESHOLD", GLUI_PANEL_NONE);
	checkboxes[THRESHOLD] = glui->add_checkbox_to_panel(panel_threshold, "THRESHOLD", &v[THRESHOLD], THRESHOLD, checkbox);
	glui->add_column_to_panel(panel_threshold, false);
	Threshold_ui(panel_threshold);
	panel_threshold->set_alignment(GLUI_ALIGN_LEFT);


	checkboxes[WRAP] = glui->add_checkbox("WRAP", &v[WRAP], WRAP, checkbox);

	houghcircles = glui->add_checkbox_to_panel(checkboxes, "Hough Circles", &iscircle, HOUGHCIRCLES, checkbox_handler);

	GLUI_Panel* move = glui->add_panel("Move", GLUI_PANEL_NONE);
	movedots = glui->add_panel_to_panel(move, "Move", GLUI_PANEL_EMBOSSED);
	dots[0] = glui->add_translation_to_panel(movedots, "Left Top", GLUI_TRANSLATION_XY, trans_dot_pos[0], 0, translation);
	dots[1] = glui->add_translation_to_panel(movedots, "Left Bottom", GLUI_TRANSLATION_XY, trans_dot_pos[1], 1, translation);
	glui->add_column_to_panel(movedots, false);
	dots[2] = glui->add_translation_to_panel(movedots, "Right Top", GLUI_TRANSLATION_XY, trans_dot_pos[2], 2, translation);
	dots[3] = glui->add_translation_to_panel(movedots, "Right Bottom", GLUI_TRANSLATION_XY, trans_dot_pos[3], 3, translation);
	movedots->disable();

	glui->add_column_to_panel(move, false);

	radiobutton = glui->add_panel_to_panel(move, "Move", GLUI_PANEL_EMBOSSED);
	radiogroup = glui->add_radiogroup_to_panel(radiobutton, &selected_point, 0, radioButtonCallback);
	glui->add_radiobutton_to_group(radiogroup, "Left Top");
	glui->add_radiobutton_to_group(radiogroup, "Left Bottom");
	glui->add_column_to_panel(radiobutton, false);
	glui->add_radiobutton_to_group(radiogroup, "Right Top");
	glui->add_radiobutton_to_group(radiogroup, "Right Bottom");
	radiobutton->disable();






	glui->add_column(false);

	/*for (int i = 0; i < CHECHBOX_SIZE; i++) {
		glui->add_checkbox(checkbox_names[i], &v[i], i, checkbox);
	}*/

	glui->add_separator();
	glui->add_button("Open", 0, OpenImage);
	btn_save = glui->add_button("Save", 0, SaveImage);
	btn_save->disable();
	//new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);
	glui->add_button("Quit", 0, (GLUI_Update_CB)exit);

	//CannyThreshold(0);
	glutIdleFunc(idle);

	glui->set_main_gfx_window(main_window);
	glutMainLoop();
	return EXIT_SUCCESS;
}



//THRESH_BINARY = 0, //!< \f[\texttt{dst} (x,y) =  \fork{\texttt{maxval}}{if \(\texttt{src}(x,y) > \texttt{thresh}\)}{0}{otherwise}\f]
//THRESH_BINARY_INV = 1, //!< \f[\texttt{dst} (x,y) =  \fork{0}{if \(\texttt{src}(x,y) > \texttt{thresh}\)}{\texttt{maxval}}{otherwise}\f]
//THRESH_TRUNC = 2, //!< \f[\texttt{dst} (x,y) =  \fork{\texttt{threshold}}{if \(\texttt{src}(x,y) > \texttt{thresh}\)}{\texttt{src}(x,y)}{otherwise}\f]
//THRESH_TOZERO = 3, //!< \f[\texttt{dst} (x,y) =  \fork{\texttt{src}(x,y)}{if \(\texttt{src}(x,y) > \texttt{thresh}\)}{0}{otherwise}\f]
//THRESH_TOZERO_INV = 4, //!< \f[\texttt{dst} (x,y) =  \fork{0}{if \(\texttt{src}(x,y) > \texttt{thresh}\)}{\texttt{src}(x,y)}{otherwise}\f]
//THRESH_MASK = 7,
//THRESH_OTSU = 8, //!< flag, use Otsu algorithm to choose the optimal threshold value
//THRESH_TRIANGLE = 16 //!< flag, use Triangle algorithm to choose the optimal threshold value


//const char* c;
//string checkbox_name;
//for (int i = 0; i < CHECHBOX_SIZE; i++) {
//checkbox_name = "v[" + to_string(i) + "]\0";
//c = checkbox_name.c_str();
//glui->add_checkbox(c, &v[i], i, checkbox);
//}

/*switch (id) {
case 1: v[2] = v[3] = 0; v[1] = 1; break;
case 2: v[1] = v[3] = 0; v[2] = 1; break;
case 3: v[1] = v[2] = 0; v[3] = 1; break;
default: break;
}*/

/*
		glui->add_checkbox("v1", &v[1], 1, checkbox);
		glui->add_checkbox("v2", &v[2], 2, checkbox);
		glui->add_checkbox("v3", &v[3], 3, checkbox);
		glui->add_checkbox("v4", &v[4], 4, checkbox);*/
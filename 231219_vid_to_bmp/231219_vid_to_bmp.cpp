#include <iostream>
#include "opencv2/opencv.hpp"
#include <GL/glut.h>
#include <GL/glui.h>
#include "OpenFileDialog.h"
#include "SaveFileDialog.h"
//#include <string>

#define PLAY_BACK_FAST	- 2
#define PLAY_BACK		- 1
#define PLAY_STOP		  0
#define PLAY_FORWARD	  1
#define PLAY_FORWARD_FAST 2

using namespace cv;
using namespace std;

int main_window;
String Filename;
Mat src;
VideoCapture vid;
Mat frame, output_frame, frame_for_hist, frame_pause, frame_cd;
bool isopen = false;
int playmod = PLAY_STOP;
int vid_fps, vid_msec, vid_framecount, vid_width, vid_height;
bool ispause = true;

double settest = 0;
int nowframe = 0;

GLUI* glui;
GLUI_Panel* playbutton;
GLUI_Panel* playbar;
GLUI_Scrollbar* scrollbar_play;
GLUI_Button* btn_save;
GLUI_Panel* movedots;
GLUI_Panel* checkboxes;
GLUI_Panel* radiobutton;
GLUI_RadioGroup* radiogroup;
GLUI_Translation* dots[4];

GLUI_Rollout* rollout_canny, * panel_colormap, * rollout_draw;
GLUI_Checkbox* houghlines, * houghcircles, * clahehisteq;
GLUI_StaticText* static_playtext;
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

int isdrawing = false;

float set_dots[4][2] = { 0 };
int selected_point;
int colormap_type = COLORMAP_RAINBOW;

const char* playtext[5] = { "Play Back Fast <<" , "Play Back <" , "Pause ||" , "Play Forward >" , "Play Forward Fast >>" };

//void idle_pause();
void pause_handler(int id = -1);
void drawCircle(int event, int x, int y, int, void* param);
void radioButtonCallback(int id);

//기능들
void CannyThreshold(Mat& frame_c);
void wrap(Mat& frame_w);
void showHist(Mat& in, int index);
void remove_background(Mat& frame_b);
void Hough_circles(Mat& frame_cir);
void ClaheHistequlize(Mat& frame_ch);
void colormap(Mat& frame_cor);
void Histeqalize(Mat& frame_h);

void just_sync(int id);
void Threshold(int id);

void idle();
void checkbox(int id);

void Canny_ui();
void Threshold_ui();

//canny
int lowTh = 40, ratioo = 3, kernel_size = 3, max_lowTh = 100;
int blur_size = 3;

void just_sync(int id) {
	GLUI_Master.sync_live_all();
}

#define CANNY			1
#define WRAP			2
#define DRAWING			3
#define HISTOGRAM		4
#define RMBACKGROUND	5
#define HOUGHLINES		6
#define HOUGHCIRCLES	7
#define COLORMAP		8
#define HISTEQ			9
#define CLAHEHISTEQ		10

void effectpack(Mat& frame_e) {
	showHist(frame_e, 0);

	Histeqalize(frame_e);
	Hough_circles(frame_e);
	if (isgray) cvtColor(frame_e, frame_e, COLOR_BGR2GRAY);
	colormap(frame_e);
	CannyThreshold(frame_e);
	remove_background(frame_e);

	wrap(frame_e);
	showHist(frame_e, 1);
}

void checkbox_handler(int id) {
	switch (id) {
	case CANNY:
		if (iscanny) {
			rollout_canny->enable(); iscircle = false; houghcircles->disable();
		}
		else {
			rollout_canny->disable(); houghcircles->enable();
		} break;
	case HOUGHLINES:
		if (isline) {
		}
		else {
		} break;
	case HOUGHCIRCLES:
		if (iscircle) {}
		else {
		} break;
	case COLORMAP:
		if (iscolormap) { panel_colormap->enable(); }
		else {
			panel_colormap->disable();
		} break;
	case WRAP:
		if (iswrap) { radiobutton->enable(); dots[selected_point]->enable(); }
		else {
			radiobutton->disable();
			dots[selected_point]->disable();
		} break;
	case DRAWING:
		if (isdrawing) { rollout_draw->enable(); }
		else { rollout_draw->disable(); } break;
	case HISTOGRAM:
		if (ishist) {}
		else {
			destroyWindow("Before Histogram"); destroyWindow("After Histogram");
		} break;
	case RMBACKGROUND:
		if (isrmbg) {}
		else { destroyWindow("Background"); } break;
	case HISTEQ:
		if (ishisteq) { clahehisteq->enable(); }
		else { clahehisteq->disable(); } break;
	default: break;
	}
	GLUI_Master.sync_live_all();
};

Mat src_ycrcb, ycrcb_planes[3];
void Histeqalize(Mat& frame_h) {
	if (ishisteq) {
		cvtColor(frame_h, src_ycrcb, COLOR_BGR2YCrCb);
		split(src_ycrcb, ycrcb_planes);

		//밝기 성분에 대해 히스토그램 평활화 수행
		if (isclahe) ClaheHistequlize(ycrcb_planes[0]);
		else equalizeHist(ycrcb_planes[0], ycrcb_planes[0]);

		merge(ycrcb_planes, 3, src_ycrcb);
		cvtColor(src_ycrcb, frame_h, COLOR_YCrCb2BGR);
	}
}

void ClaheHistequlize(Mat& frame_ch) {
	if (isclahe) {
		Mat clahehe;
		Ptr<CLAHE> clahe;
		clahe = createCLAHE(2.0, Size(8, 8));
		clahe->apply(frame_ch, clahehe);
		clahehe.copyTo(frame_ch);
	}
}


Mat result; // MOG2에 의한 forground mask 영상
Mat BGImg; // MOG2에 의한 background 영상//MOG2 배경 삭제 객체
Ptr<BackgroundSubtractor> pMOG2 = createBackgroundSubtractorMOG2();
void remove_background(Mat& frame_b) {
	if (isrmbg) {
		Mat out;
		pMOG2->apply(frame_b, result, -1);// -1 : 자동갱신비율, 0 : 갱신안함, 1 : 이전영상
		pMOG2->getBackgroundImage(BGImg);
		frame_b.copyTo(out, result);
		out.copyTo(frame_b);
		imshow("Background", BGImg);
	}
}

void drawHist(Mat& out, char mode, int histB[], int histG[], int histR[])
{
	int hist_h = vid_height / 4;  // 히스토그램 영상의 높이
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
};

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

Mat dst;
void CannyThreshold(Mat& frame_c) {
	if (iscanny) {
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

void Canny_ui() {
	rollout_canny = glui->add_rollout("Canny");
	GLUI_Panel* panel_canny = glui->add_panel_to_panel(rollout_canny, "Canny Settings");

	GLUI_Spinner* spinner_lowTh = glui->add_spinner_to_panel(panel_canny, "LowTh : ", GLUI_SPINNER_INT, &lowTh, 0, just_sync);
	spinner_lowTh->set_int_limits(1, max_lowTh, GLUI_LIMIT_CLAMP);
	spinner_lowTh->set_speed(0.001);
	GLUI_Spinner* spinner_blursize = glui->add_spinner_to_panel(panel_canny, "BlurSize : ", GLUI_SPINNER_INT, &blur_size, 0, just_sync);
	spinner_blursize->set_int_limits(1, 10, GLUI_LIMIT_CLAMP);
	spinner_blursize->set_speed(0.001);

	glui->add_column_to_panel(panel_canny, false);

	GLUI_Scrollbar* scrollbar_lowTh = new GLUI_Scrollbar(panel_canny, "lowTh", GLUI_SCROLL_HORIZONTAL, &lowTh, 0, just_sync);
	scrollbar_lowTh->set_int_limits(1, max_lowTh, GLUI_LIMIT_CLAMP);
	scrollbar_lowTh->set_speed(0.001);
	GLUI_Scrollbar* scrollbar_blursize = new GLUI_Scrollbar(panel_canny, "BlurSize", GLUI_SCROLL_HORIZONTAL, &blur_size, 0, just_sync);
	scrollbar_blursize->set_int_limits(1, 10, GLUI_LIMIT_CLAMP);
	scrollbar_blursize->set_speed(0.001);

	rollout_canny->disable();
}

Mat gray;
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

void colormap(Mat& frame_cor) {
	if (iscolormap) applyColorMap(frame_cor, frame_cor, colormap_type);
}
/*
	COLORMAP_AUTUMN = 0, //!< ![autumn](pics/colormaps/colorscale_autumn.jpg)
	COLORMAP_BONE = 1, //!< ![bone](pics/colormaps/colorscale_bone.jpg)
	COLORMAP_JET = 2, //!< ![jet](pics/colormaps/colorscale_jet.jpg)
	COLORMAP_WINTER = 3, //!< ![winter](pics/colormaps/colorscale_winter.jpg)
	COLORMAP_RAINBOW = 4, //!< ![rainbow](pics/colormaps/colorscale_rainbow.jpg)

	COLORMAP_OCEAN = 5, //!< ![ocean](pics/colormaps/colorscale_ocean.jpg)
	COLORMAP_SUMMER = 6, //!< ![summer](pics/colormaps/colorscale_summer.jpg)
	COLORMAP_SPRING = 7, //!< ![spring](pics/colormaps/colorscale_spring.jpg)
	COLORMAP_COOL = 8, //!< ![cool](pics/colormaps/colorscale_cool.jpg)
	COLORMAP_HSV = 9, //!< ![HSV](pics/colormaps/colorscale_hsv.jpg)

	COLORMAP_PINK = 10, //!< ![pink](pics/colormaps/colorscale_pink.jpg)
	COLORMAP_HOT = 11, //!< ![hot](pics/colormaps/colorscale_hot.jpg)
	COLORMAP_PARULA = 12, //!< ![parula](pics/colormaps/colorscale_parula.jpg)
	COLORMAP_MAGMA = 13, //!< ![magma](pics/colormaps/colorscale_magma.jpg)
	COLORMAP_INFERNO = 14, //!< ![inferno](pics/colormaps/colorscale_inferno.jpg)

	COLORMAP_PLASMA = 15, //!< ![plasma](pics/colormaps/colorscale_plasma.jpg)
	COLORMAP_VIRIDIS = 16, //!< ![viridis](pics/colormaps/colorscale_viridis.jpg)
	COLORMAP_CIVIDIS = 17, //!< ![cividis](pics/colormaps/colorscale_cividis.jpg)
	COLORMAP_TWILIGHT = 18, //!< ![twilight](pics/colormaps/colorscale_twilight.jpg)
	COLORMAP_TWILIGHT_SHIFTED = 19, //!< ![twilight shifted](pics/colormaps/colorscale_twilight_shifted.jpg)

	COLORMAP_TURBO = 20, //!< ![turbo](pics/colormaps/colorscale_turbo.jpg)
	COLORMAP_DEEPGREEN = 21  //!< ![deepgreen](pics/colormaps/colorscale_deepgreen.jpg)
*/
GLUI_Listbox* listbox_clrType;
void colormap_ui() {
	panel_colormap = glui->add_rollout("Colormap");
	listbox_clrType = glui->add_listbox_to_panel(panel_colormap, "ColorType : ", &colormap_type, 0, just_sync);
	listbox_clrType->add_item(COLORMAP_AUTUMN, "COLORMAP_AUTUMN");
	listbox_clrType->add_item(COLORMAP_BONE, "COLORMAP_BONE");
	listbox_clrType->add_item(COLORMAP_JET, "COLORMAP_JET");
	listbox_clrType->add_item(COLORMAP_WINTER, "COLORMAP_WINTER");
	listbox_clrType->add_item(COLORMAP_RAINBOW, "COLORMAP_RAINBOW");

	listbox_clrType->add_item(COLORMAP_OCEAN, "COLORMAP_OCEAN");
	listbox_clrType->add_item(COLORMAP_SUMMER, "COLORMAP_SUMMER");
	listbox_clrType->add_item(COLORMAP_SPRING, "COLORMAP_SPRING");
	listbox_clrType->add_item(COLORMAP_COOL, "COLORMAP_COOL");
	listbox_clrType->add_item(COLORMAP_HSV, "COLORMAP_HSV");

	listbox_clrType->add_item(COLORMAP_PINK, "COLORMAP_PINK");
	listbox_clrType->add_item(COLORMAP_HOT, "COLORMAP_HOT");
	listbox_clrType->add_item(COLORMAP_PARULA, "COLORMAP_PARULA");
	listbox_clrType->add_item(COLORMAP_MAGMA, "COLORMAP_MAGMA");
	listbox_clrType->add_item(COLORMAP_INFERNO, "COLORMAP_INFERNO");

	listbox_clrType->add_item(COLORMAP_PLASMA, "COLORMAP_PLASMA");
	listbox_clrType->add_item(COLORMAP_VIRIDIS, "COLORMAP_VIRIDIS");
	listbox_clrType->add_item(COLORMAP_CIVIDIS, "COLORMAP_CIVIDIS");
	listbox_clrType->add_item(COLORMAP_TWILIGHT, "COLORMAP_TWILIGHT");
	listbox_clrType->add_item(COLORMAP_TWILIGHT_SHIFTED, "COLORMAP_TWILIGHT_SHIFTED");

	listbox_clrType->add_item(COLORMAP_TURBO, "COLORMAP_TURBO");
	listbox_clrType->add_item(COLORMAP_DEEPGREEN, "COLORMAP_DEEPGREEN");

	glui->add_column_to_panel(panel_colormap, false);

	/*GLUI_Scrollbar* scrollbar_clrType = new GLUI_Scrollbar(panel_colormap, "ColorType", GLUI_SCROLL_HORIZONTAL, &colormap_type, 0, just_sync);
	scrollbar_clrType->set_int_limits(0, 21, GLUI_LIMIT_CLAMP);
	scrollbar_clrType->set_speed(0.001);*/
	panel_colormap->disable();
}

int open() {
	destroyAllWindows();
	BGImg = Scalar::all(0);
	OpenFileDialog* openFileDialog = new OpenFileDialog();
	if (openFileDialog->ShowDialog()) {
		Filename = openFileDialog->FileName;
		return(1);
	}
	else return(-1);
}

String SaveFilename;
int Save() {//all record 추가하기
	SaveFileDialog* openFileDialog = new SaveFileDialog();
	if (openFileDialog->ShowDialog()) {
		SaveFilename = openFileDialog->FileName;
		//vid.set(CAP_PROP_POS_AVI_RATIO, 0);
		//VideoWriter output(SaveFilename, VideoWriter::fourcc('m', 'p', '4', 'v'), vid_fps, Size(vid_width, vid_height));//, iscolor
		/*if (!output.isOpened())
		{
			std::cout << "Can't write video." << std::endl;
			return -1;
		}*/
		namedWindow(SaveFilename);
		int num = 0;
		char text[50] = {0};
		while (1) {
			vid >> output_frame;
			//remove_background(output_frame);
			framebar = nowframe = vid.get(CAP_PROP_POS_FRAMES);
			vid.set(CAP_PROP_POS_FRAMES, nowframe + 9);
			GLUI_Master.sync_live_all();



			if (output_frame.empty()) {
				cout << "Video END" << std::endl;
				break;
			}

			//영상처리
			effectpack(output_frame);
			char* c = &*SaveFilename.begin();
			//sprintf_s(text,"%s_%04d.bmp", c, num);
			sprintf_s(text, "%05d.bmp", num);
			num++;
			imwrite(text, output_frame);
			//output << output_frame;
			imshow(SaveFilename, output_frame);
			if (waitKey(1) == 27) {//1000 / vid_fps
				cout << "Stop video record" << endl;
				break;
			}
		}
		destroyWindow(SaveFilename);
		return 0;
	}
	return -1;
}
void SaveVideo(int id) {
	Save();
}

VideoCapture cap;
void OpenVideo(int id) {
	if (open() == 1) {
		cap = VideoCapture(Filename);
		vid = cap;
		namedWindow(Filename, 1);
		vid >> frame;
		imshow(Filename, frame);

		vid_fps = vid.get(CAP_PROP_FPS);
		vid_msec = 1.0 / (double)vid_fps * 1000;
		vid_framecount = vid.get(CAP_PROP_FRAME_COUNT);
		vid_width = vid.get(CAP_PROP_FRAME_WIDTH);
		vid_height = vid.get(CAP_PROP_FRAME_HEIGHT);
		cout << "vid_framecount: " << vid_framecount << "vid_fps: " << vid_fps << " vid_msec: " << vid_msec << endl;

		playbutton->enable();//모든 기능 활성화
		playbar->enable();
		btn_save->enable();
		movedots->enable();
		checkboxes->enable();
		radiobutton->enable();

		//rollout_canny->enable();
		//panel_colormap->enable();

		radioButtonCallback(0);

		scrollbar_play->set_int_limits(1, vid_framecount - 1, GLUI_LIMIT_CLAMP);

		now_dot_pos[0][0] = 0; now_dot_pos[0][1] = 0;			now_dot_pos[2][0] = vid_width; now_dot_pos[2][1] = 0;
		now_dot_pos[1][0] = 0; now_dot_pos[1][1] = vid_height;	now_dot_pos[3][0] = vid_width; now_dot_pos[3][1] = vid_height;

		set_dots[0][0] = 0; set_dots[0][1] = 0;				set_dots[2][0] = vid_width; set_dots[2][1] = 0;
		set_dots[1][0] = 0; set_dots[1][1] = -vid_height;	set_dots[3][0] = vid_width; set_dots[3][1] = -vid_height;

		setMouseCallback(Filename, drawCircle);

		GLUI_Master.set_glutIdleFunc(NULL);
		isopen = true;
	}
}

//다 되면 현재 재생 상태 화면에 띄워보기
void idle() {
	if (isopen) {
		nowframe = vid.get(CAP_PROP_POS_FRAMES);
		cout << nowframe << "\t";
		switch (playmod) {
		case PLAY_BACK_FAST:	vid.set(CAP_PROP_POS_FRAMES, nowframe - 4 - 1);//-4씩
			break;
		case PLAY_BACK:			vid.set(CAP_PROP_POS_FRAMES, nowframe - 1 - 1);//-1씩
			break;
		case PLAY_FORWARD:		vid.set(CAP_PROP_POS_FRAMES, nowframe);//+1씩
			break;
		case PLAY_FORWARD_FAST: vid.set(CAP_PROP_POS_FRAMES, nowframe + 3);//+4씩
			break;

		default: break;
		}

		nowframe = vid.get(CAP_PROP_POS_FRAMES);
		switch (playmod) {
		case PLAY_BACK_FAST:
		case PLAY_BACK:
			if (nowframe <= 0) {
				vid.set(CAP_PROP_POS_FRAMES, 1);
				nowframe = vid.get(CAP_PROP_POS_FRAMES);
				cout << "처음" << nowframe << endl;

				static_playtext->set_text(playtext[2]);

				pause_handler(1);
				break;
			}
		case PLAY_FORWARD:
		case PLAY_FORWARD_FAST:
			if (nowframe >= vid_framecount) {//vid_framecount을 초과한 경우
				vid.set(CAP_PROP_POS_FRAMES, vid_framecount);
				nowframe = vid.get(CAP_PROP_POS_FRAMES);
				cout << "마지막(1) " << nowframe << endl;

				static_playtext->set_text(playtext[2]);

				pause_handler(1);
				break;
			}

			if (frame.empty()) {
				vid.set(CAP_PROP_POS_FRAMES, vid_framecount);
				nowframe = vid.get(CAP_PROP_POS_FRAMES);
				cout << "마지막(2) " << nowframe << endl;

				static_playtext->set_text(playtext[2]);

				pause_handler(1);
				break;
			}

			if (nowframe >= 0 && !frame.empty()) {//정상 재생중
				vid >> frame;

				effectpack(frame);

				imshow(Filename, frame);
				framebar = nowframe = vid.get(CAP_PROP_POS_FRAMES);
				cout << "정상 재생중" << nowframe << endl;
				if (nowframe >= vid_framecount) {
					vid.set(CAP_PROP_POS_FRAMES, vid_framecount);
					nowframe = vid.get(CAP_PROP_POS_FRAMES);
					cout << "마지막(3) " << nowframe << endl;

					static_playtext->set_text(playtext[2]);

					pause_handler(1);
					break;
				}
				waitKey(vid_msec);
			}

			break;
		default: break;
		}
	}
	GLUI_Master.sync_live_all();
}
bool transing = false;
//void idle_pause() {
//
//	if (transing)
//	{
//		transing = false;
//		frame_pause.copyTo(frame);
//		//CannyThreshold(frame);
//		//showHist(frame, 1);
//		//wrap();
//		//imshow(Filename, frame);
//		//GLUI_Master.sync_live_all();
//	}
//}

void wrap(Mat& frame_w) {
	if (iswrap) {
		Point2f inputp[4], outputp[4];
		inputp[0] = Point2f(0, 0);			inputp[2] = Point2f(frame_w.cols, 0);
		inputp[1] = Point2f(0, frame_w.rows);	inputp[3] = Point2f(frame_w.cols, frame_w.rows);

		outputp[0] = Point2f(now_dot_pos[0][0], now_dot_pos[0][1]);	outputp[2] = Point2f(now_dot_pos[2][0], now_dot_pos[2][1]);
		outputp[1] = Point2f(now_dot_pos[1][0], now_dot_pos[1][1]);	outputp[3] = Point2f(now_dot_pos[3][0], now_dot_pos[3][1]);

		Mat h = getPerspectiveTransform(inputp, outputp);
		warpPerspective(frame_w, frame_w, h, frame_w.size());
	}
}

void Changemod(int id) {
	static_playtext->set_text(playtext[id + 2]);
	GLUI_Master.sync_live_all();
	switch (id) {
	case PLAY_BACK_FAST:
		playmod = PLAY_BACK_FAST;
		pause_handler(0);
		break;
	case PLAY_BACK:
		playmod = PLAY_BACK;
		pause_handler(0);
		break;
	case PLAY_STOP:
		pause_handler();
		break;
	case PLAY_FORWARD:
		playmod = PLAY_FORWARD;
		pause_handler(0);
		break;
	case PLAY_FORWARD_FAST:
		playmod = PLAY_FORWARD_FAST;
		pause_handler(0);
		break;
	}
}

void pause_handler(int id) {
	if (ispause == true) {//ispause = true 였다면 toggle
		ispause = false;
		GLUI_Master.set_glutIdleFunc(idle);
		static_playtext->set_text(playtext[playmod + 2]);
		GLUI_Master.sync_live_all();
	}
	else {//ispause = false 였다면 toggle
		ispause = true;
		frame.copyTo(frame_pause);
		GLUI_Master.set_glutIdleFunc(NULL);
	}
	if (id == 0) {
		ispause = false;
		GLUI_Master.set_glutIdleFunc(idle);
	}
	else if (id == 1) {
		ispause = true;
		frame.copyTo(frame_pause);
		GLUI_Master.set_glutIdleFunc(NULL);
	}
	GLUI_Master.sync_live_all();
}

void frame_handler(int id) {
	vid.set(CAP_PROP_POS_FRAMES, framebar);
	vid >> frame;
	imshow(Filename, frame);
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
	transing = true;
}

int red, green, blue, drawing = false;
void drawCircle(int event, int x, int y, int, void* param) {
	if (isdrawing) {
		if (event == EVENT_LBUTTONDOWN)
			drawing = true;
		else if (event == EVENT_MOUSEMOVE) {
			if (drawing == true)
				circle(frame, Point(x, y), 3, Scalar(blue, green, red), 10);
		}
		else if (event == EVENT_LBUTTONUP)
			drawing = false;
		imshow(Filename, frame);
	}
}

void draw_ui() {
	rollout_draw = glui->add_rollout("Draw Color", false);
	GLUI_Panel* panel_drawr = glui->add_panel_to_panel(rollout_draw, "Draw Color", GLUI_PANEL_NONE);
	GLUI_Panel* panel_drawg = glui->add_panel_to_panel(rollout_draw, "Draw Color", GLUI_PANEL_NONE);
	GLUI_Panel* panel_drawb = glui->add_panel_to_panel(rollout_draw, "Draw Color", GLUI_PANEL_NONE);

	GLUI_Spinner* spinner_r = glui->add_spinner_to_panel(panel_drawr, "R : ", GLUI_SPINNER_INT, &red, 0, just_sync);
	spinner_r->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	spinner_r->set_speed(0.001);
	glui->add_column_to_panel(panel_drawr, false);
	GLUI_Scrollbar* scrollbar_r = new GLUI_Scrollbar(panel_drawr, "R", GLUI_SCROLL_HORIZONTAL, &red, 0, just_sync);
	scrollbar_r->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	scrollbar_r->set_speed(0.001);


	GLUI_Spinner* spinner_g = glui->add_spinner_to_panel(panel_drawg, "G : ", GLUI_SPINNER_INT, &green, 0, just_sync);
	spinner_g->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	spinner_g->set_speed(0.001);
	glui->add_column_to_panel(panel_drawg, false);
	GLUI_Scrollbar* scrollbar_g = new GLUI_Scrollbar(panel_drawg, "G", GLUI_SCROLL_HORIZONTAL, &green, 0, just_sync);
	scrollbar_g->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	scrollbar_g->set_speed(0.001);

	GLUI_Spinner* spinner_b = glui->add_spinner_to_panel(panel_drawb, "B : ", GLUI_SPINNER_INT, &blue, 0, just_sync);
	spinner_b->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	spinner_b->set_speed(0.001);
	glui->add_column_to_panel(panel_drawb, false);
	GLUI_Scrollbar* scrollbar_b = new GLUI_Scrollbar(panel_drawb, "B", GLUI_SCROLL_HORIZONTAL, &blue, 0, just_sync);
	scrollbar_b->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	scrollbar_b->set_speed(0.001);

	rollout_draw->disable();
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


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glui = GLUI_Master.create_glui("Menu", 0);
	main_window = glui->get_glut_window_id();
	GLUI_Master.set_glutIdleFunc(NULL);

	playbar = glui->add_panel("Play Scrollbar", GLUI_PANEL_EMBOSSED);
	scrollbar_play = new GLUI_Scrollbar(playbar, "play", GLUI_SCROLL_HORIZONTAL, &framebar, -1, frame_handler);
	scrollbar_play->set_speed(0.01);
	scrollbar_play->set_w(500);
	playbar->disable();

	GLUI_Button* btn[5];
	playbutton = glui->add_panel("Play Controller", GLUI_PANEL_EMBOSSED);
	btn[0] = glui->add_button_to_panel(playbutton, "<<", PLAY_BACK_FAST, Changemod);
	btn[0]->set_w(30);
	glui->add_column_to_panel(playbutton, false);
	btn[1] = glui->add_button_to_panel(playbutton, "<", PLAY_BACK, Changemod);
	btn[1]->set_w(30);
	glui->add_column_to_panel(playbutton, false);
	btn[2] = glui->add_button_to_panel(playbutton, "||", PLAY_STOP, Changemod);
	btn[2]->set_w(30);
	glui->add_column_to_panel(playbutton, false);
	btn[3] = glui->add_button_to_panel(playbutton, ">", PLAY_FORWARD, Changemod);
	btn[3]->set_w(30);
	glui->add_column_to_panel(playbutton, false);
	btn[4] = glui->add_button_to_panel(playbutton, ">>", PLAY_FORWARD_FAST, Changemod);
	btn[4]->set_w(30);
	playbutton->disable();

	GLUI_Panel* panel_playtext = glui->add_panel("Text", GLUI_PANEL_NONE);
	static_playtext = glui->add_statictext_to_panel(panel_playtext, playtext[playmod + 2]);

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

	checkboxes = glui->add_panel("Settings", GLUI_PANEL_EMBOSSED);
	glui->add_checkbox_to_panel(checkboxes, "Wrap", &iswrap, WRAP, checkbox_handler);
	glui->add_checkbox_to_panel(checkboxes, "Draw", &isdrawing, DRAWING, checkbox_handler);
	glui->add_checkbox_to_panel(checkboxes, "Colormap", &iscolormap, COLORMAP, checkbox_handler);
	glui->add_checkbox_to_panel(checkboxes, "Histogram Equalize", &ishisteq, HISTEQ, checkbox_handler);
	clahehisteq = glui->add_checkbox_to_panel(checkboxes, "Clahe Histogram Equalize", &isclahe, CLAHEHISTEQ, checkbox_handler);
	glui->add_checkbox_to_panel(checkboxes, "Canny Edge Detection", &iscanny, CANNY, checkbox_handler);
	houghlines = glui->add_checkbox_to_panel(checkboxes, "Hough Lines", &isline, HOUGHLINES, checkbox_handler);
	houghcircles = glui->add_checkbox_to_panel(checkboxes, "Hough Circles", &iscircle, HOUGHCIRCLES, checkbox_handler);
	glui->add_checkbox_to_panel(checkboxes, "Histogram", &ishist, HISTOGRAM, checkbox_handler);
	glui->add_checkbox_to_panel(checkboxes, "Remove Background", &isrmbg, RMBACKGROUND, checkbox_handler);
	checkboxes->disable();

	draw_ui();
	colormap_ui();
	Canny_ui();

	glui->add_button("Open", 0, OpenVideo);
	btn_save = glui->add_button("Save", 0, SaveVideo);
	btn_save->disable();
	glui->add_button("Quit", 0, (GLUI_Update_CB)exit);

	glui->set_main_gfx_window(main_window);
	glutMainLoop();
	return EXIT_SUCCESS;
}

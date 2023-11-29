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
Mat frame, output_frame;
bool isopen = false;
int playmod = PLAY_STOP;
int vid_fps, vid_msec, vid_framecount, vid_width, vid_height;
bool ispause = true;

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
int framebar;
float trans_dot_pos[4][3] = { 0.0 };
float now_dot_pos[4][3] = { 0.0 };
float prev_pos[4][3] = { 0.0 };
int iswrap = true;
int isdrawing = false;
float set_dots[4][2] = { 0 };
int selected_point;

void wrap();
void pause_handler(int id = -1);
void drawCircle(int event, int x, int y, int, void* param);
void radioButtonCallback(int id);





void just_sync(int id);
void CannyThreshold(int id);
void HistogramEqualize(int id);
void Threshold(int id);

void idle();
void checkbox(int id);

void Canny_ui();
void Threshold_ui();
int mode = 0;

//canny
int lowTh = 1, ratioo = 3, kernel_size = 3, max_lowTh = 100;
int blur_size = 3;

void just_sync(int id) {
	glui->sync_live();
}
#define CANNY 0
#define THRESHOLD 1
#define HISTEQ 2

void CannyThreshold(int id) {
	if (mode == CANNY) {
		blur(output_frame, output_frame, Size(blur_size, blur_size));
		Canny(output_frame, output_frame, lowTh, lowTh * ratioo, kernel_size);
	}
	glui->sync_live();
}
void Canny_ui() {
	GLUI_EditText* edittext_lowTh, * edittext_blur_size;
	GLUI_Rollout* rollout_canny = glui->add_rollout("Canny");
	GLUI_Panel* panel_canny = glui->add_panel_to_panel(rollout_canny, "Canny Settings");

	edittext_lowTh = new GLUI_EditText(panel_canny, "LowTh : ", &lowTh, 0, CannyThreshold);
	edittext_blur_size = new GLUI_EditText(panel_canny, "BlurSize : ", &blur_size, 0, CannyThreshold);

	GLUI_Spinner* spinner_lowTh = glui->add_spinner_to_panel(panel_canny, "LowTh : ", GLUI_SPINNER_INT, &lowTh, 0, CannyThreshold);
	spinner_lowTh->set_int_limits(0, max_lowTh, GLUI_LIMIT_CLAMP);
	spinner_lowTh->set_speed(0.001);
	GLUI_Spinner* spinner_blursize = glui->add_spinner_to_panel(panel_canny, "BlurSize : ", GLUI_SPINNER_INT, &blur_size, 0, CannyThreshold);
	spinner_blursize->set_int_limits(1, 10, GLUI_LIMIT_CLAMP);
	spinner_blursize->set_speed(0.001);

	glui->add_column_to_panel(panel_canny, false);

	GLUI_Scrollbar* scrollbar_lowTh = new GLUI_Scrollbar(panel_canny, "lowTh", GLUI_SCROLL_HORIZONTAL, &lowTh, 0, CannyThreshold);
	scrollbar_lowTh->set_int_limits(0, max_lowTh, GLUI_LIMIT_CLAMP);
	scrollbar_lowTh->set_speed(0.001);
	GLUI_Scrollbar* scrollbar_blursize = new GLUI_Scrollbar(panel_canny, "BlurSize", GLUI_SCROLL_HORIZONTAL, &blur_size, 0, CannyThreshold);
	scrollbar_blursize->set_int_limits(1, 10, GLUI_LIMIT_CLAMP);
	scrollbar_blursize->set_speed(0.001);
}










int open() {
	OpenFileDialog* openFileDialog = new OpenFileDialog();
	if (openFileDialog->ShowDialog()) {
		Filename = openFileDialog->FileName;
		return(1);
	}
	else return(-1);
}
String SaveFilename;
int Save() {
	SaveFileDialog* openFileDialog = new SaveFileDialog();
	if (openFileDialog->ShowDialog()) {
		SaveFilename = openFileDialog->FileName;
		vid.set(CAP_PROP_POS_AVI_RATIO, 0);
		VideoWriter output(SaveFilename, VideoWriter::fourcc('M', 'J', 'P', 'G'), vid_fps, Size(vid_width, vid_height));//, iscolor
		if (!output.isOpened())
		{
			std::cout << "Can't write video." << std::endl;
			return -1;
		}
		namedWindow(SaveFilename);
		while (1) {
			vid >> output_frame;
			if (output_frame.empty()) {
				cout << "Video END" << std::endl;
				break;
			}

			//영상처리
			//CannyThreshold(0);







			output << output_frame;
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
		cout << "vid_framecount: " << vid_framecount << "vid_fps: " << vid_fps <<" vid_msec: " << vid_msec << endl;

		playbutton->enable();
		playbar->enable();
		btn_save->enable();
		movedots->enable();
		checkboxes->enable();
		radiobutton->enable();
		radioButtonCallback(0);
		scrollbar_play->set_int_limits(1, vid_framecount-1, GLUI_LIMIT_CLAMP);

		now_dot_pos[0][0]=0; now_dot_pos[0][1]=0;			now_dot_pos[2][0]= vid_width; now_dot_pos[2][1]=0;
		now_dot_pos[1][0]=0; now_dot_pos[1][1]= vid_height;	now_dot_pos[3][0]= vid_width; now_dot_pos[3][1]= vid_height;

		set_dots[0][0] = 0; set_dots[0][1] = 0;				set_dots[2][0] = vid_width; set_dots[2][1] = 0;
		set_dots[1][0] = 0; set_dots[1][1] = -vid_height;	set_dots[3][0] = vid_width; set_dots[3][1] = -vid_height;
		
		setMouseCallback(Filename, drawCircle);
		//prev_pos[0][0] = 0;  prev_pos[0][1] = 0;			prev_pos[2][0] = vid_width;   prev_pos[2][1] = 0;
		//prev_pos[1][0] = 0;  prev_pos[1][1] = vid_height;	prev_pos[3][0] = vid_width;   prev_pos[3][1] = vid_height;
		isopen = true;
	}
}

//void SaveVideo(int id) {
//	//VideoWriter capout;
//	//VideoCapture cap;
//	//save();
//}

double settest = 0;
int nowframe = 0;
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
				pause_handler(1);
				break;
			}
		case PLAY_FORWARD:
		case PLAY_FORWARD_FAST:
			if (nowframe >= vid_framecount) {//vid_framecount을 초과한 경우
				vid.set(CAP_PROP_POS_FRAMES, vid_framecount);
				nowframe = vid.get(CAP_PROP_POS_FRAMES);
				cout << "마지막(1) " << nowframe << endl;
				pause_handler(1);
				break;
			}

			if (frame.empty()) {
				vid.set(CAP_PROP_POS_FRAMES, vid_framecount);
				nowframe = vid.get(CAP_PROP_POS_FRAMES);
				cout << "마지막(2) " << nowframe << endl;
				pause_handler(1);
				break;
			}

			if (nowframe >= 0 && !frame.empty()) {//정상 재생중
				vid >> frame;

				wrap();

				imshow(Filename, frame);

				framebar = nowframe = vid.get(CAP_PROP_POS_FRAMES);
				cout << "정상 재생중" << nowframe << endl;
				if (nowframe >= vid_framecount) {
					vid.set(CAP_PROP_POS_FRAMES, vid_framecount);
					nowframe = vid.get(CAP_PROP_POS_FRAMES);
					cout << "마지막(3) " << nowframe << endl;
					pause_handler(1);
					break;
				}
				waitKey(vid_msec);
			}

			break;
		default: break;
		}
	}
	glui->sync_live();
}
void wrap() {
	if (iswrap) {
		Point2f inputp[4], outputp[4];
		inputp[0] = Point2f(0, 0);			inputp[2] = Point2f(frame.cols, 0);
		inputp[1] = Point2f(0, frame.rows);	inputp[3] = Point2f(frame.cols, frame.rows);

		outputp[0] = Point2f(now_dot_pos[0][0], now_dot_pos[0][1]);	outputp[2] = Point2f(now_dot_pos[2][0], now_dot_pos[2][1]);
		outputp[1] = Point2f(now_dot_pos[1][0], now_dot_pos[1][1]);	outputp[3] = Point2f(now_dot_pos[3][0], now_dot_pos[3][1]);

		Mat h = getPerspectiveTransform(inputp, outputp);
		warpPerspective(frame, frame, h, frame.size());
	}
}
void Changemod(int id) {
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
		//playmod = PLAY_STOP;
		//settest = vid.get(CAP_PROP_POS_FRAMES);
		//cout << settest << endl;
		break;
	case PLAY_FORWARD:
		playmod = PLAY_FORWARD;
		pause_handler(0);
		//settest = vid.get(CAP_PROP_POS_FRAMES);
		//cout << settest << endl;
		break;
	case PLAY_FORWARD_FAST:
		playmod = PLAY_FORWARD_FAST;
		pause_handler(0);
		//vid.set(CAP_PROP_POS_FRAMES, 100);
		break;
	}
	//cout << playmod << endl;
}

void pause_handler(int id) {
	if (ispause == true) {//ispause = true 였다면 toggle
		ispause = false;
		GLUI_Master.set_glutIdleFunc(idle);
	}
	else {//ispause = false 였다면 toggle
		ispause = true;
		GLUI_Master.set_glutIdleFunc(NULL);
	}
	if (id == 0) {
		ispause = false;
		GLUI_Master.set_glutIdleFunc(idle);
	}
	else if (id == 1) {
		ispause = true;
		GLUI_Master.set_glutIdleFunc(NULL);
	}
}
//수업시간 추가
//void idlef() {
//	static int i = 0;
//}
//void btnfunc(int id) {
//	if(id==0)
//		GLUI_Master.set_glutIdleFunc(idlef);
//	else
//		GLUI_Master.set_glutIdleFunc(NULL);
//}

void frame_handler(int id) {
	vid.set(CAP_PROP_POS_FRAMES, framebar);
	vid >> frame;
	imshow(Filename, frame);
}


void translation_handler(int index) {
	//x=현재x + 이동값x, y=현재y-이동값y
	float x = now_dot_pos[index][0] + trans_dot_pos[index][0];
	float y = now_dot_pos[index][1] - trans_dot_pos[index][1];

	float dx = x - prev_pos[index][0];//x이동 거리 계산
	//now_dot_pos[index][0] += dx;//x이동 거리 적용
	prev_pos[index][0] = x;//이전 좌표 저장

	float dy = y - prev_pos[index][1];//x이동 거리 계산
	//now_dot_pos[index][1] += dy;//x이동 거리 적용
	prev_pos[index][1] = y;//이전 좌표 저장

}

void translation(int id) {



	//	switch (id) {
	//case 0:
	//	translation_handler(0);
	//	break;
	//case 1:
	//	translation_handler(1);
	//	break;
	//case 2:
	//	translation_handler(2);
	//	break;
	//case 3:
	//	translation_handler(3);
	//	break;
	//}
	//switch (id) {
	//case 0:
	//	now_dot_pos[0][0] = now_dot_pos[0][0] + trans_dot_pos[0][0];
	//	now_dot_pos[0][1] = now_dot_pos[0][1] - trans_dot_pos[0][1];
	//	cout << trans_dot_pos[0][0] << ", " << trans_dot_pos[0][1] << endl;
	//	break;
	//case 1:
	//	now_dot_pos[1][0] = now_dot_pos[1][0] + trans_dot_pos[1][0];
	//	now_dot_pos[1][1] = now_dot_pos[1][1] - trans_dot_pos[1][1];
	//	cout << trans_dot_pos[1][0] << ", " << trans_dot_pos[1][1] << endl;
	//	break;
	//case 2:
	//	now_dot_pos[2][0] = now_dot_pos[2][0] + trans_dot_pos[2][0];
	//	now_dot_pos[2][1] = now_dot_pos[2][1] - trans_dot_pos[2][1];
	//	cout << trans_dot_pos[2][0] << ", " << trans_dot_pos[2][1] << endl;
	//	break;
	//case 3:
	//	now_dot_pos[3][0] = now_dot_pos[3][0] + trans_dot_pos[3][0];
	//	now_dot_pos[3][1] = now_dot_pos[3][1] - trans_dot_pos[3][1];
	//	cout << trans_dot_pos[3][0] << ", " << trans_dot_pos[3][1] << endl;
	//	break;
	//}

	switch (id) {
	case 0:
		now_dot_pos[0][0] =  trans_dot_pos[0][0];
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

	//glui->sync_live();
	/*
		dots[0]->set_float_array_val(set_dots);
		dots[1]->set_float_array_val(set_dots);
		dots[2]->set_float_array_val(set_dots);
		dots[3]->set_float_array_val(set_dots);*/
		//switch (id) {
		//case 0:
		//	prev_pos[0][0] = dot_pos[0][0];
		//	prev_pos[0][1] = -dot_pos[0][1];
		//	dot_pos[0][1] = -dot_pos[0][1];
		//	dots[0]->set_float_array_val(prev_pos[0]);
		//	cout << dot_pos[0][0] << ", " << dot_pos[0][1] << endl;
		//	break;
		//case 1:
		//	prev_pos[1][0] = dot_pos[1][0];
		//	prev_pos[1][1] = -dot_pos[1][1];
		//	dot_pos[1][1] = -dot_pos[1][1];
		//	dots[1]->set_float_array_val(prev_pos[1]);
		//	break;
		//case 2:
		//	dot_pos[2][1] = -dot_pos[2][1];
		//	break;
		//case 3:
		//	dot_pos[3][1] = -dot_pos[3][1];
		//	break;
		//}
}

int red, green, blue, drawing = false;

void drawCircle(int event, int x, int y, int, void* param) {
	if(isdrawing){
		if (event == EVENT_LBUTTONDOWN)
			drawing = true;
		else if (event == EVENT_MOUSEMOVE) {
			if (drawing == true)
				circle(frame, Point(x, y), 3, Scalar(blue, green, red), 10);
		}
		else if (event == EVENT_LBUTTONUP)
			drawing = false;
		imshow(Filename, frame);
		//cap << frame;
	}
}

void radioButtonCallback(int id) {
	switch (selected_point) {
	case 0:
		dots[1]->disable(); dots[2]->disable(); dots[3]->disable(); break;
	case 1:
		//dots[1]->enable();
		dots[0]->disable(); dots[2]->disable(); dots[3]->disable(); break;
	case 2:
		//dots[2]->enable();
		dots[0]->disable(); dots[1]->disable(); dots[3]->disable(); break;
	case 3:
		//dots[3]->enable();
		dots[0]->disable(); dots[1]->disable(); dots[2]->disable(); break;
	}

	dots[selected_point]->set_float_array_val(set_dots[selected_point]);
	dots[selected_point]->enable();
	glui->sync_live();
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

	movedots = glui->add_panel("Move", GLUI_PANEL_EMBOSSED);
	dots[0] = glui->add_translation_to_panel(movedots, "Left Top",		GLUI_TRANSLATION_XY, trans_dot_pos[0], 0, translation);
	dots[1] = glui->add_translation_to_panel(movedots, "Left Bottom",	GLUI_TRANSLATION_XY, trans_dot_pos[1], 1, translation);
	glui->add_column_to_panel(movedots, false);
	dots[2] = glui->add_translation_to_panel(movedots, "Right Top",		GLUI_TRANSLATION_XY, trans_dot_pos[2], 2, translation);
	dots[3] = glui->add_translation_to_panel(movedots, "Right Bottom",	GLUI_TRANSLATION_XY, trans_dot_pos[3], 3, translation);
	movedots->disable();

	glui->add_column(false);

	checkboxes = glui->add_panel("Settings", GLUI_PANEL_EMBOSSED);
	glui->add_checkbox_to_panel(checkboxes, "Wrap", &iswrap);
	glui->add_checkbox_to_panel(checkboxes, "Draw", &isdrawing);
	//glui->add_checkbox_to_panel(checkboxes, "Wrap", &isdraw);
	checkboxes->disable();

	radiobutton = glui->add_panel("Move", GLUI_PANEL_EMBOSSED);
	radiogroup=glui->add_radiogroup_to_panel(radiobutton, &selected_point, 0, radioButtonCallback);
	glui->add_radiobutton_to_group(radiogroup, "Left Top");
	glui->add_radiobutton_to_group(radiogroup, "Left Bottom");
	glui->add_column_to_panel(radiobutton, false);
	glui->add_radiobutton_to_group(radiogroup, "Right Top");
	glui->add_radiobutton_to_group(radiogroup, "Right Bottom");
	radiobutton->disable();

	glui->add_button("Open", 0, OpenVideo);
	btn_save = glui->add_button("Save", 0, SaveVideo);
	btn_save->disable();
	glui->add_button("Quit", 0, (GLUI_Update_CB)exit);
	Canny_ui();
	glui->set_main_gfx_window(main_window);

	glutMainLoop();
	return EXIT_SUCCESS;
}

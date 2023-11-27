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

GLUI* glui; 
GLUI_Panel* playbutton;
GLUI_Panel* playbar;
GLUI_Scrollbar* scrollbar_play;
GLUI_Button* btn_save;
int framebar;

void pause_handler(int id = -1);

int open() {
	OpenFileDialog* openFileDialog = new OpenFileDialog();
	if (openFileDialog->ShowDialog()) {
		Filename = openFileDialog->FileName;
		return(1);
	}
	else return(-1);
}

int Save() {
	SaveFileDialog* openFileDialog = new SaveFileDialog();
	if (openFileDialog->ShowDialog()) {
		Filename = openFileDialog->FileName;
		vid.set(CAP_PROP_POS_AVI_RATIO, 0);
		VideoWriter output(Filename, VideoWriter::fourcc('M', 'J', 'P', 'G'), vid_fps, Size(vid_width, vid_height));//, iscolor
		if (!output.isOpened())
		{
			std::cout << "Can't write video !!! check setting" << std::endl;
			return -1;
		}
		namedWindow(Filename);
		while (1) {
			vid >> output_frame;
			if (output_frame.empty()) {
				cout << "Video END" << std::endl;
				break;
			}
			output << output_frame;
			imshow(Filename, output_frame);
			if (waitKey(1) == 27) {//1000 / vid_fps
				cout << "Stop video record" << endl;
				break;
			}
		}
		destroyWindow(Filename);
		return 0;
	}
	return -1;
}
void SaveVideo(int id) {
	Save();
}
void OpenVideo(int id) {
	if (open() == 1) {
		VideoCapture cap(Filename);
		vid = cap;
		namedWindow("frame", 1);
		vid >> frame;
		imshow("frame", frame); 

		vid_fps = vid.get(CAP_PROP_FPS);
		vid_msec = 1.0 / (double)vid_fps * 1000; 
		vid_framecount = vid.get(CAP_PROP_FRAME_COUNT);
		vid_width = vid.get(CAP_PROP_FRAME_WIDTH);
		vid_height = vid.get(CAP_PROP_FRAME_HEIGHT);
		cout << "vid_framecount: " << vid_framecount << "vid_fps: " << vid_fps <<" vid_msec: " << vid_msec << endl;

		playbutton->enable();
		playbar->enable();
		btn_save->enable();
		scrollbar_play->set_int_limits(1, vid_framecount-1, GLUI_LIMIT_CLAMP);
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

		switch (playmod) {
		case PLAY_BACK_FAST:
		case PLAY_BACK:
			if (nowframe <= 1) {
				vid.set(CAP_PROP_POS_FRAMES, 1);
				cout << "처음" << nowframe << endl;
				pause_handler(1);
				break;
			}
		case PLAY_FORWARD:
		case PLAY_FORWARD_FAST:
			if (nowframe >= vid_framecount) {//vid_framecount을 초과한 경우
				
				cout << "마지막" << vid.set(CAP_PROP_POS_AVI_RATIO, 1) << endl;
				pause_handler(1);
				break;
			}

			if (nowframe >= 1 && nowframe < vid_framecount) {//정상 재생중
				vid >> frame;
				imshow("frame", frame);
				framebar = nowframe = vid.get(CAP_PROP_POS_FRAMES);
				cout << "정상 재생중" << nowframe << endl;
				if (nowframe == vid_framecount) {
					vid.set(CAP_PROP_POS_FRAMES, vid_framecount - 1);
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

bool ispause = true;
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
	imshow("frame", frame);
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

	glui->add_column(false);
	glui->add_button("Open", 0, OpenVideo);
	btn_save = glui->add_button("Save", 0, SaveVideo);
	btn_save->disable();
	glui->add_button("Quit", 0, (GLUI_Update_CB)exit);

	glui->set_main_gfx_window(main_window);
	//glutIdleFunc(idle);
	glutMainLoop();
	return EXIT_SUCCESS;
}

#include <iostream>
#include <opencv2/opencv.hpp>
#include <GL/glui.h>

using namespace cv;
using namespace std;

int main_window;
GLUI* glui;

void just_sync(int id);
void CannyThreshold(int id);
void HistogramEqualize(int id);
void Threshold(int id);

void idle();
void checkbox(int id);

void Canny_ui();
void Threshold_ui();
int mode = 0;

Mat src, dst, edges;
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
	if(mode == CANNY) {
		src = imread("lenna.jpg", IMREAD_GRAYSCALE);
		dst.create(src.size(), src.type());
		//edittext_lowTh->set_int_val(lowTh);
		blur(src, dst, Size(blur_size, blur_size));
		Canny(dst, edges, lowTh, lowTh * ratioo, kernel_size);
		dst = Scalar::all(0);
		src.copyTo(dst, edges);
		imshow("Input", src);
		imshow("Output", edges);
		//imshow("Image+Canny", dst);

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

void HistogramEqualize(int id) {
	if (mode == HISTEQ) {
		src = imread("lenna.jpg", IMREAD_GRAYSCALE);
		dst.create(src.size(), src.type());

		equalizeHist(src, dst);

		imshow("Input", src);
		imshow("Output", dst);
	}
		glui->sync_live();
}

int threshold_val = 100;
int threshold_type = 0;
void Threshold(int id) {
	if (mode == THRESHOLD) {
		src = imread("lenna.jpg", IMREAD_GRAYSCALE);
		dst.create(src.size(), src.type());

		threshold(src, dst, threshold_val, 255, threshold_type);

		imshow("Input", src);
		imshow("Output", dst);
	}
		glui->sync_live();
}

void Threshold_ui() {
	GLUI_Rollout* rollout_th = glui->add_rollout("Threshold");
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
}

#define CHECHBOX_SIZE 5
int v[CHECHBOX_SIZE] = {1};
const char* checkbox_names[CHECHBOX_SIZE] = { 
	"CannyThreshold",
	"Threshold",
	"HistogramEqualize",
	"qwer", 
	"1234"
};

void idle() {
	switch (mode) {
	case CANNY: CannyThreshold(mode);break;
	case THRESHOLD: Threshold(mode); break;
	case HISTEQ: HistogramEqualize(mode); break;
	case 5:  break;
	default: break;
	}
}

void checkbox(int id) {
	mode = id;
	idle();
	for (int i = 0; i < CHECHBOX_SIZE; i++) {
		if (i == id) v[id] = 1;
		else v[i] = 0;
	}
	glui->sync_live();
}

int main(int argc, char* argv[]) {
	glui = GLUI_Master.create_glui("Menu", 0);
	main_window = glui->get_glut_window_id();
	GLUI_Master.set_glutIdleFunc(NULL);
	cout << main_window << endl;

	glui->add_statictext("60191798 KYC");

	Canny_ui();
	Threshold_ui();

	glui->add_column(false);

	for (int i = 0; i < CHECHBOX_SIZE; i++) {
		glui->add_checkbox(checkbox_names[i], &v[i], i, checkbox);
	}

	glui->add_separator();
	//new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);
	glui -> add_button("Quit", 0, (GLUI_Update_CB)exit);

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
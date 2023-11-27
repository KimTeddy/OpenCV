#include "opencv2/opencv.hpp"
#include <GL/glut.h>
#include <GL/glui.h>
#include "OpenFileDialog.h"
#include "SaveFileDialog.h"
//#include <string>
using namespace cv;
//using namespace std;

int main_window;
String Filename;
Mat src;

int open() {
	OpenFileDialog* openFileDialog = new OpenFileDialog();
	if (openFileDialog->ShowDialog()) {
		Filename = openFileDialog->FileName;
		return(1);
	}
	else return(-1);
}
void save() {
	SaveFileDialog* openFileDialog = new SaveFileDialog();
	if (openFileDialog->ShowDialog()) {
		Filename = openFileDialog->FileName;
	}
}

void OpenImage(int id) {
	if (open() == 1) {
		src = imread(Filename);
		imshow("src", src);
	}
}
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	GLUI* glui = GLUI_Master.create_glui("Menu", 0);
	main_window = glui->get_glut_window_id();
	GLUI_Master.set_glutIdleFunc(NULL);

	glui->add_button("Open", 0, OpenImage);

	new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);
	glui->set_main_gfx_window(main_window);
	glutMainLoop();
	return EXIT_SUCCESS;
}



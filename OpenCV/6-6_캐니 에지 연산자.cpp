#include <opencv2/opencv.hpp>
#include <GL/glui.h>

using namespace cv;
using namespace std;

Mat src, dst, edges;
int lowTh = 1, ratioo = 3, kernel_size = 3, max_lowTh = 100;
GLUI_EditText* edittext;
void CannyThreshold(int id)
{
	edittext->set_int_val(lowTh);
	blur(src, dst, Size(3, 3));
	Canny(dst, edges, lowTh, lowTh * ratioo, kernel_size);
	dst = Scalar::all(0);
	src.copyTo(dst, edges);
	imshow("Image", src);
	imshow("Canny", edges);
	imshow("Image+Canny", dst);	
}
int main_window;
int main(int argc, char* argv[])
{
	GLUI* glui = GLUI_Master.create_glui("Menu", 0);
	main_window = glui->get_glut_window_id();
	GLUI_Master.set_glutIdleFunc(NULL);

	edittext = new GLUI_EditText(glui, "LowTh : ", &lowTh);
	GLUI_Scrollbar* scrollbar = new GLUI_Scrollbar(glui, "LowTh",
		GLUI_SCROLL_HORIZONTAL, &lowTh, -1, CannyThreshold);
	scrollbar->set_int_limits(0, max_lowTh, GLUI_LIMIT_CLAMP);
	scrollbar->set_speed(0.001);
	new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);

	src = imread("lenna.jpg", IMREAD_GRAYSCALE);
	dst.create(src.size(), src.type());
	CannyThreshold(0);

	glui->set_main_gfx_window(main_window);
	glutMainLoop();
	return EXIT_SUCCESS;
}

#include <cmath>
#include <vector>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "bevgrafmath2017.h"

//	Size of window
GLsizei winHeight = 800, winWidth = 1000;

void init() {
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glShadeModel(GL_SMOOTH);
	gluOrtho2D(0.0, winWidth, 0.0, winHeight);
}

void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);

	glutSwapBuffers();
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("");
	init();
	glutDisplayFunc(myDisplay);
	//glutMouseFunc(processMouse);
	//glutMotionFunc(processMouseActiveMotion);
	//glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}

//	Vig Levente István
//	GFZ5JS
//	2017/18	DE-IK PTI
//

#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "bevgrafmath2017.h"


GLfloat winWidth = 800.0f, winHeight = 800.0f;
GLfloat viewportPositionX = 325.0f, viewportPositionY = 300.0f, viewportSize = 150.0f;

mat4 Rz, Vo, Vc, w2v, To, Tc, ToR, TcR, coordTrans;

vec3 camera, Xn, Yn, Zn, up = {0,0,1};
GLfloat uCam = 0.0f, vCam = 0.0f, rCam = 3;

vec3 lightSource = {0, 0, 1};

bool orthogonal = true;

GLfloat alphaZ = 0.0f;
GLfloat center = 5.0f;
GLfloat delta = 0.05f;

GLfloat R = 3, r = 0.75;

vec3 cube[8] = {{-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}, {0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5},
                {-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5}, {0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5}};

struct Face {
    vec3 vertices[4];
    vec3 centerPoint;
    vec3 normalVecor;
    vec3 color;
    char object;    //  Cube or Torus
};

std::vector<Face> allFaces;

bool pointLessZ(Face a, Face b) {
    // orthogonal
	return a.centerPoint.z < b.centerPoint.z;
}

bool centerDist(Face a, Face b) {
    // perspective
    return dist(a.centerPoint, (0, 0, center)) > dist(b.centerPoint, (0, 0, center));
}


void setNormalVector(Face& f) {
		f.normalVecor = cross(f.vertices[1] - f.vertices[0], f.vertices[2] - f.vertices[0]);
}

void setCenterPoint(Face& f) {
	f.centerPoint = (f.vertices[0] + f.vertices[1] + f.vertices[2] + f.vertices[3]) / 4;
}

void initFaces() {
    allFaces.clear();

    Face f;
    f.object = 'c';

    f.vertices[0] = cube[0];
    f.vertices[1] = cube[1];
    f.vertices[2] = cube[5];
    f.vertices[3] = cube[4];
    allFaces.push_back(f);

    f.vertices[0] = cube[1];
    f.vertices[1] = cube[2];
    f.vertices[2] = cube[6];
    f.vertices[3] = cube[5];
    allFaces.push_back(f);

    f.vertices[0] = cube[2];
    f.vertices[1] = cube[3];
    f.vertices[2] = cube[7];
    f.vertices[3] = cube[6];
    allFaces.push_back(f);

    f.vertices[0] = cube[3];
    f.vertices[1] = cube[0];
    f.vertices[2] = cube[4];
    f.vertices[3] = cube[7];
    allFaces.push_back(f);

    f.vertices[0] = cube[1];
    f.vertices[1] = cube[0];
    f.vertices[2] = cube[3];
    f.vertices[3] = cube[2];
    allFaces.push_back(f);

    f.vertices[0] = cube[4];
    f.vertices[1] = cube[5];
    f.vertices[2] = cube[6];
    f.vertices[3] = cube[7];
    allFaces.push_back(f);

    f.object = 't';

    for (double u = 0; u <= two_pi(); u += pi() / 12) {
        for (double v = 0; v <= two_pi(); v += pi() / 12) {
            f.vertices[0] = vec3((R + r * cos(u)) * cos(v),
                                 (R + r * cos(u)) * sin(v),
                                  r * sin(u));
            f.vertices[1] = vec3((R + r * cos(u)) * cos(v + pi() / 12),
                                 (R + r * cos(u)) * sin(v + pi() / 12),
                                  r * sin(u));
            f.vertices[2] = vec3((R + r * cos(u + pi() / 12)) * cos(v + pi() / 12),
                                 (R + r * cos(u + pi() / 12)) * sin(v + pi() / 12),
                                  r * sin(u + pi() / 12));
            f.vertices[3] = vec3((R + r * cos(u + pi() / 12)) * cos(v),
                                 (R + r * cos(u + pi() / 12)) * sin(v),
                                  r * sin(u + pi() / 12));

            allFaces.push_back(f);
        }
    }

	for (int i = 0; i < allFaces.size(); i++) {
		setNormalVector(allFaces[i]);
		setCenterPoint(allFaces[i]);
	}
}

void initTransformations()
{
	Rz = rotateZ(alphaZ);

	Vo = ortho();
	Vc = perspective(center);

	camera = {rCam * cos(uCam), rCam * sin(uCam), vCam};

	Zn = normalize(vec3(0,0,0) - (-camera));
	Xn = normalize(cross(up,Zn));
	Yn = normalize(cross(Zn, Xn));

	coordTrans = coordinateTransform(camera, Xn, Yn, Zn);

	w2v = windowToViewport3(vec2(-1.0f, -1.0f), vec2(2.0f, 2.0f),
                            vec2(viewportPositionX, viewportPositionY), vec2(viewportSize, viewportSize));

	To = coordTrans;
	ToR = To * Rz;

	Tc = coordTrans;
	TcR = Tc * Rz;
}

void init()
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, winWidth, 0.0f, winHeight, 0.0f, 1.0f);

    initFaces();
	initTransformations();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    std::vector<Face> transformedFaces;


    for (int i = 0; i < allFaces.size(); i++) {

        Face f = allFaces[i];
        vec3 result;

        GLfloat c = (dot(normalize(f.normalVecor), normalize(lightSource)) + 1) / 2;
        f.color = (c,c,c);

        for (int j = 0; j < 4; j++) {

            vec4 pointH = ihToH(f.vertices[j]);
            vec4 transformedPoint;

            if (f.object == 'c') {
                if (orthogonal)
                    transformedPoint = To * pointH;
                else
                    transformedPoint = Tc * pointH;
            } else {
                if (orthogonal)
                    transformedPoint = ToR * pointH;
                else
                    transformedPoint = TcR * pointH;
            }

            if (transformedPoint.w != 0) {
                vec3 result = hToIh(transformedPoint);
                f.vertices[j] = result;
            }
        }
        setNormalVector(f);
        setCenterPoint(f);

        // vec4 lightH = ihToH(lightSource);
        // vec4 transformedLight;
        //
        // if (orthogonal) {
        //     transformedLight = transpose(inverse(To)) * lightH;
        // }
        // else {
        //     transformedLight = transpose(inverse(Tc)) * lightH;
        // }
        //
        // c = (dot(normalize(f.normalVecor), normalize(transformedLight)) + 1) / 2;
        // f.color = (c,c,c);

        transformedFaces.push_back(f);
    }

    if (orthogonal)
        std::sort(transformedFaces.begin(), transformedFaces.end(), pointLessZ);
    else
        std::sort(transformedFaces.begin(), transformedFaces.end(), centerDist);

    for (int i = 0; i < transformedFaces.size(); i++) {

        Face f = transformedFaces[i];
        vec3 result;

        for (int j = 0; j < 4; j++) {

            vec4 pointH = ihToH(f.vertices[j]);
            vec4 transformedPoint;

            if (f.object == 'c') {
                if (orthogonal)
                    transformedPoint = w2v * Vo * pointH;
                else
                    transformedPoint = w2v * Vc * pointH;
            } else {
                if (orthogonal)
                    transformedPoint = w2v * Vo * pointH;
                else
                    transformedPoint = w2v * Vc * pointH;
            }

            if (transformedPoint.w != 0) {
                vec3 result = hToIh(transformedPoint);
                f.vertices[j] = result;
            }
        }

        if (orthogonal) {
            if (f.normalVecor.z > 0) {
                glLineWidth(2.0);
                glBegin(GL_LINE_LOOP);
                glColor3f(0.0, 0.0, 0.0);
                glLineWidth(5);
                for (int j = 0; j < 4; j++) {
                    glVertex2f(f.vertices[j].x, f.vertices[j].y);
                }
                glEnd();

                glBegin(GL_POLYGON);
                glColor3f(f.color.x, f.color.y, f.color.z);
                for (int j = 0; j < 4; j++) {
                    glVertex2f(f.vertices[j].x, f.vertices[j].y);
                }
                glEnd();
            }
        } else {
            if (dot(normalize(f.normalVecor), normalize(vec3(0, 0, center) - f.centerPoint)) > 0) {
                glLineWidth(2.0);
                glBegin(GL_LINE_LOOP);
                glColor3f(0.0, 0.0, 0.0);
                glLineWidth(5);
                for (int j = 0; j < 4; j++) {
                    glVertex2f(f.vertices[j].x, f.vertices[j].y);
                }
                glEnd();

                glBegin(GL_POLYGON);
                glColor3f(f.color.x, f.color.y, f.color.z);
                for (int j = 0; j < 4; j++) {
                    glVertex2f(f.vertices[j].x, f.vertices[j].y);
                }
                glEnd();
            }
        }
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;

	case 'a':
		uCam -= delta;
		if(uCam <= 0)
			uCam = two_pi();
		break;
	case 'd':
		if(uCam >= two_pi() )
			uCam = 0;
		uCam += delta;
		break;
	case 'w':
		vCam += delta;
		break;
	case 's':
		vCam -= delta;
		break;
	case 'r':
		rCam -= delta;
		break;
	case 't':
		rCam += delta;
		break;

    case 'q':
		center -= delta;
		break;
	case 'e':
		center += delta;
		break;

	case 'v':
		orthogonal = !orthogonal;
		break;

    case 'n':
        R -= delta;
        break;
    case 'm':
        R += delta;
        break;
    case 'j':
        r -= delta;
        break;
    case 'k':
        r += delta;
        break;
	}

    initFaces();
	initTransformations();
	glutPostRedisplay();
}

void update(int v){

    alphaZ += 0.01;
    if( alphaZ >= two_pi()) {
        alphaZ = 0;
    }
    initTransformations();

    glutPostRedisplay();
    glutTimerFunc(10, update, 0);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);                         // Initialize GLUT.
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);   // Set display mode.
	glutInitWindowPosition(50, 100);   // Set top-left display-window position.
	glutInitWindowSize(winWidth, winHeight);      // Set display-window width and height.
	glutCreateWindow("2. Beadandó - Vig Levente"); // Create display window.

	init();                         // Execute initialization procedure.
	glutDisplayFunc(display);       // Send graphics to display window.
	glutKeyboardFunc(keyboard);

	glutTimerFunc(10, update, 0);

	glutMainLoop();                 // Display everything and wait.
	return 0;
}

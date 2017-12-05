#include <GLUT/glut.h>
#include "bevgrafmath2017.h"
#include <math.h>
#include <vector>

#include <iostream>

/*======================================*/

/**
* ablak mérete
*/
GLfloat winWidth = 800.0f, winHeight = 800.0f;

/**
* forgatási mátrixok
*/
mat4 Rz;

/**4
* merõleges és centrális vetítések mátrixai
*/
mat4 Vo, Vc;

/**
* Wtv mátrixok
*/
mat4 Wo, Wc;

/**
* a fenti mátrixokból elõállított két transzformációs mátrix
*/
mat4 To, Tc, ToR, TcR;

vec3 cam;
GLfloat u = 0.0f, v = 0.0f;
GLfloat rCam = 10;
mat4 koord;

vec3 up = {0,0,1};

vec3 Xn, Yn, Zn;

bool meroleges = true;

vec3 feny = {10, 10, 100};

/**
* X és Y tengely körüli forgatások
*/
GLfloat alphaX = 0.0f, alphaY = 0.0f, deltaAlpha = degToRad(1.0f);
GLfloat alphaZ = 0.0f;

/**
* nézetek koordinátái
*/
GLfloat oX = 325.0f, oY = 300.0f, oW = 150.0f, oH = 150.0f;
GLfloat cX = 325.0f, cY = 300.0f, cW = 150.0f, cH = 150.0f;
/**
* centrális vetítés középpontjának Z koordinátája
*/
GLfloat center = 5.0f, deltaCenter = 0.05f;

GLfloat R = 3;
GLfloat r = 1;


vec3 cube[8] = {
	{-1, -1, -1},
	{1, -1, -1},
	{1, 1, -1},
	{-1, 1, -1},
	{-1, -1, 1},
	{1, -1, 1},
	{1, 1, 1},
	{-1, 1, 1}
};

//vec2 drawableCube[8] = {};

struct Face {
	std::vector<vec3> csucsok(4);
    vec3 norma;
    float szin[6];
	vec3 kozepPont;
};

Face CubeFaces[6] = {};

std::vector<Face> allFaces;

void setNormalVectors() {
    for (int i = 0; i < 6; i++)
        CubeFaces[i].norma = cross(drawableCube[CubeFaces[i].p.z] - drawableCube[CubeFaces[i].p.x],
                                   drawableCube[CubeFaces[i].p.y] - drawableCube[CubeFaces[i].p.x]);
}
/*
void setKozePont() {
	for (int i = 0; i < 6; i++)
        CubeFaces[i].kozepPont = {0.5 * CubeFaces[i].p[0].x + 0.5 * CubeFaces[i].p[2].x,
								  0.5 * CubeFaces[i].p[0].y + 0.5 * CubeFaces[i].p[2].y,
							  	  0.5 * CubeFaces[i].p[0].z + 0.5 * CubeFaces[i].p[2].z};
}
*/
void initFaces() {
	/*
	CubeFaces[0].p = vec4(0,1,2,3);
	CubeFaces[1].p = vec4(1,5,6,2);
	CubeFaces[2].p = vec4(3,2,6,7);
	CubeFaces[3].p = vec4(4,0,3,7);
	CubeFaces[4].p = vec4(4,5,1,0);
	CubeFaces[5].p = vec4(5,4,7,6);
	*/


    CubeFaces[0].p[0] = 0;
    CubeFaces[0].p[1] = 1;
    CubeFaces[0].p[2] = 2;
    CubeFaces[0].p[3] = 3;

    CubeFaces[1].p[0] = 1;
    CubeFaces[1].p[1] = 5;
    CubeFaces[1].p[2] = 6;
    CubeFaces[1].p[3] = 2;

    CubeFaces[2].p[0] = 3;
    CubeFaces[2].p[1] = 2;
    CubeFaces[2].p[2] = 6;
    CubeFaces[2].p[3] = 7;

    CubeFaces[3].p[0] = 4;
    CubeFaces[3].p[1] = 0;
    CubeFaces[3].p[2] = 3;
    CubeFaces[3].p[3] = 7;

    CubeFaces[4].p[0] = 4;
    CubeFaces[4].p[1] = 5;
    CubeFaces[4].p[2] = 1;
    CubeFaces[4].p[3] = 0;

    CubeFaces[5].p[0] = 5;
    CubeFaces[5].p[1] = 4;
    CubeFaces[5].p[2] = 7;
    CubeFaces[5].p[3] = 6;


    setNormalVectors();
	//setKozePont();

	/*
    for (int k = 0; k < 6; k++) {
        CubeFaces[k].szin[0] = k;
        CubeFaces[k].szin[1] = k + 3;
        CubeFaces[k].szin[2] = 0.0688 * k;
    }
	*/

}

/**
* elõállítja a szükséges mátrixokat
*/
void initTransformations()
{
	// forgatási mátrixok
	Rz = rotateZ(alphaZ);

	// vetítési mátrixok
	Vo = ortho();
	Vc = perspective(center);

	cam = {rCam * cos(u), rCam * sin(u), v};

	Zn = normalize(-1 * cam);
	Xn = normalize(cross(up,Zn));
	Yn = normalize(cross(Zn, Xn));

	koord = coordinateTransform(cam, Xn, Yn, Zn);

	// Wtv mátrixok
	Wo = windowToViewport3(vec2(-1.0f, -1.0f), vec2(2.0f, 2.0f), vec2(oX, oY), vec2(oW, oH));
	Wc = windowToViewport3(vec2(-1.0f, -1.0f), vec2(2.0f, 2.0f), vec2(cX, cY), vec2(cW, cH));

	// merõleges
	To = Wo * Vo * koord;
	ToR = Wo * Vo * koord * Rz;

	// centrális
	Tc = Wc * Vc * koord;
	TcR = Wc * Vc * koord * Rz;
}

/*======================================*/

void init()
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, winWidth, 0.0f, winHeight, 0.0f, 1.0f);

	initTransformations();
}

void drawCube(mat4 T)
{
	for (int i = 0; i < 8; i++) {

		vec4 pointH = ihToH(cube[i]);
		vec4 transformedPoint = T * pointH;

		if (transformedPoint.w != 0) {
			vec3 result = hToIh(transformedPoint);
			drawableCube[i] = {result.x, result.y};
		}

	}

	glPointSize(10);
	glColor3f(0, 0, 0);
    glBegin(GL_POINTS);
    for (int i = 0; i < 8; i++) {
        glVertex2f(drawableCube[i].x, drawableCube[i].y);
    }
    glEnd();

    for (int i = 0; i < 6; i++) {
		if (meroleges) {
			if (CubeFaces[i].norma.z > 0) {
				GLfloat c = (dot(normalize(CubeFaces[i].norma), normalize(feny)) + 1) / 2;
				//std::cout << c << '\n';
	            glBegin(GL_QUADS);
	            glColor3f(0.5, 0.5, 0.5);
	            for (int j = 0; j < 4; j++) {
	                glVertex2f(drawableCube[CubeFaces[i].p[j]].x, drawableCube[CubeFaces[i].p[j]].y);
	            }
	            glEnd();
	        }
		}

		else {
			if ( dot(CubeFaces[i].norma, cam - CubeFaces[i].kozepPont) > 0 ) {
				GLfloat c = (dot(normalize(CubeFaces[i].norma), normalize(feny)) + 1) / 2;
				//std::cout << c << '\n';
				glBegin(GL_QUADS);
				glColor3f(0.5, 0.5, 0.5);
				glVertex2f(drawableCube[CubeFaces[i].p.x].x, drawableCube[CubeFaces[i].p.x].y);
				glVertex2f(drawableCube[CubeFaces[i].p.y].x, drawableCube[CubeFaces[i].p.y].y);
				glVertex2f(drawableCube[CubeFaces[i].p.z].x, drawableCube[CubeFaces[i].p.z].y);
				glVertex2f(drawableCube[CubeFaces[i].p.w].x, drawableCube[CubeFaces[i].p.w].y);
				glEnd();
			}
		}

    }

}

void drawTorus(vec3 color, mat4 T)
{
	// beállítja a gömb éleinek vastagságát
	glLineWidth(2.0f);

	// beállítja a kocka színét
	glColor3f(color.x, color.y, color.z);

	//hosszusagi korok
	for (double u = 0; u <= two_pi() + 0.001; u += pi() / 6)
	{
		glBegin(GL_POINTS);
		for (double v = 0; v <= two_pi() + 0.001; v += pi() / 6)
		{
			vec3 pih = vec3(( R + r * cos(u)) * cos(v), ( R + r * cos(u)) * sin(v), r * sin(u));
			vec4 ph = ihToH(pih);
			vec4 pt = T * ph;
			if (pt.w != 0)
			{
				vec3 ptih = hToIh(pt);
				glVertex2f(ptih.x, ptih.y);
			}
		}
		glEnd();
	}

	/*
    //szelessegi korok
	for (double v = 0; v <= two_pi() + 0.001; v += pi() / 6)
	{
		glBegin(GL_POINTS);
		for (double u = 0; u <= two_pi() + 0.001; u += pi() / 6)
		{
			vec3 pih = vec3((R + r* cos(u)) * cos(v), (R + r* cos(u)) * sin(v), r* sin(u));
			vec4 ph = ihToH(pih);
			vec4 pt = T * ph;
			if (pt.w != 0)
			{
				vec3 ptih = hToIh(pt);
				glVertex2f(ptih.x, ptih.y);
			}
		}
		glEnd();
	}
	*/

	Face lapok;
    torusFaces.clear();

    for (double u = 0; u <= two_pi(); u += pi() / 10) {

        for (double v = 0; v <= two_pi(); v += pi() / 10) {

            lapok.p[0] = vec3((R_torus + r_torus * cos(u)) * cos(v), (R_torus + r_torus * cos(u)) * sin(v), r_torus * sin(u));

            lapok.p[1] = vec3((R_torus + r_torus * cos(u)) * cos(v + pi() / 10), (R_torus + r_torus * cos(u)) * sin(v + pi() / 10), r_torus * sin(u));

            lapok.p[2] = vec3((R_torus + r_torus * cos(u + pi() / 10)) * cos(v + pi() / 10), (R_torus + r_torus * cos(u + pi() / 10)) * sin(v + pi() / 10), r_torus * sin(u + pi() / 10));

            lapok.p[3] = vec3((R_torus + r_torus * cos(u + pi() / 10)) * cos(v), (R_torus + r_torus * cos(u + pi() / 10)) * sin(v), r_torus * sin(u + pi() / 10));

            lapok.lapAtlag = (lapok.p[0] + lapok.p[1] + lapok.p[2] + lapok.p[3]) / 4;

//face-ek eltárolása
            torusFaces.push_back(lapok);
        }
    }

    setNormalVectors();

}
}

void draw()
{

	initFaces();
	glClear(GL_COLOR_BUFFER_BIT);

	if (meroleges) {
		drawTorus(vec3(0.0f, 0.0f, 0.0f), ToR);
		drawCube(To);
	}
	else{
		drawTorus(vec3(0.0f, 0.0f, 0.0f), TcR);
		drawCube(Tc);
	}


	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
		exit(0);
		break;
	case 'a':
		u -= 0.1;
		if(u <= 0)
			u = two_pi();
		break;
	case 'd':
		if(u >= two_pi() )
			u = 0;
		u += 0.1;
		break;
	case 'w':
		v -= 0.1;
		if (v <= -10.0)
			v = -10.0;
		break;
	case 's':
		v += 0.1;
		if (v >= 10.0)
			v = 10.0;
		break;
	case '+':
		center -= deltaCenter;
		break;
	case '-':
		center += deltaCenter;
		break;
	case 'v':
		meroleges = !meroleges;
		break;
	case 'r':
		rCam -= 0.2;
		break;
	case 't':
		rCam += 0.2;
		break;
	}

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
	glutCreateWindow("Wonderful Sphere"); // Create display window.

	init();                         // Execute initialization procedure.
	glutDisplayFunc(draw);       // Send graphics to display window.
	glutKeyboardFunc(keyboard);

	glutTimerFunc(10, update, 0);

	glutMainLoop();                 // Display everything and wait.
	return 0;
}

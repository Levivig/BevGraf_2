#include <GLUT/glut.h>
#include "bevgrafmath2017.h"
#include <math.h>

GLsizei winWidth = 800, winHeight = 800;
GLint keyStates[256];
bool upd = false;
int N = 8;
vec3 cube[8] = {
        {-1, -1, -1},   //c0
        {1,  -1, -1},   //c1
        {1,  1,  -1},   //c2
        {-1, 1,  -1},   //c3
        {-1, -1, 1},    //c4
        {1,  -1, 1},    //c5
        {1,  1,  1},    //c6
        {-1, 1,  1},    //c7
};
vec2 drawableCube[8] = {};

mat4 w2v, projection;
float alpha = 0;
float beta = 0;

struct Face {
    int p[4];
    vec3 norma;
    float szin[6];
};

Face CubeFaces[6] = {};

void setNormalVectors() {
    for (int i = 0; i < 6; i++)
        CubeFaces[i].norma = cross(drawableCube[CubeFaces[i].p[2]] - drawableCube[CubeFaces[i].p[0]],
                                   drawableCube[CubeFaces[i].p[1]] - drawableCube[CubeFaces[i].p[0]]);
}

void initFaces() {
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

    for (int k = 0; k < 6; k++) {
        CubeFaces[k].szin[0] = k;
        CubeFaces[k].szin[1] = k + 3;
        CubeFaces[k].szin[2] = 0.0688 * k;
    }

}


void initMatrices() {

    projection = ortho();

    vec2 windowSize = {4, 4};
    vec2 windowPosition = {-2, -2};
    vec2 viewportSize = {800, 800};
    vec2 viewportPosition = {0, 0};
    w2v = windowToViewport3(windowPosition, windowSize, viewportPosition, viewportSize);
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, winWidth, 0.0, winHeight);
    glShadeModel(GL_FLAT);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(5.0);
    glLineWidth(1.0);

    initMatrices();
}

void drawCube() {
    glColor3f(0.0, 1.0, 2.0);
    glBegin(GL_POINTS);
    for (int i = 0; i < N; i++) {
        glVertex2f(drawableCube[i].x, drawableCube[i].y);
    }
    glEnd();

    for (int i = 0; i < 6; i++) {
        if (CubeFaces[i].norma.z > 0) {
            glBegin(GL_QUADS);
            glColor3f(CubeFaces[i].szin[0], CubeFaces[i].szin[1], CubeFaces[i].szin[2]);
            for (int j = 0; j < 4; j++) {
                glVertex2f(drawableCube[CubeFaces[i].p[j]].x, drawableCube[CubeFaces[i].p[j]].y);
            }
            glEnd();
        }
    }
}

void transform() {
    mat4 rotationX = rotateX(degToRad(alpha));
    mat4 rotationY = rotateY(degToRad(beta));

    mat4 M = w2v * projection * rotationX * rotationY;

    for (int i = 0; i < N; i++) {

        vec4 pointH = ihToH(cube[i]);
        vec4 transformedPoint = M * pointH;

        if (transformedPoint.w != 0) {
            vec3 result = hToIh(transformedPoint);
            //drawableCube[i] = { (transformedPoint.x / transformedPoint.w), (transformedPoint.y / transformedPoint.w)};
            drawableCube[i] = {result.x, result.y};
        }

    }
}

void keyPressed(unsigned char key, int x, int y) {
    keyStates[key] = 1;
}

void keyUp(unsigned char key, int x, int y) {
    keyStates[key] = 0;
}

void update(int v) {
    if (upd) {
        alpha += 1.0;
        beta += 1.0;
        transform();
    }
    glutPostRedisplay();

    glutTimerFunc(10, update, 0);
}

void keyOperations() {
    if (keyStates['a']) { beta -= 2.0; }
    if (keyStates['d']) { beta += 2.0; }
    if (keyStates['u']) { upd = !upd; }
    if (keyStates['s']) { alpha -= 2.0; }
    if (keyStates['w']) { alpha += 2.0; }

    transform();
    glutPostRedisplay();
}

void display() {
    keyOperations();
    glClear(GL_COLOR_BUFFER_BIT);
    transform();
    initFaces();
    drawCube();
    glutSwapBuffers();
}


int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Pyramid");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyPressed);

    glutKeyboardUpFunc(keyUp);
    glutTimerFunc(10, update, 0);
    glutMainLoop();
    return 0;
}

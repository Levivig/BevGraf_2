//	Vig Levente István
//	GFZ5JS
//	2017/18	DE-IK PTI
//

#include <algorithm>    // std::sort
#include <vector>   // std::vector

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <bevgrafmath2017.h>


GLint winWidth = 800, winHeight = 800;

GLfloat windowPosition = -2.0f, windowSize = 2.0f;
GLfloat viewportPosition = 200.0f, viewportSize = 200.0f;

mat4 w2v, Op, Pp, coordTrans, Rz, TR;

vec3 camera, Xn, Yn, Zn, up = vec3(0.0f, 0.0f, 1.0f);
GLfloat uCam = 0.0f, vCam = 0.0f, rCam = 3.0f;

vec3 lightSource = vec3(0.0f, 0.0f, 10.0f);

bool orthogonal = true;

GLfloat center = 5.0f;
GLfloat alphaZ = 0.0f, delta = 0.05f;

GLfloat R = 3.0f, r = 0.75f;

vec3 cube[8] = {vec3(-0.5, -0.5, 0.5),  vec3(0.5, -0.5, 0.5),
                vec3(-0.5, -0.5, -0.5), vec3(0.5, -0.5, -0.5),
                vec3(-0.5, 0.5, 0.5),   vec3(0.5, 0.5, 0.5),
                vec3(-0.5, 0.5, -0.5),  vec3(0.5, 0.5, -0.5)};

struct Face {
    vec3 vertices[4];
    vec3 centerPoint;
    vec3 normalVecor;
    vec3 color;
    char object;    //  Cube or Torus

    void setNormalVector() {
    		this->normalVecor = cross(this->vertices[1] - this->vertices[0],
                                      this->vertices[2] - this->vertices[0]);
    }

    void setCenterPoint() {
    	this->centerPoint = (this->vertices[0] + this->vertices[1] +
                             this->vertices[2] + this->vertices[3]) / 4.0f;
    }
};

std::vector<Face> faces;

void initFaces() {
    faces.clear();

    Face f;
    f.object = 'c';

    f.vertices[0] = cube[3];
    f.vertices[1] = cube[1];
    f.vertices[2] = cube[0];
    f.vertices[3] = cube[2];
    faces.push_back(f);

    f.vertices[0] = cube[7];
    f.vertices[1] = cube[5];
    f.vertices[2] = cube[1];
    f.vertices[3] = cube[3];
    faces.push_back(f);

    f.vertices[0] = cube[6];
    f.vertices[1] = cube[4];
    f.vertices[2] = cube[5];
    f.vertices[3] = cube[7];
    faces.push_back(f);

    f.vertices[0] = cube[2];
    f.vertices[1] = cube[0];
    f.vertices[2] = cube[4];
    f.vertices[3] = cube[6];
    faces.push_back(f);

    f.vertices[0] = cube[0];
    f.vertices[1] = cube[1];
    f.vertices[2] = cube[5];
    f.vertices[3] = cube[4];
    faces.push_back(f);

    f.vertices[0] = cube[3];
    f.vertices[1] = cube[2];
    f.vertices[2] = cube[6];
    f.vertices[3] = cube[7];
    faces.push_back(f);

    f.object = 't';

    for (GLfloat u = 0.0f; u <= two_pi(); u += pi() / 12.0f) {
        for (GLfloat v = 0.0f; v <= two_pi(); v += pi() / 12.0f) {
            f.vertices[0] = vec3((R + r * cos(u)) * cos(v),
                                 (R + r * cos(u)) * sin(v),
                                  r * sin(u));
            f.vertices[1] = vec3((R + r * cos(u)) * cos(v + pi() / 12.0f),
                                 (R + r * cos(u)) * sin(v + pi() / 12.0f),
                                  r * sin(u));
            f.vertices[2] = vec3((R + r * cos(u + pi() / 12.0f)) * cos(v + pi() / 12.0f),
                                 (R + r * cos(u + pi() / 12.0f)) * sin(v + pi() / 12.0f),
                                  r * sin(u + pi() / 12.0f));
            f.vertices[3] = vec3((R + r * cos(u + pi() / 12.0f)) * cos(v),
                                 (R + r * cos(u + pi() / 12.0f)) * sin(v),
                                  r * sin(u + pi() / 12.0f));

            faces.push_back(f);
        }
    }
}

void initTransformations() {
	Rz = rotateZ(alphaZ);

	Op = ortho();
	Pp = perspective(center);

	camera = vec3(rCam * cos(uCam), rCam * sin(uCam), vCam);

	Zn = normalize(vec3(0.0f, 0.0f, 0.0f) - (-camera));
	Xn = normalize(cross(up, Zn));
	Yn = normalize(cross(Zn, Xn));

	coordTrans = coordinateTransform(camera, Xn, Yn, Zn);

	w2v = windowToViewport3(vec2(windowPosition, windowPosition),
                            vec2(windowSize, windowSize),
                            vec2(viewportPosition, viewportPosition),
                            vec2(viewportSize, viewportSize));

	TR = coordTrans * Rz;
}

void init() {
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, winWidth, 0.0f, winHeight, 0.0f, 2.0f);

    initFaces();
	initTransformations();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    std::vector<Face> transformedFaces;

    vec4 lightH = ihToH(lightSource);
    vec4 transformedLight = transpose(inverse(coordTrans)) * lightH;
    vec3 resultLight = vec3(transformedLight.x,
                            transformedLight.y,
                            transformedLight.z);

    for (int i = 0; i < faces.size(); i++) {
        Face f = faces[i];

        for (int j = 0; j < 4; j++) {
            vec4 pointH = ihToH(f.vertices[j]);
            vec4 transformedPoint;

            if (f.object == 'c')
                transformedPoint = coordTrans * pointH;
            else
                transformedPoint = TR * pointH;

            if (transformedPoint.w != 0.0f) {
                vec3 result = hToIh(transformedPoint);
                f.vertices[j] = result;
            }
        }

        f.setNormalVector();
        f.setCenterPoint();

        if (orthogonal && f.normalVecor.z > 0.0f ||
           !orthogonal && dot(normalize(f.normalVecor),
           normalize(vec3(0.0f, 0.0f, center) - f.centerPoint)) > 0.0f) {

            GLfloat c = (dot(normalize(f.normalVecor),
                             normalize(resultLight)) + 1.0f) / 2.0f;
            f.color = vec3(c, c, c);

            transformedFaces.push_back(f);
        }
    }

    if (orthogonal)
        std::sort(transformedFaces.begin(), transformedFaces.end(),
        [](Face a, Face b) {
            return a.centerPoint.z < b.centerPoint.z;
        });
    else
        std::sort(transformedFaces.begin(), transformedFaces.end(),
        [](Face a, Face b) {
            return dist(a.centerPoint, (0.0f, 0.0f, center)) >
                   dist(b.centerPoint, (0.0f, 0.0f, center));
        });

    for (int i = 0; i < transformedFaces.size(); i++) {
        Face f = transformedFaces[i];

        for (int j = 0; j < 4; j++) {
            vec4 pointH = ihToH(f.vertices[j]);
            vec4 transformedPoint;

            if (orthogonal)
                transformedPoint = w2v * Op * pointH;
            else
                transformedPoint = w2v * Pp * pointH;

            if (transformedPoint.w != 0) {
                vec3 result = hToIh(transformedPoint);
                f.vertices[j] = result;
            }
        }

        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glColor3f(0.0f, 0.0f, 0.0f);
        for (int j = 0; j < 4; j++)
            glVertex2f(f.vertices[j].x, f.vertices[j].y);
        glEnd();

        glBegin(GL_POLYGON);
        glColor3f(f.color.x, f.color.y, f.color.z);
        for (int j = 0; j < 4; j++)
            glVertex2f(f.vertices[j].x, f.vertices[j].y);
        glEnd();
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
		exit(0);
		break;

	case 'a':
		uCam -= delta;
		if(uCam <= 0) uCam = two_pi();
		break;
	case 'd':
		if(uCam >= two_pi()) uCam = 0;
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
        if (rCam < 0.1f) rCam = 0.1f;
		break;
	case 't':
		rCam += delta;
		break;

    case 'q':
		center -= delta;
        if (center < 0.1f) center = 0.1f;
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

void update(int v) {
    alphaZ += 0.01f;
    if(alphaZ >= two_pi()) alphaZ = 0.0f;

    initTransformations();
    glutPostRedisplay();

    glutTimerFunc(10, update, 0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("2. Beadandó - Vig Levente István (GFZ5JS)");

	glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(10, update, 0);

	init();
	glutMainLoop();
	return 0;
}

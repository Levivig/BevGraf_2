//	Vig Levente István
//	GFZ5JS
//	2017/18	DE-IK PTI
//

#include <algorithm>    // std::sort()
#include <vector>   // std::vector<>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <bevgrafmath2017.h>


GLint winWidth = 800, winHeight = 800;

vec2 windowPosition = vec2(-2.0f, -2.0), windowSize = vec2(2.0f, 2.0f);
vec2 viewportPosition = vec2(200.0f, 200.0f), viewportSize = vec2(200.0f, 200.0f);

mat4 w2v, Op, Pp, coordTrans, Rz, TR;

vec3 camera, Xn, Yn, Zn, up = vec3(0.0f, 0.0f, 1.0f);
GLfloat uCam = 5.5f, vCam = 1.5f, rCam = 3.0f;

vec3 lightSource = vec3(0.0f, 0.0f, 10.0f);

bool orthogonal = true;

GLfloat center = 5.0f;
GLfloat alphaZ = 0.0f, delta = 0.05f;

GLfloat R = 2.0f, r = 0.66f;

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

	camera = vec3(rCam * cos(uCam), rCam * sin(uCam), vCam);

	Zn = normalize(vec3(0.0f, 0.0f, 0.0f) - (-camera));
	Xn = normalize(cross(up, Zn));
	Yn = normalize(cross(Zn, Xn));

	coordTrans = coordinateTransform(camera, Xn, Yn, Zn);
    TR = coordTrans * Rz;

    Op = ortho();
    Pp = perspective(center);

	w2v = windowToViewport3(windowPosition, windowSize,
                            viewportPosition, viewportSize);
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

    vec4 transformedLight = transpose(inverse(coordTrans)) * ihToH(lightSource);
    vec3 resultLight = vec3(transformedLight.x,
                            transformedLight.y,
                            transformedLight.z);

    for (int i = 0; i < faces.size(); i++) {
        Face f = faces[i];

        for (int j = 0; j < 4; j++) {
            vec4 transformedPoint = ihToH(f.vertices[j]);;

            if (f.object == 'c')
                transformedPoint = coordTrans * transformedPoint;
            else
                transformedPoint = TR * transformedPoint;

            if (transformedPoint.w != 0.0f)
                f.vertices[j] = hToIh(transformedPoint);
        }

        f.setNormalVector();
        f.setCenterPoint();

        if ((orthogonal && f.normalVecor.z > 0.0f) ||
           (!orthogonal && dot(normalize(f.normalVecor),
           normalize(vec3(0.0f, 0.0f, center) - f.centerPoint)) > 0.0f)) {

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
            return dist(a.centerPoint, vec3(0.0f, 0.0f, center)) >
                   dist(b.centerPoint, vec3(0.0f, 0.0f, center));
        });

    for (int i = 0; i < transformedFaces.size(); i++) {
        Face f = transformedFaces[i];

        for (int j = 0; j < 4; j++) {
            vec4 transformedPoint = ihToH(f.vertices[j]);;

            if (orthogonal)
                transformedPoint = w2v * Op * transformedPoint;
            else
                transformedPoint = w2v * Pp * transformedPoint;

            if (transformedPoint.w != 0.0f)
                f.vertices[j] = hToIh(transformedPoint);
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
    	case 27: exit(0); break;

    	case 'a': uCam -= pi() / 120;
    		      if(uCam <= 0) uCam = two_pi(); break;
    	case 'd': uCam += pi() / 120;
    		      if(uCam >= two_pi()) uCam = 0; break;

    	case 'w': vCam += delta; break;
    	case 's': vCam -= delta; break;

    	case 'r': rCam -= delta;
                  if (rCam < 0.1f) rCam = 0.1f; break;
    	case 't': rCam += delta; break;

        case 'q': center -= delta; break;
    	case 'e': center += delta; break;

        case 'n': R -= delta;
                  if (R < 0.4f) R = 0.4f; break;
        case 'm': R += delta; break;
        case 'j': r -= delta;
                  if (r < 0.01f) r = 0.01f; break;
        case 'k': r += delta; break;

        case 'v': orthogonal = !orthogonal; break;
	}

    initFaces();
	initTransformations();
	glutPostRedisplay();
}

void update(int v) {
    alphaZ += 0.002f;
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

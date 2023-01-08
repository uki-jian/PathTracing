
#include "super.h"
#include "mesh.h"
#include "camera.h"
#include "octree.h"
#include "raytracer.h"


using namespace std;
using namespace glm;

//  ”Ω«±‰ªªæÿ’Û

RayTracer rt;

int INTERSECTED;


void testImg() {
	Texture tex("../model/throneroom/Candlstik_M_08_c.png");
	
	float xo = 0, yo = 0;
	float xt = 1, yt = 1;
	float dx = (xt - xo) / 1024, dy = (yt - yo) / 1024;
	float yp = yo;
	for (int y = 0; y < 1024; y++) {
		float xp = xo;
		for (int x = 0; x < 1024; x++) {
			vec3 result = tex.getColor(vec2(xp, yp));
			glColor3f(result.r, result.g, result.b);
			glVertex2i(x, y);
			xp += dx;
		}
		yp += dy;
	}
}

void refreshScreen() {
	clock_t t1 = clock();

	rt.render();

	clock_t t2 = clock();
	cout << "Rendering ends. Intersected: " << INTERSECTED << endl
		<< "Cost time: ";
	TimeParser(t2 - t1);

}

void myDisplay() {
	const int height = rt.camera.height, width = rt.camera.width;

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);

	glBegin(GL_POINTS);
	//testImg();
	refreshScreen();
	glEnd();
	glFinish();
	
}

void myIdle() {
	return glutPostRedisplay();
}

void initWindow() {
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(rt.camera.width, rt.camera.height);
	glutInitWindowPosition(800, 0);
	glutCreateWindow("Path tracing");
}

int main(int argc, char** argv) {
	rt.init();
	
	initWindow();
	glutDisplayFunc(myDisplay);
	glutIdleFunc(myIdle);
	glutMainLoop();
	return 0;
}
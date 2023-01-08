#pragma once

#include "super.h"
#include "mesh.h"
#include "ray.h"

#define MAXPOS -1000000
#define MINPOS  1000000

using namespace std;
using namespace glm;

class Camera {
public:
	vec3 eye, lookat, up;
	float fovy;
	int width, height;

	vec3 x_axis, y_axis, z_axis;	//right, up, direction
	vec3 x_view, y_view, z_view;
	float d2screen; //Ä£ÄâÆÁÄ»µÄ¾àÀë
	float ymin, ymax, xmin, xmax;

	mat4 initViewSpace();

	Ray getRay(double x, double y);
	static void loadXML(Camera& camera, Mesh& mesh, string path);
};


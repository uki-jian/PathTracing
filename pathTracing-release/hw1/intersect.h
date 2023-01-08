#pragma once
#include "super.h"
#include "mesh.h"

using namespace glm;

//和ray相交的面元信息
struct Intersect_Primitive {
	float distance = 99999; //离源的距离
	vec3 point;
	vec3 normal;
	Material* material;
	vec3 kd, ks;
	void getReflectance(Face& face);
	vec3 auxiliaryILM(Ray& ray);
};

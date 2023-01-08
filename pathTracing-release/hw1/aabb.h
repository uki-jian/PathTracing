#pragma once
#include "super.h"
#include "ray.h"
#define MAXVALUE 10000000

using namespace glm;

struct AABB {
	float maxx = -MAXVALUE, minx = MAXVALUE,
		maxy = -MAXVALUE, miny = MAXVALUE,
		maxz = -MAXVALUE, minz = MAXVALUE;
	void refresh_AABB(vec3 pos);
	bool intersect(Ray& ray);
};

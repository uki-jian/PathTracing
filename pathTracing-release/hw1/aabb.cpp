#include "aabb.h"

using namespace std;
using namespace glm;


void AABB::refresh_AABB(vec3 pos) {
	maxx = std::max(maxx, pos.x);
	minx = std::min(minx, pos.x);
	maxy = std::max(maxy, pos.y);
	miny = std::min(miny, pos.y);
	maxz = std::max(maxz, pos.z);
	minz = std::min(minz, pos.z);
}


bool AABB::intersect(Ray& ray)
{
	vec3 inverseDirection = vec3(1.f / ray.direction.x, 1.f / ray.direction.y, 1.f / ray.direction.z);

	float t1 = (minx - ray.origin.x)*inverseDirection.x;
	float t2 = (maxx - ray.origin.x)*inverseDirection.x;
	float t3 = (miny - ray.origin.y)*inverseDirection.y;
	float t4 = (maxy - ray.origin.y)*inverseDirection.y;
	float t5 = (minz - ray.origin.z)*inverseDirection.z;
	float t6 = (maxz - ray.origin.z)*inverseDirection.z;

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
	if (tmax < 0) return false;
	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax) return false;

	return tmin < ray.Tmax && tmax > ray.Tmin;
}
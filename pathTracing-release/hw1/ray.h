#pragma once
#include "super.h"

using namespace glm;
using namespace std;
#define __TMAX 99999

class Ray {
public:
	vec3 origin;	//���
	vec3 direction;	//�ⷢ���ķ���

	float Tmin = 0;		//��Tmin == 0����Ϊ���ߣ�����Ϊֱ��

	float Tmax = __TMAX;	//��Tmax == __TMAX����Ϊ���ߣ�������Ϊ�߶�

	int type;
#define __DIFFUSE 0
#define __SPECULAR 1
#define __TRANSMISSION 2
	Ray(vec3 _origin = vec3(0, 0, 0), vec3 _direction = vec3(0, 0, 0), int _type = __DIFFUSE)
		: origin(_origin), direction(normalize(_direction)), Tmin(0), Tmax(__TMAX), type( _type) {};
#define INTER_EPS 0.0001
	bool isWithinBounds(double t) { return t >= Tmin + INTER_EPS && t <= Tmax - INTER_EPS; }
	vec3 getPoint(double t) { return origin + direction * float(t); }
	float getT(vec3 pos) {
		vec3 dif = pos - origin;
		float t1 = dif.x / direction.x;
		float t2 = dif.y / direction.y;
		float t3 = dif.z / direction.z;
		if (fabs(t1 - t2) >= D_EPS || fabs(t1 - t3) >= D_EPS) {
			cout << "WARNING: point is not on the ray!" << endl;
		}
		return length(dif);
	}
#define NO_DIRECTION vec3(0, 0, 0)
	bool noDirection() { return direction == NO_DIRECTION; }
};

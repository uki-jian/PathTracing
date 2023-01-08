#pragma once
#include "super.h"

using namespace glm;
using namespace std;

static const float gamma = 2.2;
static const float GaussianTemplate[3][3] =
{
	{1.f / 16, 2.f / 16, 1.f / 16},
	{2.f / 16, 4.f / 16, 2.f / 16},
	{1.f / 16, 2.f / 16, 1.f / 16}
};

class ImageProcess {
public:
	static vec3 gammaCorrection(vec3& origin, bool reVerse = false);
	static void GaussianFilter(vector<vec3>& src, vector<vec3>& dst, int width, int height);
};

void TimeParser(clock_t time);


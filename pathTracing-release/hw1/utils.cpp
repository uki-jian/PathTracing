#include "utils.h"

using namespace glm;
using namespace std;

//gamma校正
vec3 ImageProcess::gammaCorrection(vec3& origin, bool reVerse) {
	vec3 result;
	float idx = reVerse ? gamma : 1.f / gamma;
	result.r = pow(origin.r, idx);
	result.g = pow(origin.g, idx);
	result.b = pow(origin.b, idx);

	return result;
}


//高斯滤波
void ImageProcess::GaussianFilter(vector<vec3>& src, vector<vec3>& dst, int width, int height) {
	for (int y = 1; y < height - 1; y++) {
		for (int x = 1; x < width - 1; x++) {
			vec3 result =
				src[(y - 1) * width + x - 1] * GaussianTemplate[0][0] +
				src[(y - 1) * width + x    ] * GaussianTemplate[0][1] +
				src[(y - 1) * width + x + 1] * GaussianTemplate[0][2] +
				src[(y    ) * width + x - 1] * GaussianTemplate[1][0] +
				src[(y    ) * width + x    ] * GaussianTemplate[1][1] +
				src[(y    ) * width + x + 1] * GaussianTemplate[1][2] +
				src[(y + 1) * width + x - 1] * GaussianTemplate[2][0] +
				src[(y + 1) * width + x    ] * GaussianTemplate[2][1] +
				src[(y + 1) * width + x + 1] * GaussianTemplate[2][2];

			dst[y * width + x] = result;
		}
	}
}

void TimeParser(clock_t time) {
	int h, m, s;
	h= time / 3600;
	m = (time - 3600 * h) / 60;
	s = time - 3600 * h - 60 * m;
	cout << h << "hours " << m << "minutes " << s << "seconds" << endl;
}
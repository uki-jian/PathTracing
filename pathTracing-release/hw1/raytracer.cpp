#include "raytracer.h"

using namespace std;
using namespace glm;

//�����Ƿ���ֱ�ӹ⣨�ݹ����Ϊ0��Ϊֱ�ӹ⣩
bool RayTracer::isDirectLight(int depth) {
	return !depth;
}

//����ɫ��ֵ������0~1
vec3 RayTracer::restrain021(vec3& origin) {
	vec3 result;

	result.r = std::max(std::min(origin.r, 1.f), 0.f);
	result.g = std::max(std::min(origin.g, 1.f), 0.f);
	result.b = std::max(std::min(origin.b, 1.f), 0.f);

	return result;
}


//��ȡ���䷽��
vec3 RayTracer::getReflectD(const vec3 &i, const vec3 &n) {
	return i - 2.0f * dot(n, i) * n;
}

//��ȡ���䷽��
vec3 RayTracer::getRefractD(const vec3 &i, const vec3 &n, float eta) {
	float ndoti = dot(n, i), k = 1.0f - eta * eta * (1.0f - ndoti * ndoti);
	if (k >= 0.0f) {
		return eta * i - n * (eta * ndoti + sqrt(k));
	}
	else return NULL_DIRECTION;
}

//����schlick����,����fresnel����ϵ��
double RayTracer::getFresnelIndex(double ni, double nt, double cosTheta) {
	cosTheta = abs(cosTheta);

	double f0 = (ni - nt) / (ni + nt);
	f0 *= f0;
	double schlick = f0 + (1 - f0)* pow(1.0 - cosTheta, 5);

	if (schlick > 1 || schlick < 0)cout << "ERROR:: WRONG SCHLICK INDEX" << endl;
	return schlick;
}

void RayTracer::init() {
	string path = MODEL_PATH + string(MODEL_NAME) + "/" + string(MODEL_NAME);
	mesh.init(path);
	//XML�ļ���Ϊ�ֶ�����
	Camera::loadXML(camera, mesh, path);	
	camera.initViewSpace();

	octree.initOctree(mesh);

	colorCache = new vec3*[camera.width];
	for (int i = 0; i < camera.width; i++)colorCache[i] = new vec3[camera.height];

	rayNum = RAYNUM;
	maxDepth = MAXDEPTH;
	lightSamples = LIGHTSAMPLES;
	survivalIdx = SURVIVALIDX;
	randerIteration = 0;
}

//����ֱ�ӹ���
vec3 RayTracer::renderDirectL(Ray& ray, Intersect_Primitive& primitive) {
	vec3 color = COLOR_BLACK;

//#pragma omp parallel for
	for (Face* light : mesh.lights) {
		Intersect_Primitive light_result;
//#pragma omp parallel for
		for (int i = 0; i < lightSamples; i++) {
			Ray newray;
			newray.origin = primitive.point;
			vec3 lightpos = light->getRandPoint();
			newray.direction = normalize(lightpos - primitive.point);
			newray.Tmax = newray.getT(lightpos);

			if (!octree.ifIntersectRay(octree.oct_root, newray, light_result)) {	//����û�б��ڵ�
				vec3& kd = primitive.kd, &ks = primitive.ks;
				float& ns = primitive.material->Ns;
				vec3 s = normalize(newray.direction);
				float& raylen = newray.Tmax;

				float weight = 1.f / lightSamples;								//���β�����Ȩ�أ����������ĵ���

				//L_dir = L_i * f_r * cosThetaIn * cosThetaOut / (raylen * raylen) / pdf_light
				float square = length(cross(light->dx, light->dy)) * 0.5;			//���=1/pdf
				float cosThetaIn = std::max(dot(primitive.normal, s), 0.f);		//�����
				float cosThetaOut = std::max(dot(-s, light->normal), 0.f);		//�����
				float geo = cosThetaIn * cosThetaOut / (raylen * raylen);	//����Ȩ�أ��͹�ľ���ƽ���ɷ���
				vec3 real_radiance = light->material->radiance * geo * square * weight; //

				if (primitive.material->name == string("WoodFloor")) {
					int stop = 1;
				}

				if (kd != COLOR_BLACK){
					float mDots = dot(s, primitive.normal);
					if (mDots > 0.0) color += mDots * kd * real_radiance / (float)PI;
				}

				if (ks != COLOR_BLACK){
					vec3 v = -ray.direction;
					vec3 h = normalize(s + v);
					float mDotH = dot(h, primitive.normal);
					if (mDotH > 0.0) color += pow(mDotH, ns)*ks
						*real_radiance
						*(ns + 1) / float((2 * PI));
				}
				if (ambientILM != COLOR_BLACK) {
					color += ambientILM;
				}
			}
		}
	}
	return color;
}

//���ؿ������
Ray RayTracer::MCSelect(Ray& ray, Intersect_Primitive& primitive) {

	vec3& origin = primitive.point;
	vec3& kd = primitive.kd, &ks = primitive.ks;
	float ns = primitive.material->Ns, ni = primitive.material->Ni, nt = 1.f;
	float fkd = dot(kd, COLOR_WHITE), fks = dot(ks, COLOR_WHITE);

	if (primitive.material->Ni != 1.f) {
		int stop = 1;
	}
	//��������
	if (ni != 1.f) {
		double cosTheta = dot(normalize(ray.direction), normalize(primitive.normal));
		vec3 normal;

		if (cosTheta > 0.0){
			normal = -primitive.normal;
			ni = primitive.material->Ni;
			nt = 1.0;
		}
		else{
			normal = primitive.normal;
			ni = 1.0;
			nt = primitive.material->Ni;
		}

		//frasnelָ����ƽ�淴���ı���
		double fresnelIdx = getFresnelIndex(ni, nt, abs(cosTheta));

		//�Ƿ�Ϊ�����
		if (roulette(1 - fresnelIdx)) {
			vec3 direction = getRefractD(ray.direction, normal, ni / nt);
			if (direction == NULL_DIRECTION) {
				direction = getReflectD(ray.direction, normal);
				return Ray(primitive.point, direction, __SPECULAR);
			}
			else return Ray(primitive.point, direction, __TRANSMISSION);
		}
	}

	//��ȫû�з���ϵ��
	if (fkd + fks <= 0) return Ray(origin, NULL_DIRECTION);

	//������or���淴��
	if (roulette(fkd / (fkd + fks))) {
		//������
		vec3 direction = importanceSampling(primitive.normal);
		return Ray(primitive.point, direction, __DIFFUSE);
	}
	else {
		//���淴��
		vec3 precise_direction = getReflectD(ray.direction, primitive.normal);
		vec3 direction = importanceSampling(precise_direction, ns);
		return Ray(primitive.point, direction, __SPECULAR);
	}
}

//���̶ģ�survivalProbablility�ĸ��ʱ����ܣ���һ����
bool RayTracer::roulette(double survivalProbablility) {
	if (survivalProbablility < 0 || survivalProbablility > 1)cout << "ERROR::invalid roulette" << endl;
	double randn = rand0to1();
	return randn < survivalProbablility;
}

//��Ҫ�Ȳ���
vec3 RayTracer::importanceSampling(vec3& up, double n) {
	double phi, theta;
	double r1 = (double)rand() / RAND_MAX, r2 = (double)rand() / RAND_MAX;

	phi = r1 * 2 * PI;
	theta = n < 0 ? asin(sqrt(r2)) : acos(pow(r2, 1 / (n + 1)));
	vec3 sample(sin(theta)*cos(phi), cos(theta), sin(theta)*sin(phi));

	vec3 front, right;

	if (fabs(up.x) > fabs(up.y))
		front = vec3(up.z, 0, -up.x);
	else
		front = vec3(0, -up.z, up.y);

	front = normalize(front);
	right = cross(up, front);

	mat3 m3(right, up, front);
	return normalize(m3 * sample);
}

//����׷��������
vec3 RayTracer::trace(Ray& ray, int depth) {

	Intersect_Primitive primitive;
	//�ҵ�������ཻ��Ƭ 
	if (ray.noDirection() || !octree.ifIntersectRay(octree.oct_root, ray, primitive)) {
		//cout << "Ray unintersected" << endl;
		return COLOR_BLACK;
	}



	//������ֱ�Ӵ򵽹�Դ�ϣ�Ӧֹͣ��������Ϊʹ����ֱ�ӹ��գ��ᵼ�¹�������
	if (!isDirectLight(depth) && primitive.material->isemission())return COLOR_BLACK;

	//���ﵽ������������ֱ�ӷ��ظ���ķ�������
	if (depth >= maxDepth) {
		return primitive.material->radiance;
	}

	//�ܷ���ֵ���Է���+ֱ�ӹ�+��ӹ�
	vec3 selfEMI, directILM, indirectILM;

	//�Է��⣺��������ķ���ϵ��
	selfEMI = primitive.material->radiance;

	//ֱ�ӹ��գ������Դ�в���
	directILM = renderDirectL(ray, primitive);	//����3����ǳ���

	//��ӹ��գ����̶��������Ƿ����trace
	float survivalReciprocal = 1.f / survivalIdx;	//���̶Ĵ���ʵĵ�����ʹ�����ƫ

	if (roulette(survivalIdx)) {
		Ray newray = MCSelect(ray, primitive);
		indirectILM = trace(newray, depth + 1) * survivalReciprocal * primitive.auxiliaryILM(newray);
	}
	else indirectILM = COLOR_BLACK;

	return selfEMI + directILM + indirectILM;

}

//��ȡһ������
Ray RayTracer::getRay(double x, double y) {
	vec3 direction = camera.z_view + float((x - 0.5f)) * camera.x_view * float(camera.width) / float(camera.height)
		+ float((y - 0.5f)) * camera.y_view * float(camera.width) / float(camera.height);
	Ray ray(camera.eye, direction);

	return ray;
}

//��ÿ�����ص��������ö������ߣ�����ƽ��
vec3 RayTracer::getRays(double x, double y) {
	double dx = 1.f / camera.width, dy = 1.f / camera.height;
	vector<vec3> color_components;

	//#pragma omp parallel for
	for (int i = 0; i < rayNum; i++) {
		//�ڸ����ھ��Ȳ���
		double randomX = (double)rand() / RAND_MAX;
		double randomY = (double)rand() / RAND_MAX;
		Ray ray = getRay((x + randomX)*dx, (y + randomY)*dy);
		vec3 color_component = trace(ray, 0) / float(rayNum);
		color_components.push_back(color_component);
	}

	vec3 color(0, 0, 0);
	for (int i = 0; i < rayNum; i++) {
		color += color_components[i];
	}
	return restrain021(color);
}


void RayTracer::render() {
	int width = camera.width;
	int height = camera.height;
	randerIteration++;
	vector<vec3> results;

	clock_t t1, t2;
	//#pragma omp parallel for
	for (int y = 0; y < height; y++) {
		//#pragma omp parallel for
		if ((y + 1) % 10 == 1)t1 = clock();
		for (int x = 0; x < width; x++) {
			vec3 color = getRays(x, y);
			colorCache[x][y] = (colorCache[x][y] * float((randerIteration - 1)) + color) / (float)randerIteration;

			vec3 result = ImageProcess::gammaCorrection(colorCache[x][y]) * COLOR_ENHANCE;
			result = restrain021(result);

			results.push_back(result);

		}
		cout << "Iteration: " << randerIteration << "  Percent:" << y << " of " << camera.height << endl;
		if ((y + 1) % 10 == 0) {
			t2 = clock();
			float restTime = (camera.height - y - 1)  * (t2 - t1) / 10.f / 1000.f;
			cout << "Rest time: ";
			TimeParser(restTime);
		}
	}
#ifdef GAUSSIAN
	vector<vec3> temp = results;
	ImageProcess::GaussianFilter(results, temp, width, height);
	results = temp;
#endif
	int it = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			vec3& result = results[it++];
			glColor3f(result.r, result.g, result.b);
			glVertex2i(x, y);
		}
	}
	writeImg(results);
}

//����ͼƬ
void RayTracer::writeImg(vector<vec3>& results) {
	FreeImage_Initialise();

	FIBITMAP* colorMap = FreeImage_Allocate(camera.width, camera.height, 8 * 3);
	RGBQUAD pixel;
	int it = 0;
	for (int y = 0; y < camera.height; y++) {
		for (int x = 0; x < camera.width; x++) {
			pixel.rgbRed = results[it].r * 255;
			pixel.rgbGreen = results[it].g * 255;
			pixel.rgbBlue = results[it].b * 255;
			FreeImage_SetPixelColor(colorMap, x, y, &pixel);
			it++;
		}
	}
	string result_name = RESULT_PATH + string(MODEL_NAME) + ".bmp";
	if (FreeImage_Save(FIF_BMP, colorMap, result_name.c_str()))cout << "write render image to " << RESULT_PATH << endl;
	FreeImage_DeInitialise();
}
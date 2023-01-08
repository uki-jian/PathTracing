#include "super.h"
#include "camera.h"
#include "counter.h"

mat4 Camera::initViewSpace() {
	vec3 cameraPos = eye;
	vec3 cameraTarget = lookat;
	z_axis = normalize(cameraPos - cameraTarget);	//z-axis

	y_axis = normalize(up);	//y-axis
	x_axis = normalize(cross(y_axis, z_axis));	//x-axis

	x_view = x_axis * float(2 * tan(fovy * PI / 360));
	y_view = y_axis * float(2 * tan(fovy *  height / width * PI / 360));
	z_view =  - z_axis;

	mat4 view;
	view = lookAt(eye, lookat, up);	//等同于上面的代码
	return view;
}

Ray Camera::getRay(double x, double y) {
	vec3 direction = z_view + float((x - 0.5f)) * x_view
		+ float((y - 0.5)) * y_view;
	Ray ray(eye, direction);
	return ray;
}

void Camera::loadXML(Camera& camera, Mesh& mesh, string path) {
	path = path + ".xml";
	TiXmlDocument doc;
	if (doc.LoadFile(path.c_str())) {
		//doc.Print();
	}
	else {
		cout << "Cannot find .xml file!" << endl;
	}
	//TiXmlElement* xml_root = doc.RootElement();
	////cout << xml_root->Value();
	//TiXmlElement* xml_camera = xml_root->NextSiblingElement();


	vector<string> lightNames;
	vector<vec3> radiances;

#ifdef CORNELL_BOX
	camera.eye = vec3(0, 1, 6.8);
	camera.lookat = vec3(0, 1, 5.8);
	camera.up = vec3(0, 1, 0);
	camera.fovy = 19.5;
#ifdef __DEBUG
	camera.width = DEBUG_WIDTH;
	camera.height = DEBUG_HEIGHT;
#else
	camera.width = 1024;
	camera.height = 1024;
#endif

	lightNames.push_back("Light");
	radiances.push_back(vec3(17, 12, 4));

#endif


#ifdef BEDROOM
	camera.eye = vec3(3.456, 1.212, 3.299);
	camera.lookat = vec3(2.699, 1.195, 2.645);
	camera.up = vec3(-0.013, 1.0, -0.011);
	camera.fovy = 39.4305;
#ifdef __DEBUG
	camera.width = DEBUG_WIDTH;
	camera.height = DEBUG_HEIGHT;
#else
	camera.width = 1280;
	camera.height = 720;
#endif

	lightNames.push_back("Light");
	radiances.push_back(vec3(16.4648, 16.4648, 16.4648));

#endif


#ifdef VEACH_MIS
	camera.eye = vec3(0.0, 2.0, 15.0);
	camera.lookat = vec3(0.0, 1.69521, 14.0476);
	camera.up = vec3(0.0, 0.952421, -0.304787);
	camera.fovy = 27.3909;
#ifdef __DEBUG
	camera.width = DEBUG_WIDTH;
	camera.height = DEBUG_HEIGHT;
#else
	camera.width = 1200;
	camera.height = 900;
#endif

	lightNames.push_back("Light1");
	radiances.push_back(vec3(901.803, 901.803, 901.803));
	lightNames.push_back("Light2");
	radiances.push_back(vec3(100.0, 100.0, 100.0));
	lightNames.push_back("Light3");
	radiances.push_back(vec3(11.1111, 11.1111, 11.1111));
	lightNames.push_back("Light4");
	radiances.push_back(vec3(1.23457, 1.23457, 1.23457));
	lightNames.push_back("Light5");
	radiances.push_back(vec3(800.0, 800.0, 800.0));

#endif


#ifdef THRONE_ROOM
	camera.eye = vec3(-10, 0, 0);
	camera.lookat = vec3(-9, 0.1, 0);
	camera.up = vec3(0, 1.0, 0);
	camera.fovy = 40;
#ifdef __DEBUG
	camera.width = DEBUG_WIDTH;
	camera.height = DEBUG_HEIGHT;
#else
	camera.width = 1280;
	camera.height = 720;
#endif

	lightNames.push_back("Sphere");
	radiances.push_back(vec3(800.0, 800.0, 800.0));

#endif

	//0 - 87.6 0 0 - 86.6 0 0 0 1
#ifdef MY_CB
	camera.eye = vec3(0, -40, 0);
	camera.lookat = vec3(0, -38, 0);
	camera.up = vec3(0, 0, 1);
	camera.fovy = 70;
#ifdef __DEBUG
	camera.width = DEBUG_WIDTH;
	camera.height = DEBUG_HEIGHT;
#else
	camera.width = 1000;
	camera.height = 1000;
#endif

	lightNames.push_back("Light");
	radiances.push_back(vec3(20.0, 20.0, 15.0));

#endif

	//添加material类的radiance信息
	for (int i = 0; i < lightNames.size(); i++) {
		for (Material& item : mesh.materials) {
			if (item.name == lightNames[i]) {
				item.radiance = radiances[i];
				break;
			}
		}
	}

	//将radiance非零的material添加至lights，方便后续检索
	for (int i = 0; i < mesh.faces.size(); i++) {
		int mtlIdx = mesh.faces[i].mtlIdx;
		if (mesh.materials[mtlIdx].radiance != COLOR_BLACK) {
			mesh.lights.push_back(&mesh.faces[i]);
		}
	}
}

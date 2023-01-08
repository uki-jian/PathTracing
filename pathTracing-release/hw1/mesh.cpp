#include "mesh.h"

using namespace std;

//从面中随机采样一个点
vec3 Face::getRandPoint() {
	// randx + randy <= 1
	float randx = rand0to1();
	float randy = rand0to1() * (1.f - randx);
	
	return origin + randx * dx + randy * dy;
}
vec2 Face::getToocords(vec3& pos) {
	vec3 v1 = origin, v2 = origin + dx, v3 = origin + dy;
	vec2 t1 = uv, t2 = uv + uv_dx, t3 = uv + uv_dy;

	float squareABC = length(cross(dx, dy));

	vec3 l1 = pos - v1, l2 = pos - v2, l3 = pos - v3;

	float squareAB = length(cross(l1, l2));
	float squareAC = length(cross(l1, l3));
	float squareBC = length(cross(l2, l3));
	
	float kA = squareBC / squareABC;
	float kB = squareAC / squareABC;
	float kC = squareAB / squareABC;

	//if (fabs(kA + kB + kC - 1) > D_EPS * 10) {
	//	cout << "ERROR::Position trasfromation failed :" << material->name << endl;
	//	//exit(1);
	//}
	return kA * t1 + kB * t2 + kC * t3;
}
void Mesh::init(const string& path) {
	unsigned int seed = RANDOM_SEED;
	srand(seed);
	clock_t t1 = clock();
	loadOBJ(path + ".obj");
	loadMTL(path + ".mtl");
	faceProcess();

	clock_t t2 = clock();

	cout << "Model loading success!" << endl
		<< "Vertices: " << vertices.size() << endl
		<< "Faces: " << faces.size() << endl
		<< "Use time: " << t2 - t1 << " ms" << endl;
}
void Mesh::loadMTL(const string& path) {
	ifstream mtl(path);
	if (!mtl.is_open()) {
		cout << "Cannot find .mtl file!" << endl;
		exit(1);
	}
	string type, title;
	Material* item = &materials[0];
	while (mtl >> type) {
		if (type == "newmtl") {
			mtl >> title;
			for (Material& mt : materials) {
				if (title == mt.name) {
					item = &mt;
				}
			}
		}
		else if (type == "Kd") {
			mtl >> item->Kd.x >> item->Kd.y >> item->Kd.z;
		}
		else if (type == "Ks") {
			mtl >> item->Ks.x >> item->Ks.y >> item->Ks.z;
		}
		else if (type == "Ns") {
			mtl >> item->Ns;
		}
		else if (type == "Ni") {
			mtl >> item->Ni;
		}
		else if (type == "map_Kd") {
			string texture_name;
			mtl >> texture_name;
			Texture texture(MODEL_PATH + string(MODEL_NAME) + "/" + texture_name);
			item->have_KdTexture = true;
			item->KdTexture = texture;
		}
	}
	mtl.close();
}
void Mesh::loadOBJ(const string& path) {
	ifstream obj(path);
	if (!obj.is_open()) {
		cout << "Cannot find .obj file!" << endl;
		exit(1);
	}
	string type, title;

	int mtlIdx = 0; //记录当前使用的材质索引号
	while (obj >> type) {

		// 新物体
		if (type == "usemtl") {
			string name;
			obj >> name;
			findOrAddMaterial(name, mtlIdx);
		}
		// 顶点
		else if (type == "v") {
			Vertex v;
			obj >> v.position.x >> v.position.y >> v.position.z;
			vertices.push_back(v);
		}
		// texcoords
		else if (type == "vt") {
			vec2 vt;
			obj >> vt.x >> vt.y;
			textures.push_back(vt);
		}
		//normal, 如果obj文件中有法向量，可以直接读入
		else if (type == "vn") {
			vec3 vn;
			obj >> vn.x >> vn.y >> vn.z;
			normals.push_back(vn);
		}
		// 面的索引
		if (type == "f") {
			Face face;
			char ch;
			int cnt = 0;
			while (ch = obj.get() && cnt < 3) {
				if (ch == '\n' || ch == EOF)break;
				int vi = 0, ti = 0, ni = 0;
				int count_v = 0, count_t = 0, count_n = 0;
				string str;
				obj >> str;
				count_v = str.find_first_of('/');
				count_t = str.find_last_of('/') - str.find_first_of('/') - 1;
				count_n = str.size() - str.find_last_of('/') - 1;
				if(count_v > 0)vi = stoi(str.substr(0, count_v));
				if(count_t > 0)ti = stoi(str.substr(str.find_first_of('/') + 1, count_t));
				if(count_n > 0)ni = stoi(str.substr(str.find_last_of('/') + 1, count_n));
				if (materials.size() == 0) {
					cout << "Error: Item's name must have been read!" << endl;
					exit(1);
				}

				face.mtlIdx = mtlIdx;
				
				face.vertexIdx.push_back(vi - 1);
				face.textureIdx.push_back(ti - 1);
				face.normalIdx.push_back(ni - 1);
				cnt++;
			}	
			faces.push_back(face);
		}
		else {
			getline(obj, type);
		}
	}
	obj.close();
}

vec3 Mesh::randFaceColor() {

	float r = float(1.0 * rand() / RAND_MAX);
	float g = float(1.0 * rand() / RAND_MAX);
	float b = float(1.0 * rand() / RAND_MAX);
	vec3 color = { r, g, b };
	return color;
}

//计算面片和整体的包围盒,补充面片信息
void Mesh::faceProcess() {
	int cnt = 0;
//#pragma omp parallel for
	for (Face& face : faces) {
		face.idx = cnt++;
//#pragma omp parallel for
		for (int& idx : face.vertexIdx) {			
			face.aabb.refresh_AABB(vertices[idx].position);
			full_aabb.refresh_AABB(vertices[idx].position);
		}	
		vec3& pt1 = vertices[face.vertexIdx[0]].position, &pt2 =vertices[face.vertexIdx[1]].position
			, &pt3 = vertices[face.vertexIdx[2]].position;
		face.origin = pt1;
		face.dx = pt2 - pt1;
		face.dy = pt3 - pt1;
		face.normal = normalize(cross(face.dx, face.dy));	//normal必须是标准化的
		face.central = (pt1 + pt2 + pt3) / 3.f;
		face.material = &materials[face.mtlIdx];
		vec2 tt1, tt2, tt3;
		if (!textures.empty()) {
			tt1 = textures[face.textureIdx[0]];
			tt2 = textures[face.textureIdx[1]];
			tt3 = textures[face.textureIdx[2]];
			face.uv = tt1;
			face.uv_dx = tt2 - tt1;
			face.uv_dy = tt3 - tt1;
		}
		else {
			face.uv = vec2(0, 0);
			face.uv_dx = vec2(0, 0);
			face.uv_dy = vec2(0, 0);
		}
		//face.normal = normalize(normals[face.vertexIdx[0]]);
		//if (fabs(dot(mnormal - face.normal, mnormal - face.normal)) > 0.001) {
		//	cout << "normal calculate wrong!" << endl;
		//	exit(1);
		//}
		for (int i = 0; i < 3; i++)face.points[i] = vertices[face.vertexIdx[i]].position;
	}
}

bool Face::intersect(Ray& ray, vec3& point)
{
	float tmp = dot(normal, ray.direction);

	if (fabs(tmp) <= D_EPS) return false;
	//if(DoubleCompare(tmp,0)>0&&ray.source!=SOURCE::TRANSMISSON) return false;

	vec3 v = origin - ray.origin;

	vec3 tmp1 = cross(dy, ray.direction);
	double s = -dot(tmp1, v) / dot(tmp1, dx);

	vec3 tmp2 = cross(dx, ray.direction);
	double t = -dot(tmp2, v) / dot(tmp2, dy);

	double far = dot(normal, v) / dot(normal, ray.direction);

	if (s >= -D_EPS && t >= -D_EPS
		&& s + t - 1 <= D_EPS && ray.isWithinBounds(far))
	{
		point = ray.getPoint(far);
		//ray.tMax = bestTime; 
		return true;
	}

	return false;
}

//如果是新材料，则添加到materials的末尾；如果是旧材料，则找到索引号
bool Mesh::findOrAddMaterial(string& name, int& mtlIdx) {
	for (int i = 0; i < materials.size(); i++) {
		if (materials[i].name == name) {
			mtlIdx = i;
			return true;
		}
	}
	mtlIdx = materials.size();
	Material material;
	material.name = name;
	materials.push_back(material);
	return false;
}
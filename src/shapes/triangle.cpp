#include "triangle.h"

RAINBOW_NAMESPACE_BEGIN

TriangleMesh::TriangleMesh(const Transform * ObjectToWorld,
	const int & _VertexNum, const int & _TriangleNum,
	const std::vector<Point3f>& _Position, const std::vector<int>& _VertexIndices, const Normal3f * _n = nullptr) :
	TriangleNum(_TriangleNum), VertexNum(_VertexNum), VertexIndices(_VertexIndices) {
	
	Position.resize(VertexNum);
	for (int i = 0; i < VertexNum; i++)
		Position[i] = (*ObjectToWorld)(_Position[i]);

	if (_n) {
		n.reset(new Normal3f[TriangleNum]);
		for (int i = 0; i < TriangleNum; i++)
			n[i] = (*ObjectToWorld)(_n[i]);
	}
}

Triangle::Triangle(const std::shared_ptr<TriangleMesh>& _mesh, int _triNumber):
	mesh(_mesh),triNumber(_triNumber){
	index = &(mesh->VertexIndices[3 * triNumber]);
	//std::cout << index[0] << ' ' << index[1] << ' ' << index[2] << std::endl;
}

Bounds3f Triangle::ObjectBounds() const {
	const Point3f &p0 = mesh->Position[index[0]];
	const Point3f &p1 = mesh->Position[index[1]];
	const Point3f &p2 = mesh->Position[index[2]];
	const Transform &T = *WorldToObject;
	return Union(Bounds3f(T(p0), T(p1)), T(p2));
}

Bounds3f Triangle::WorldBounds() const {
	const Point3f &p0 = mesh->Position[index[0]];
	const Point3f &p1 = mesh->Position[index[1]];
	const Point3f &p2 = mesh->Position[index[2]];
	return Union(Bounds3f(p0, p1), p2);
}

bool Triangle::Intersect(const Ray & ray, Float * tHit, SurfaceInteraction* inter) const {
	/* Get Triangle's Vertices p0, p1, p2*/
	const Point3f &p0 = mesh->Position[index[0]];
	const Point3f &p1 = mesh->Position[index[1]];
	const Point3f &p2 = mesh->Position[index[2]];

	Point3f tp0(p0), tp1(p1), tp2(p2);
	tp0 -= ray.o;
	tp1 -= ray.o;
	tp2 -= ray.o;

	int kz = MaxDimension(Abs(ray.d));
	int kx = kz + 1; if (kx == 3) kx = 0;
	int ky = kx + 1; if (ky == 3) ky = 0;
	Vector3f d = Permute(ray.d, kx, ky, kz);
	tp0 = Permute(tp0, kx, ky, kz);
	tp1 = Permute(tp1, kx, ky, kz);
	tp2 = Permute(tp2, kx, ky, kz);

	Float Sx = -d.x / d.z;
	Float Sy = -d.y / d.z;
	Float Sz = 1 / d.z;
	tp0.x += Sx * tp0.z;
	tp0.y += Sy * tp0.z;
	tp1.x += Sx * tp1.z;
	tp1.y += Sy * tp1.z;
	tp2.x += Sx * tp2.z;
	tp2.y += Sy * tp2.z;

	Float e0 = tp1.x*tp2.y - tp2.x*tp1.y;
	Float e1 = tp2.x*tp0.y - tp0.x*tp2.y;
	Float e2 = tp0.x*tp1.y - tp1.x*tp0.y;

	if ((e0 > 0 || e1 > 0 || e2 > 0) && (e0 < 0 || e1 < 0 || e2 < 0))
		return false;
	
	Float det = e0 + e1 + e2;
	if (det == 0) return false;

	tp0.z *= Sz;
	tp1.z *= Sz;
	tp2.z *= Sz;
	Float sum = e0 * tp0.z + e1 * tp1.z + e2 * tp2.z;
	if (det > 0 && (sum <= 0 || sum > ray.tMax * det))
		return false;
	else if (det < 0 && (sum >= 0 || sum < ray.tMax *det))
		return false;
	
	Float invDet = 1 / det;
	Float b0 = e0 * invDet;
	Float b1 = e1 * invDet;
	Float b2 = e2 * invDet;
	Float t = sum * invDet;

	*tHit = sum * invDet;
	Point3f pHit = ray(*tHit);
	Normal3f nHit;
	if (mesh->n) nHit = mesh->n[triNumber];
	else {
		nHit = static_cast<Normal3f>(Cross(p1 - p0, p2 - p0));
		nHit = -FaceForward(nHit, ray.d);
	}
    nHit = Normalize(nHit);
	*inter = SurfaceInteraction(pHit, nHit, -ray.d, this);

	return true;
}

bool Triangle::IntersectP(const Ray & ray) const {
	const Point3f &p0 = mesh->Position[index[0]];
	const Point3f &p1 = mesh->Position[index[1]];
	const Point3f &p2 = mesh->Position[index[2]];

	Point3f tp0(p0), tp1(p1), tp2(p2);
	tp0 -= ray.o;
	tp1 -= ray.o;
	tp2 -= ray.o;

	int kz = MaxDimension(Abs(ray.d));
	int kx = kz + 1; if (kx == 3) kx = 0;
	int ky = kx + 1; if (ky == 3) ky = 0;
	Vector3f d = Permute(ray.d, kx, ky, kz);
	tp0 = Permute(tp0, kx, ky, kz);
	tp1 = Permute(tp1, kx, ky, kz);
	tp2 = Permute(tp2, kx, ky, kz);

	Float Sx = -d.x / d.z;
	Float Sy = -d.y / d.z;
	Float Sz = 1 / d.z;
	tp0.x += Sx * tp0.z;
	tp0.y += Sy * tp0.z;
	tp1.x += Sx * tp1.z;
	tp1.y += Sy * tp1.z;
	tp2.x += Sx * tp2.z;
	tp2.y += Sy * tp2.z;

	Float e0 = tp1.x*tp2.y - tp2.x*tp1.y;
	Float e1 = tp2.x*tp0.y - tp0.x*tp2.y;
	Float e2 = tp0.x*tp1.y - tp1.x*tp0.y;

	if ((e0 > 0 || e1 > 0 || e2 > 0) && (e0 < 0 || e1 < 0 || e2 < 0))
		return false;

	Float det = e0 + e1 + e2;
	if (det == 0) return false;

	Float sum = (e0 * tp0.z + e1 * tp1.z + e2 * tp2.z) * Sz;
	if (det > 0 && (sum <= 0 || sum > ray.tMax * det))
		return false;
	else if (det < 0 && (sum >= 0 || sum < ray.tMax *det))
		return false;
	return true;
}

Interaction Triangle::Sample(const Point3f &p, const Point2f & sample, Float * pdf) const {
	Interaction inter;

	Point2f q = UniformSampleTriangle(sample);
	const Point3f &p0 = mesh->Position[index[0]];
	const Point3f &p1 = mesh->Position[index[1]];
	const Point3f &p2 = mesh->Position[index[2]];
	inter.p = q[0] * p0 + q[1] * p1 + (1 - q[0] - q[1]) * p2;

	inter.n = Normalize(Normal3f(Cross(p1 - p0, p2 - p0)));
	inter.n = FaceForward(inter.n, p - inter.p);

	if (mesh->n) {
		Normal3f ns(q[0] * mesh->n[index[0]] + q[1] * mesh->n[index[1]] +
			(1 - q[0] - q[1]) * mesh->n[index[2]]);
		inter.n = FaceForward(inter.n, ns);
	}

	*pdf = 1 / Area();
	return inter;
}

Float Triangle::Area() const {
	const Point3f &p0 = mesh->Position[index[0]];
	const Point3f &p1 = mesh->Position[index[1]];
	const Point3f &p2 = mesh->Position[index[2]];
 	return static_cast<Float>(0.5) * Cross(p1-p0,p2-p0).Length();
}

int ParseOBJVertex(const std::string& s) {
	std::string ret;
	for (char i : s) {
		if (i == '/') break;
		else ret += i;
	}
	return toInteger(ret);
}

void ParseWavefrontOBJ(const std::string& name, int* VertexNum, int* TriangleNum,
	std::vector<Point3f>* Position, std::vector<int>* Indices) {
	
	filesystem::path filename = getFileResolver()->resolve(name);
	std::ifstream is(filename.str());
	Assert(is.fail() == false, "Parsing OBJ File " + name + " fail!");
	std::vector<std::string> v;
	std::string line_str;
	while (std::getline(is, line_str)) {
		std::istringstream line(line_str);
		std::string prefix;
		line >> prefix;

		if (prefix == "v") {
			Point3f p;
			line >> p.x >> p.y >> p.z;
			(*VertexNum)++;
			Position->push_back(p);
		} else if (prefix == "f") {
			std::string v1, v2, v3, v4;
			line >> v1 >> v2 >> v3 >> v4;
			Assert(v4.empty() == true, "Quad?");
			(*TriangleNum)++;
			Indices->push_back(ParseOBJVertex(v1) - 1);
			Indices->push_back(ParseOBJVertex(v2) - 1);
			Indices->push_back(ParseOBJVertex(v3) - 1);
		}		
	}
	/*
	std::cout << filename << std::endl;
	for (const auto& a : *Indices) {
		std::cout << a << std::endl;
	}
	puts("");*/
}

std::vector<std::shared_ptr<Shape>> CreateWavefrontOBJ(const Transform* o2w, const Transform* w2o, 
	PropertyList & list) {
	
	std::string name = list.getString("filename");
	int VertexNum = 0, TriangleNum = 0;
	std::vector<Point3f> Position;
	std::vector<int> Indices;
	ParseWavefrontOBJ(name, &VertexNum, &TriangleNum, &Position, &Indices);

	std::shared_ptr<TriangleMesh> mesh =
		std::make_shared<TriangleMesh>(o2w, VertexNum, TriangleNum, Position, Indices);
	std::vector<std::shared_ptr<Shape>> tris;
	tris.reserve(TriangleNum);
	for (int i = 0; i < mesh->TriangleNum; i++) {
		tris.push_back(std::make_shared<Triangle>(mesh, i));
	}

	return tris;
}

RAINBOW_NAMESPACE_END

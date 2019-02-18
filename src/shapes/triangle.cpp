#include "triangle.h"
#include "meshloaders/wavefront.h"

RAINBOW_NAMESPACE_BEGIN

TriangleMesh::TriangleMesh(const Transform * ObjectToWorld, const int & m_VertexNum, const int & m_TriangleNum, 
    const std::vector<Point3f>& m_Position, const std::vector<int>& m_VertexIndices, 
    const std::vector<Normal3f>& m_Normal):
    TriangleNum(m_TriangleNum), VertexNum(m_VertexNum), VertexIndices(m_VertexIndices) {
    
    Position.resize(VertexNum);
    for (int i = 0; i < VertexNum; i++)
        Position[i] = (*ObjectToWorld)(m_Position[i]);
    
    Normal.resize(TriangleNum);
    for (int i = 0; i < TriangleNum; i++)
        Normal[i] = (*ObjectToWorld)(m_Normal[i]);
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

bool Triangle::IntersectP(const Ray & ray, Float * tHit, SurfaceInteraction* inter) const {
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
	//Float t = sum * invDet;

	*tHit = sum * invDet;
	Point3f pHit = ray(*tHit);
	Normal3f nHit;

    nHit = mesh->Normal[triNumber];

    // Compute error bounds for triangle intersection
    Float xAbsSum =
        (std::abs(b0 * p0.x) + std::abs(b1 * p1.x) + std::abs(b2 * p2.x));
    Float yAbsSum =
        (std::abs(b0 * p0.y) + std::abs(b1 * p1.y) + std::abs(b2 * p2.y));
    Float zAbsSum =
        (std::abs(b0 * p0.z) + std::abs(b1 * p1.z) + std::abs(b2 * p2.z));
    Vector3f pError = gamma(7) * Vector3f(xAbsSum, yAbsSum, zAbsSum);

    nHit = Normalize(nHit);
	*inter = SurfaceInteraction(pHit, pError, nHit, -ray.d, this);

	return true;
}

bool Triangle::Intersect(const Ray & ray) const {
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

	//if (!mesh->Normal.empty()) {
	//	Normal3f ns(q[0] * mesh->Normal[index[0]] + q[1] * mesh->Normal[index[1]] +
	//		(1 - q[0] - q[1]) * mesh->n[index[2]]);
	//	inter.n = FaceForward(inter.n, ns);
	//}

    Point3f pAbsSum =
        Abs(q[0] * p0) + Abs(q[1] * p1) + Abs((1 - q[0] - q[1]) * p2);
    inter.pError = gamma(6) * Vector3f(pAbsSum.x, pAbsSum.y, pAbsSum.z);

	*pdf = 1 / Area();
	return inter;
}

Float Triangle::Area() const {
	const Point3f &p0 = mesh->Position[index[0]];
	const Point3f &p1 = mesh->Position[index[1]];
	const Point3f &p2 = mesh->Position[index[2]];
 	return static_cast<Float>(0.5) * Cross(p1-p0,p2-p0).Length();
}

std::vector<std::shared_ptr<Triangle>> CreateWavefrontOBJ(
    const Transform* o2w, 
    const Transform* w2o, 
	PropertyList & list) {
	
	std::string name = list.getString("filename");
	int VertexNum = 0, TriangleNum = 0;
	std::vector<Point3f> Position;
	std::vector<int> Indices;
    std::vector<Normal3f> Normal;
    ParseWavefrontOBJ(name, &VertexNum, &TriangleNum, &Position, &Indices, &Normal);

    std::shared_ptr<TriangleMesh> mesh =
        std::make_shared<TriangleMesh>(o2w, VertexNum, TriangleNum, Position, Indices, Normal);
	std::vector<std::shared_ptr<Triangle>> tris;
	tris.resize(TriangleNum);
	for (int i = 0; i < mesh->TriangleNum; i++) {
        tris[i] = std::make_shared<Triangle>(mesh, i);
	}

	return tris;
}

std::vector<std::shared_ptr<Triangle>> CreateRectangle(
    const Transform* o2w, 
    const Transform* w2o, 
    PropertyList &list) {

    const int VertexNum = 4;
    const int TriangleNum = 2;
    std::vector<Point3f> Position(VertexNum);
    std::vector<int> Indices(3 * TriangleNum);
    std::vector<Normal3f> Normal(TriangleNum);

    Position[0] = Point3f(-1, -1, 0);
    Position[1] = Point3f(1, -1, 0);
    Position[2] = Point3f(1, 1, 0);
    Position[3] = Point3f(-1, 1, 0);

    Indices[0] = 0;
    Indices[1] = 1;
    Indices[2] = 2;
    Indices[3] = 2;
    Indices[4] = 3;
    Indices[5] = 0;

    Normal[0] = Normal[1] = Normal3f(0, 0, 1);

    std::shared_ptr<TriangleMesh> mesh =
        std::make_shared<TriangleMesh>(o2w, VertexNum, TriangleNum, Position, Indices, Normal);

    std::vector<std::shared_ptr<Triangle>> tris;
    tris.resize(TriangleNum);
    for (int i = 0; i < mesh->TriangleNum; i++) {
        tris[i] = std::make_shared<Triangle>(mesh, i);
    }

    return tris;
}

static Float CubeData_vertexPositions[][3] = { { 1, -1, -1 },{ 1, -1, 1 },{ -1, -1, 1 },{ -1, -1, -1 },{ 1, 1, -1 },{ -1, 1, -1 },{ -1, 1, 1 },{ 1, 1, 1 },{ 1, -1, -1 },{ 1, 1, -1 },{ 1, 1, 1 },{ 1, -1, 1 },{ 1, -1, 1 },{ 1, 1, 1 },{ -1, 1, 1 },{ -1, -1, 1 },{ -1, -1, 1 },{ -1, 1, 1 },{ -1, 1, -1 },{ -1, -1, -1 },{ 1, 1, -1 },{ 1, -1, -1 },{ -1, -1, -1 },{ -1, 1, -1 } };
//static Float CubeData_vertexNormals[][3] = { { 0, -1, 0 },{ 0, -1, 0 },{ 0, -1, 0 },{ 0, -1, 0 },{ 0, 1, 0 },{ 0, 1, 0 },{ 0, 1, 0 },{ 0, 1, 0 },{ 1, 0, 0 },{ 1, 0, 0 },{ 1, 0, 0 },{ 1, 0, 0 },{ 0, 0, 1 },{ 0, 0, 1 },{ 0, 0, 1 },{ 0, 0, 1 },{ -1, 0, 0 },{ -1, 0, 0 },{ -1, 0, 0 },{ -1, 0, 0 },{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 } };
static uint32_t CubeData_triangles[][3] = { { 0, 1, 2 },{ 3, 0, 2 },{ 4, 5, 6 },{ 7, 4, 6 },{ 8, 9, 10 },{ 11, 8, 10 },{ 12, 13, 14 },{ 15, 12, 14 },{ 16, 17, 18 },{ 19, 16, 18 },{ 20, 21, 22 },{ 23, 20, 22 } };

std::vector<std::shared_ptr<Triangle>> CreateCube(
    const Transform* o2w, 
    const Transform* w2o, 
    PropertyList &list) {

    const int VertexNum = 24;
    const int TriangleNum = 12;
    std::vector<Point3f> Position(VertexNum);
    std::vector<int> Indices(3 * TriangleNum);
    std::vector<Normal3f> Normal(TriangleNum);

    for (int i = 0; i < VertexNum; i++)
        Position[i] = Point3f(CubeData_vertexPositions[i][0], 
            CubeData_vertexPositions[i][1], 
            CubeData_vertexPositions[i][2]);
    for (int i = 0; i < TriangleNum; i++) {
        int id1 = CubeData_triangles[i][0];
        int id2 = CubeData_triangles[i][1];
        int id3 = CubeData_triangles[i][2];
        Indices[i * 3 + 0] = id1;
        Indices[i * 3 + 1] = id2;
        Indices[i * 3 + 2] = id3;
        Normal3f n(Cross(Position[id2] - Position[id1], Position[id3] - Position[id1]));
        //cout << n << endl;
        Assert(n.SquareLength() != 0, "Length of Normal is Zero!");
        Normal[i] = Normalize(n);
    }

    std::shared_ptr<TriangleMesh> mesh =
        std::make_shared<TriangleMesh>(o2w, VertexNum, TriangleNum, Position, Indices, Normal);

    std::vector<std::shared_ptr<Triangle>> tris;
    tris.resize(TriangleNum);
    for (int i = 0; i < mesh->TriangleNum; i++) {
        tris[i] = std::make_shared<Triangle>(mesh, i);
    }

    return tris;

}

RAINBOW_NAMESPACE_END
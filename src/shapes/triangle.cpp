#include "triangle.h"
#include "core/frame.h"
#include "meshloaders/wavefront.h"


RAINBOW_NAMESPACE_BEGIN

TriangleMesh::TriangleMesh(
    const Transform*             ObjectToWorld, 
    const int&                   vertexNum, 
    const int&                   triangleNum, 
    const std::vector<Point3f>&  vertices, 
    const std::vector<Normal3f>& normals,
    const std::vector<Point2f>&  texcoords,
    const std::vector<Index>&    indices) : 
    m_triangleNum(triangleNum), m_vertexNum(vertexNum), 
    m_texcoords(texcoords), m_indices(indices) 
{    
    m_vertices.resize(m_vertexNum);
    for (int i = 0; i < m_vertexNum; i++)
        m_vertices[i] = (*ObjectToWorld)(vertices[i]);
    
    m_normals.resize(normals.size());
    for (int i = 0; i < normals.size(); i++)
        m_normals[i] = (*ObjectToWorld)(normals[i]);    
}

Triangle::Triangle(
    const std::shared_ptr<TriangleMesh>& mesh, 
    const int&                           triNumber):
	m_mesh(mesh), m_triNumber(triNumber)
{
	m_index = &(mesh->m_indices[3 * triNumber]);	
}

Bounds3f Triangle::ObjectBounds() const {
	const Point3f &p0 = m_mesh->m_vertices[m_index[0].vertexIndex];
	const Point3f &p1 = m_mesh->m_vertices[m_index[1].vertexIndex];
	const Point3f &p2 = m_mesh->m_vertices[m_index[2].vertexIndex];
	const Transform &T = *WorldToObject;
	return Union(Bounds3f(T(p0), T(p1)), T(p2));
}

Bounds3f Triangle::WorldBounds() const {
    const Point3f &p0 = m_mesh->m_vertices[m_index[0].vertexIndex];
    const Point3f &p1 = m_mesh->m_vertices[m_index[1].vertexIndex];
    const Point3f &p2 = m_mesh->m_vertices[m_index[2].vertexIndex];
	return Union(Bounds3f(p0, p1), p2);
}

bool Triangle::IntersectP(const Ray & ray, Float * tHit, SurfaceInteraction* inter) const {
	/* Get Triangle's Vertices p0, p1, p2*/
    const Point3f &p0 = m_mesh->m_vertices[m_index[0].vertexIndex];
    const Point3f &p1 = m_mesh->m_vertices[m_index[1].vertexIndex];
    const Point3f &p2 = m_mesh->m_vertices[m_index[2].vertexIndex];

    // Transform the ray origin at the origin of the coordinate system
	Point3f tp0(p0), tp1(p1), tp2(p2);
	tp0 -= ray.o;
	tp1 -= ray.o;
	tp2 -= ray.o;

    // Make the ray's z axis is non-zero and largest
    // Permute the tp0, tp1, tp2 as it, too
	int kz = MaxDimension(Abs(ray.d));
	int kx = kz + 1; if (kx == 3) kx = 0;
	int ky = kx + 1; if (ky == 3) ky = 0;
	Vector3f d = Permute(ray.d, kx, ky, kz);
	tp0 = Permute(tp0, kx, ky, kz);
	tp1 = Permute(tp1, kx, ky, kz);
	tp2 = Permute(tp2, kx, ky, kz);

    // Transform the ray to (0, 0, 1, 0)
	Float Sx = -d.x / d.z;
	Float Sy = -d.y / d.z;
	Float Sz =   1. / d.z;
	tp0.x += Sx * tp0.z;
	tp0.y += Sy * tp0.z;
	tp1.x += Sx * tp1.z;
	tp1.y += Sy * tp1.z;
	tp2.x += Sx * tp2.z;
	tp2.y += Sy * tp2.z;

    // Edge function
    // Left-hand 
	Float e0 = tp1.x * tp2.y - tp2.x * tp1.y;
	Float e1 = tp2.x * tp0.y - tp0.x * tp2.y;
	Float e2 = tp0.x * tp1.y - tp1.x * tp0.y;

    // Different side
	if ((e0 > 0 || e1 > 0 || e2 > 0) && (e0 < 0 || e1 < 0 || e2 < 0))
		return false;

    // Edge-on
	Float det = e0 + e1 + e2;
	if (det == 0) return false;

    // Transform the ray to (0, 0, 1, 0)
	tp0.z *= Sz;
	tp1.z *= Sz;
	tp2.z *= Sz;

    // Out of [0, tMax] range
	Float sum = e0 * tp0.z + e1 * tp1.z + e2 * tp2.z;
	if (det > 0 && (sum <= 0 || sum > ray.tMax * det))
		return false;
	else if (det < 0 && (sum >= 0 || sum < ray.tMax *det))
		return false;

    // Barycentric coordinate
	Float invDet = 1 / det;
	Float b0 = e0 * invDet;
	Float b1 = e1 * invDet;
	Float b2 = e2 * invDet;
	Float t = sum * invDet;

    // Get vertices' uv coordinate
    Point2f uv[3];
    GetUV(uv);

    Vector3f dpdu, dpdv;
    Vector3f dp02 = p0 - p2, dp12 = p1 - p2;
    Vector2f duv02 = uv[0] - uv[2], duv12 = uv[1] - uv[2];
    Float determinant = (duv02[0] * duv12[1]) * (duv02[1] - duv12[0]);
    if (determinant == 0) {
        CoordinateSystem(Normal3f(Cross(p1 - p0, p2 - p0)), &dpdu, &dpdv);
    }
    else {
        Float inv = 1. / determinant;
        dpdu = ( duv12[1] * dp02 - duv02[1] * dp12) * inv;
        dpdv = (-duv12[0] * dp02 + duv02[0] * dp12) * inv;
    }
        
    Point3f pHit = b0 * p0 + b1 * p1 + b2 * p2;
    Point2f uvHit = b0 * uv[0] + b1 * uv[1] + b2 * uv[2];
	
    *inter = 
        SurfaceInteraction(pHit, uvHit, -ray.d, dpdu, dpdv, Normal3f(0, 0, 0), Normal3f(0, 0, 0), this);
    

    if (m_index->normalIndex != -1) {
        Normal3f ns = Normalize(
            b0 * m_mesh->m_normals[m_index[0].normalIndex] +
            b1 * m_mesh->m_normals[m_index[1].normalIndex] +
            b2 * m_mesh->m_normals[m_index[2].normalIndex]);
        inter->shading.n = ns;
        inter->n = FaceForward(inter->n, inter->shading.n);
        //inter->n = inter->shading.n;
    }
    else {
        inter->n = inter->shading.n = Normal3f(Normalize(Cross(dp02, dp12)));        
    }

    *tHit = t;
	return true;
}

bool Triangle::Intersect(const Ray & ray) const {
    /* Get Triangle's Vertices p0, p1, p2*/
    const Point3f &p0 = m_mesh->m_vertices[m_index[0].vertexIndex];
    const Point3f &p1 = m_mesh->m_vertices[m_index[1].vertexIndex];
    const Point3f &p2 = m_mesh->m_vertices[m_index[2].vertexIndex];

    // Transform the ray origin at the origin of the coordinate system
    Point3f tp0(p0), tp1(p1), tp2(p2);
    tp0 -= ray.o;
    tp1 -= ray.o;
    tp2 -= ray.o;

    // Make the ray's z axis is non-zero and largest
    // Permute the tp0, tp1, tp2 as it, too
    int kz = MaxDimension(Abs(ray.d));
    int kx = kz + 1; if (kx == 3) kx = 0;
    int ky = kx + 1; if (ky == 3) ky = 0;
    Vector3f d = Permute(ray.d, kx, ky, kz);
    tp0 = Permute(tp0, kx, ky, kz);
    tp1 = Permute(tp1, kx, ky, kz);
    tp2 = Permute(tp2, kx, ky, kz);

    // Transform the ray to (0, 0, 1, 0)
    Float Sx = -d.x / d.z;
    Float Sy = -d.y / d.z;
    Float Sz = 1. / d.z;
    tp0.x += Sx * tp0.z;
    tp0.y += Sy * tp0.z;
    tp1.x += Sx * tp1.z;
    tp1.y += Sy * tp1.z;
    tp2.x += Sx * tp2.z;
    tp2.y += Sy * tp2.z;

    // Edge function
    // Left-hand 
    Float e0 = tp1.x * tp2.y - tp2.x * tp1.y;
    Float e1 = tp2.x * tp0.y - tp0.x * tp2.y;
    Float e2 = tp0.x * tp1.y - tp1.x * tp0.y;

    // Different side
    if ((e0 > 0 || e1 > 0 || e2 > 0) && (e0 < 0 || e1 < 0 || e2 < 0))
        return false;

    // Edge-on
    Float det = e0 + e1 + e2;
    if (det == 0) return false;

    // Transform the ray to (0, 0, 1, 0)
    tp0.z *= Sz;
    tp1.z *= Sz;
    tp2.z *= Sz;

    // Out of [0, tMax] range
    Float sum = e0 * tp0.z + e1 * tp1.z + e2 * tp2.z;
    if (det > 0 && (sum <= 0 || sum > ray.tMax * det))
        return false;
    else if (det < 0 && (sum >= 0 || sum < ray.tMax *det))
        return false;
    return true;
}

Interaction Triangle::Sample(const Point3f &p, const Point2f & sample, Float * pdf) const {
	Interaction inter;

	Point2f q = UniformSampleTriangle(sample);
    const Point3f &p0 = m_mesh->m_vertices[m_index[0].vertexIndex];
    const Point3f &p1 = m_mesh->m_vertices[m_index[1].vertexIndex];
    const Point3f &p2 = m_mesh->m_vertices[m_index[2].vertexIndex];
	inter.p = q[0] * p0 + q[1] * p1 + (1 - q[0] - q[1]) * p2;

	inter.n = Normalize(Normal3f(Cross(p1 - p0, p2 - p0)));

	//if (!mesh->normals.empty()) {
	//	Normal3f ns(q[0] * mesh->normals[index[0]] + q[1] * mesh->normals[index[1]] +
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
    const Point3f &p0 = m_mesh->m_vertices[m_index[0].vertexIndex];
    const Point3f &p1 = m_mesh->m_vertices[m_index[1].vertexIndex];
    const Point3f &p2 = m_mesh->m_vertices[m_index[2].vertexIndex];
 	return static_cast<Float>(0.5) * Cross(p1-p0,p2-p0).Length();
}

std::vector<std::shared_ptr<Triangle>> CreateWavefrontOBJ(
    const Transform* o2w, 
    const Transform* w2o, 
    const PropertyList & list) {
	
	std::string filename = list.getString("filename");
	int vertexNum = 0, triangleNum = 0;
	std::vector<Point3f>  vertices;
    std::vector<Normal3f> normals;
    std::vector<Point2f>  texcoords;
	std::vector<Index>    indices;
    ParseWavefrontOBJ(
        filename, &vertexNum, &triangleNum,
        &vertices, &normals, &texcoords, &indices);

    std::shared_ptr<TriangleMesh> mesh =
        std::make_shared<TriangleMesh>(
            o2w, vertexNum, triangleNum, 
            vertices, normals, texcoords, indices);
	std::vector<std::shared_ptr<Triangle>> tris;
	tris.resize(triangleNum);
	for (int i = 0; i < mesh->m_triangleNum; i++) {
        tris[i] = std::make_shared<Triangle>(mesh, i);
	}

	return tris;
}

std::vector<std::shared_ptr<Triangle>> CreateRectangle(
    const Transform* o2w, 
    const Transform* w2o, 
    const PropertyList &list) {

    const int vertexNum = 4;
    const int triangleNum = 2;
    std::vector<Point3f>  vertices;  vertices.resize(vertexNum);
    std::vector<Index>    indices;   indices.resize(3 * triangleNum);
    std::vector<Normal3f> normals;   normals.resize(3 * triangleNum);
    std::vector<Point2f>  texcoords; texcoords.resize(3 * triangleNum);

    vertices[0] = Point3f(-1, -1, 0);
    vertices[1] = Point3f(1, -1, 0);
    vertices[2] = Point3f(1, 1, 0);
    vertices[3] = Point3f(-1, 1, 0);

    indices[0] = Index(0, 0, -1);
    indices[1] = Index(1, 0, -1);
    indices[2] = Index(2, 0, -1);
    indices[3] = Index(2, 0, -1);
    indices[4] = Index(3, 0, -1);
    indices[5] = Index(0, 0, -1);

    normals[0] = Normal3f(0, 0, 1);

    std::shared_ptr<TriangleMesh> mesh =
        std::make_shared<TriangleMesh>(
            o2w, vertexNum, triangleNum, 
            vertices, normals, texcoords, indices);

    std::vector<std::shared_ptr<Triangle>> tris;
    tris.resize(triangleNum);
    for (int i = 0; i < mesh->m_triangleNum; i++) {
        tris[i] = std::make_shared<Triangle>(mesh, i);
    }

    return tris;
}

static Float CubeData_vertexverticess[][3] = { { 1, -1, -1 },{ 1, -1, 1 },{ -1, -1, 1 },{ -1, -1, -1 },{ 1, 1, -1 },{ -1, 1, -1 },{ -1, 1, 1 },{ 1, 1, 1 },{ 1, -1, -1 },{ 1, 1, -1 },{ 1, 1, 1 },{ 1, -1, 1 },{ 1, -1, 1 },{ 1, 1, 1 },{ -1, 1, 1 },{ -1, -1, 1 },{ -1, -1, 1 },{ -1, 1, 1 },{ -1, 1, -1 },{ -1, -1, -1 },{ 1, 1, -1 },{ 1, -1, -1 },{ -1, -1, -1 },{ -1, 1, -1 } };
//static Float CubeData_vertexNormals[][3] = { { 0, -1, 0 },{ 0, -1, 0 },{ 0, -1, 0 },{ 0, -1, 0 },{ 0, 1, 0 },{ 0, 1, 0 },{ 0, 1, 0 },{ 0, 1, 0 },{ 1, 0, 0 },{ 1, 0, 0 },{ 1, 0, 0 },{ 1, 0, 0 },{ 0, 0, 1 },{ 0, 0, 1 },{ 0, 0, 1 },{ 0, 0, 1 },{ -1, 0, 0 },{ -1, 0, 0 },{ -1, 0, 0 },{ -1, 0, 0 },{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 } };
static uint32_t CubeData_triangles[][3] = { { 0, 1, 2 },{ 3, 0, 2 },{ 4, 5, 6 },{ 7, 4, 6 },{ 8, 9, 10 },{ 11, 8, 10 },{ 12, 13, 14 },{ 15, 12, 14 },{ 16, 17, 18 },{ 19, 16, 18 },{ 20, 21, 22 },{ 23, 20, 22 } };

std::vector<std::shared_ptr<Triangle>> CreateCube(
    const Transform* o2w, 
    const Transform* w2o, 
    const PropertyList &list) {

    const int vertexNum = 24;
    const int triangleNum = 12;
    std::vector<Point3f>  vertices;  vertices.resize(vertexNum);
    std::vector<Index>    indices;   indices.resize(3 * triangleNum);
    std::vector<Normal3f> normals;   normals.resize(3 * triangleNum);
    std::vector<Point2f>  texcoords; texcoords.resize(3 * triangleNum);

    for (int i = 0; i < vertexNum; i++)
        vertices[i] = Point3f(CubeData_vertexverticess[i][0], 
            CubeData_vertexverticess[i][1], 
            CubeData_vertexverticess[i][2]);
    for (int i = 0; i < triangleNum; i++) {
        int id1 = CubeData_triangles[i][0];
        int id2 = CubeData_triangles[i][1];
        int id3 = CubeData_triangles[i][2];
        indices[i * 3 + 0] = Index(id1, -1, -1);
        indices[i * 3 + 1] = Index(id2, -1, -1);
        indices[i * 3 + 2] = Index(id3, -1, -1);
        Normal3f n(Cross(vertices[id2] - vertices[id1], vertices[id3] - vertices[id1]));
        //cout << n << endl;
        Assert(n.SquareLength() != 0, "Length of normals is Zero!");
        normals[i] = Normalize(n);
    }

    std::shared_ptr<TriangleMesh> mesh =
        std::make_shared<TriangleMesh>(
            o2w, vertexNum, triangleNum,
            vertices, normals, texcoords, indices);

    std::vector<std::shared_ptr<Triangle>> tris;
    tris.resize(triangleNum);
    for (int i = 0; i < mesh->m_triangleNum; i++) {
        tris[i] = std::make_shared<Triangle>(mesh, i);
    }

    return tris;
}

std::shared_ptr<TriangleMesh>
CreateWavefrontOBJMesh(
    const Transform* o2w, 
    const Transform* w2o, 
    const PropertyList& list)
{
    std::string filename = list.getString("filename");
    int vertexNum = 0, triangleNum = 0;
    std::vector<Point3f>  vertices;
    std::vector<Normal3f> normals;
    std::vector<Point2f>  texcoords;
    std::vector<Index>    indices;
    ParseWavefrontOBJ(
        filename, &vertexNum, &triangleNum,
        &vertices, &normals, &texcoords, &indices);

    std::shared_ptr<TriangleMesh> mesh =
        std::make_shared<TriangleMesh>(
            o2w, vertexNum, triangleNum,
            vertices, normals, texcoords, indices);

    return mesh;
}

std::shared_ptr<TriangleMesh> 
CreateRectangleMesh(
    const Transform* o2w, 
    const Transform* w2o, 
    const PropertyList& list)
{
    const int vertexNum = 4;
    const int triangleNum = 2;
    std::vector<Point3f>  vertices;  vertices.resize(vertexNum);
    std::vector<Index>    indices;   indices.resize(3 * triangleNum);
    std::vector<Normal3f> normals;   normals.resize(3 * triangleNum);
    std::vector<Point2f>  texcoords; texcoords.resize(3 * triangleNum);

    vertices[0] = Point3f(-1, -1, 0);
    vertices[1] = Point3f(1, -1, 0);
    vertices[2] = Point3f(1, 1, 0);
    vertices[3] = Point3f(-1, 1, 0);

    indices[0] = Index(0, 0, -1);
    indices[1] = Index(1, 0, -1);
    indices[2] = Index(2, 0, -1);
    indices[3] = Index(2, 0, -1);
    indices[4] = Index(3, 0, -1);
    indices[5] = Index(0, 0, -1);

    normals[0] = Normal3f(0, 0, 1);

    std::shared_ptr<TriangleMesh> mesh =
        std::make_shared<TriangleMesh>(
            o2w, vertexNum, triangleNum,
            vertices, normals, texcoords, indices);

    return mesh;
}

std::shared_ptr<TriangleMesh> 
CreateCubeMesh(
    const Transform* o2w, 
    const Transform* w2o, 
    const PropertyList& list)
{
    const int vertexNum = 24;
    const int triangleNum = 12;
    std::vector<Point3f>  vertices;  vertices.resize(vertexNum);
    std::vector<Index>    indices;   indices.resize(3 * triangleNum);
    std::vector<Normal3f> normals;   normals.resize(3 * triangleNum);
    std::vector<Point2f>  texcoords; texcoords.resize(3 * triangleNum);

    for (int i = 0; i < vertexNum; i++)
        vertices[i] = Point3f(CubeData_vertexverticess[i][0],
            CubeData_vertexverticess[i][1],
            CubeData_vertexverticess[i][2]);
    for (int i = 0; i < triangleNum; i++) {
        int id1 = CubeData_triangles[i][0];
        int id2 = CubeData_triangles[i][1];
        int id3 = CubeData_triangles[i][2];
        indices[i * 3 + 0] = Index(id1, -1, -1);
        indices[i * 3 + 1] = Index(id2, -1, -1);
        indices[i * 3 + 2] = Index(id3, -1, -1);
        Normal3f n(Cross(vertices[id2] - vertices[id1], vertices[id3] - vertices[id1]));
        //cout << n << endl;
        Assert(n.SquareLength() != 0, "Length of normals is Zero!");
        normals[i] = Normalize(n);
    }

    std::shared_ptr<TriangleMesh> mesh =
        std::make_shared<TriangleMesh>(
            o2w, vertexNum, triangleNum,
            vertices, normals, texcoords, indices);

    return mesh;
}

std::shared_ptr<TriangleMesh> 
CreateSphereTriangleMesh(
    const Transform* o2w, 
    const Transform* w2o,
    const PropertyList& list)
{
    // setting
    Float radius = list.getFloat("radius", 1.);
    Point3f center(0, 0, 0);
    int subdiv = list.getInteger("subdiv", 50);
    int nLatitude = subdiv;
    int nLongitude = subdiv * 2;
    Float detLatitudeAngle = M_PI / nLatitude;
    Float detLongitudeAngle = 2 * M_PI / nLongitude;

    // container
    const int vertexNum = nLongitude * (nLatitude - 1) + 2;
    const int triangleNum = 2 * nLongitude + (nLatitude - 2)*nLongitude * 2;
    std::vector<Point3f>  vertices;  
    std::vector<Index>    indices;   
    std::vector<Normal3f> normals;   
    std::vector<Point2f>  texcoords; 

    // v  px py pz and 
    // vn nx ny nz
    Point3f top = Point3f(0., 0., radius) + center;
    vertices.push_back(top);
    normals.emplace_back(0., 0., 1.);
    for (int i = 1; i < nLatitude; i++) {
        Float theta = i * detLatitudeAngle;
        for (int j = 0; j < nLongitude; j++) {
            Float phi = j * detLongitudeAngle;
            Float x = sin(theta) * cos(phi);
            Float y = sin(theta) * sin(phi);
            Float z = cos(theta);
            Normal3f n = Normal3f(x, y, z);
            Point3f p = Point3f(x, y, z)* radius + center;
            vertices.push_back(p);
            normals.push_back(n);
        }
    }
    Point3f bottom = Point3f(0., 0., -radius) + center;
    vertices.push_back(bottom);
    normals.emplace_back(0., 0., -1.);

    // f v1//vn1 v2//vn2 v3//vn3
    // counterclockwise
    for (int i = 0; i < nLongitude; i++) {
        int a = i + 1, b = a + 1;
        if (i == nLongitude - 1) b = 1;
        indices.emplace_back(a, a, -1);
        indices.emplace_back(b, b, -1);
        indices.emplace_back(0, 0, -1);
    }

    for (int i = 2; i < nLatitude; i++) {
        for (int j = 0; j < nLongitude; j++) {
            int a = (i - 1)*nLongitude + j + 1, b = a + 1, c = a - nLongitude, d = c + 1;
            if (j == nLongitude - 1) b = (i - 1)*nLongitude + 1, d = b - nLongitude;
            indices.emplace_back(a, a, -1);
            indices.emplace_back(b, b, -1);
            indices.emplace_back(c, c, -1);

            indices.emplace_back(b, b, -1);
            indices.emplace_back(d, d, -1);
            indices.emplace_back(c, c, -1);
        }
    }

    int bottomIdx = nLongitude * (nLatitude - 1) + 1;
    for (int i = 0; i < nLongitude; i++) {
        int a = (nLatitude - 2)*nLongitude + i + 1, b = a + 1;
        if (i == nLongitude - 1) b = (nLatitude - 2)*nLongitude + 1;
        indices.emplace_back(a, a, -1);
        indices.emplace_back(bottomIdx, bottomIdx, -1);
        indices.emplace_back(b, b, -1);
    }

    // create mesh
    std::shared_ptr<TriangleMesh> mesh =
        std::make_shared<TriangleMesh>(
            o2w, vertexNum, triangleNum,
            vertices, normals, texcoords, indices);

    return mesh;
}

RAINBOW_NAMESPACE_END

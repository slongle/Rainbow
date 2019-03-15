#ifndef __TRIANGLE_H
#define __TRIANGLE_H

#include "../core/shape.h"

RAINBOW_NAMESPACE_BEGIN

struct Index{
    Index(
        const int& vertexIndex = 0,
        const int& normalIndex = 0,
        const int& texcoordIndex = 0)
    : vertexIndex(vertexIndex), normalIndex(normalIndex), texcoordIndex(texcoordIndex) {};

    int vertexIndex;
    int normalIndex;
    int texcoordIndex;
};

struct TriangleMesh{
    TriangleMesh(
        const Transform*             ObjectToWorld,
        const int&                   vertexNum,
        const int&                   triangleNum,
        const std::vector<Point3f>&  vertices,
        const std::vector<Normal3f>& normals,
        const std::vector<Point2f>&  texcoords,
        const std::vector<Index>&    indices);

	const int m_triangleNum, m_vertexNum;
	std::vector<Point3f>  m_vertices;
    std::vector<Normal3f> m_normals;
    std::vector<Point2f>  m_texcoords;
	std::vector<Index>    m_indices;
};

class Triangle :public Shape{
public:
	Triangle(
        const std::shared_ptr<TriangleMesh>& m_mesh, 
        const int&                           m_triNumber);

	Bounds3f ObjectBounds() const;
	Bounds3f WorldBounds() const;

	bool IntersectP(
        const Ray&          ray, 
        Float*              tHit, 
        SurfaceInteraction* inter) const;
	bool Intersect(
        const Ray & ray) const;

	Interaction Sample(
        const Point3f& p, 
        const Point2f& sample, 
        Float*         pdf) const;
	Float Area() const;	

private:
	std::shared_ptr<TriangleMesh> m_mesh;
	const Index*                  m_index = nullptr;
	const int                     m_triNumber = 0;

    void GetUV(Point2f* uv) const {
        if (m_index[0].texcoordIndex != -1) {
            uv[0] = m_mesh->m_texcoords[m_index[0].texcoordIndex];
            uv[1] = m_mesh->m_texcoords[m_index[1].texcoordIndex];
            uv[2] = m_mesh->m_texcoords[m_index[2].texcoordIndex];
        }
        else {
            uv[0] = Point2f(0, 0);
            uv[1] = Point2f(1, 0);
            uv[2] = Point2f(1, 1);
        }
    }
};

std::vector<std::shared_ptr<Triangle>> CreateWavefrontOBJ(
    const Transform* o2w, 
    const Transform* w2o, 
    PropertyList&    list);
std::vector<std::shared_ptr<Triangle>> CreateRectangle(
    const Transform* o2w, 
    const Transform* w2o, 
    PropertyList&    list);
std::vector<std::shared_ptr<Triangle>> CreateCube(
    const Transform* o2w, 
    const Transform* w2o, 
    PropertyList&    list);


RAINBOW_NAMESPACE_END

#endif // !__TRIANGLE_H

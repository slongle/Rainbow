#ifndef __TRIANGLE_H
#define __TRIANGLE_H

#include "../core/shape.h"

RAINBOW_NAMESPACE_BEGIN

struct TriangleMesh{
	TriangleMesh(const Transform *ObjectToWorld, 
		const int &m_VertexNum, const int &m_TriangleNum,
		const std::vector<Point3f>& m_Position, const std::vector<int>& m_VertexIndices, 
        const std::vector<Normal3f>& m_Normal);

	const int TriangleNum, VertexNum;
	std::vector<int> VertexIndices;
	std::vector<Point3f> Position;
    std::vector<Normal3f> Normal;
};

class Triangle :public Shape{
public:
	Triangle(const std::shared_ptr<TriangleMesh> & _mesh, int _triNumber);

	Bounds3f ObjectBounds() const;
	Bounds3f WorldBounds() const;

	bool Intersect(const Ray & ray, Float *tHit, SurfaceInteraction*inter) const;
	bool IntersectP(const Ray & ray) const;

	Interaction Sample(const Point3f &p, const Point2f& sample, Float* pdf) const;
	Float Area() const;	

private:
	std::shared_ptr<TriangleMesh> mesh;
	const int * index = nullptr;
	const int triNumber = 0;
};

std::vector<std::shared_ptr<Triangle>> CreateWavefrontOBJ(const Transform* o2w, const Transform* w2o, PropertyList &list);
std::vector<std::shared_ptr<Triangle>> CreateRectangle(const Transform* o2w, const Transform* w2o, PropertyList &list);
std::vector<std::shared_ptr<Triangle>> CreateCube(const Transform* o2w, const Transform* w2o, PropertyList &list);


RAINBOW_NAMESPACE_END

#endif // !__TRIANGLE_H

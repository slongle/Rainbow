#ifndef __TRIANGLE_H
#define __TRIANGLE_H

#include "../core/shape.h"

RAINBOW_NAMESPACE_BEGIN

struct TriangleMesh {
	TriangleMesh(const Transform &ObjectToWorld, const int &_nTriangles, const int &_nVertices,
		const int *_vertexIndices, const Point3f *_p, const Normal3f *_n);

	const int nTriangles, nVertices;
	std::vector<int> vertexIndices;
	std::unique_ptr<Point3f[]> p;
	std::unique_ptr<Normal3f[]> n;
};

class Triangle :public Shape{
public:
	Triangle(const Transform *_ObjectToWorld, const Transform * _WorldToObject, 
		const std::shared_ptr<TriangleMesh> & _mesh, int triNumber);

	Bounds3f ObjectBounds() const;
	Bounds3f WorldBounds() const;

	bool Intersect(const Ray & ray, Float *tHit, Interaction *inter) const;
	bool IntersectP(const Ray & ray) const;

	Float Area() const;

private:
	const std::shared_ptr<TriangleMesh> mesh;
	const int * index;
	const int triNumber;
};

RAINBOW_NAMESPACE_END

#endif // !__TRIANGLE_H

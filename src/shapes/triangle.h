#ifndef __TRIANGLE_H
#define __TRIANGLE_H

#include "../core/shape.h"

RAINBOW_NAMESPACE_BEGIN

struct TriangleMesh{
	TriangleMesh(const Transform *ObjectToWorld, 
		const int &_VertexNum, const int &_TriangleNum,
		const std::vector<Point3f>& _p, const std::vector<int>& _VertexIndices, const Normal3f *_n);

	const int TriangleNum, VertexNum;
	std::vector<int> VertexIndices;
	std::vector<Point3f> Position;
	std::unique_ptr<Normal3f[]> n;
};

class Triangle :public Shape{
public:
	Triangle() {}
	Triangle(const std::shared_ptr<TriangleMesh> & _mesh, int triNumber);

	Bounds3f ObjectBounds() const;
	Bounds3f WorldBounds() const;

	bool Intersect(const Ray & ray, Float *tHit, Interaction *inter) const;
	bool IntersectP(const Ray & ray) const;

	Float Area() const;

private:
	const std::shared_ptr<TriangleMesh> mesh;
	const int * index = nullptr;
	const int triNumber = 0;
};

std::vector<Triangle*>* CreateWavefrontOBJ(PropertyList &list);

RAINBOW_NAMESPACE_END

#endif // !__TRIANGLE_H

#ifndef __TRIANGLE_H
#define __TRIANGLE_H

#include "../core/shape.h"

RAINBOW_NAMESPACE_BEGIN

class TriangleMesh:public Shape{
public:
	TriangleMesh(const Transform *ObjectToWorld, 
		const int &_VertexNum, const int &_TriangleNum,
		const std::vector<Point3f>& _p, const std::vector<int>& _VertexIndices, const Normal3f *_n);

	Bounds3f ObjectBounds() const { Assert(false, "Invoke TriangleMesh's ObjectBounds() Function!"); }
	bool Intersect(const Ray & ray, Float *tHit, Interaction *inter) const 
		{ Assert(false, "Invoke TriangleMesh's Intersect() Function!"); }
	Float Area() const { Assert(false, "Invoke TriangleMesh's Area() Function!"); }
	
	EShapeType getShapeType() const { return EShapeMesh; }
	
	const int TriangleNum, VertexNum;
	std::vector<int> VertexIndices;
	std::vector<Point3f> Position;
	std::unique_ptr<Normal3f[]> n;
};

class Triangle :public Shape{
public:
	Triangle() {}
	Triangle(TriangleMesh* _mesh, int _triNumber);
	Triangle(const std::shared_ptr<TriangleMesh> & _mesh, int _triNumber);

	Bounds3f ObjectBounds() const;
	Bounds3f WorldBounds() const;

	bool Intersect(const Ray & ray, Float *tHit, Interaction *inter) const;
	bool IntersectP(const Ray & ray) const;

	Float Area() const;	

private:
	//const 
	//std::shared_ptr<TriangleMesh> mesh;
	TriangleMesh * mesh;
	const int * index = nullptr;
	const int triNumber = 0;
};

TriangleMesh CreateWavefrontOBJ(PropertyList &list);

RAINBOW_NAMESPACE_END

#endif // !__TRIANGLE_H

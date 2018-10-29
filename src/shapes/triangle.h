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
	bool Intersect(const Ray & ray, Float *tHit, SurfaceInteraction*inter) const 
		{ Assert(false, "Invoke TriangleMesh's Intersect() Function!"); }
	Interaction Sample(const Point3f &p, const Point2f& sample, Float* pdf)const {
		Assert(false, "No Implement!");
	}
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
	Triangle(const std::shared_ptr<TriangleMesh> & _mesh, int _triNumber);

	Bounds3f ObjectBounds() const;
	Bounds3f WorldBounds() const;

	bool Intersect(const Ray & ray, Float *tHit, SurfaceInteraction*inter) const;
	bool IntersectP(const Ray & ray) const;

	Interaction Sample(const Point3f &p, const Point2f& sample, Float* pdf) const;
	Float Area() const;	

private:
	//const 
	std::shared_ptr<TriangleMesh> mesh;
	//TriangleMesh * mesh;
	const int * index = nullptr;
	const int triNumber = 0;
};

std::vector<std::shared_ptr<Shape>> CreateWavefrontOBJ(const Transform* o2w, const Transform* w2o, PropertyList &list);

RAINBOW_NAMESPACE_END

#endif // !__TRIANGLE_H

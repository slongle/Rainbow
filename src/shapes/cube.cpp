#include "cube.h"

RAINBOW_NAMESPACE_BEGIN

/*Cube::Cube(const Transform * _ObjectToWorld, const Transform * _WorldToObject) :Shape(_ObjectToWorld, _WorldToObject) {

}


Bounds3f Cube::ObjectBounds() const {
	return bound;
}

bool Cube::Intersect(const Ray & ray, Float * tHit, SurfaceInteraction* inter) const {
	Ray r = (*WorldToObject)(ray);
	Float t0 = 0, t1 = r.tMax;
	for (int i = 0; i < 3; i++) {
		Float invDir = 1 / r.d[i];
		Float tNear = (-1 - r.o[i]) * invDir;
		Float tFar  = ( 1 - r.o[i]) * invDir;
		if (tNear > tFar) std::swap(tNear, tFar);

		t0 = std::max(t0, tNear);
		
		//t1 = std::min(t1, tFar);
		if (t1 > tFar) t1 = tFar; 
		
		if (t0 > t1) return false;
	}
	*tHit = t0;
	//*inter = (*ObjectToWorld)(SurfaceInteraction(r(t0), , this));
	return true;
}

bool Cube::IntersectP(const Ray & ray) const {
	return bound.IntersectP((*WorldToObject)(ray));
}

Float Cube::Area() const {
	return bound.Area(); 
}

std::shared_ptr<Cube> CreateCube(const PropertyList & list) {
	return nullptr;
}
*/
RAINBOW_NAMESPACE_END

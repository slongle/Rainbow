#ifndef __INTERACTION_H
#define __INTERACTION_H

#include "vector.h"
#include "spectrum.h"
#include "ray.h"
#include "medium.h"
#include <cmath>

RAINBOW_NAMESPACE_BEGIN

Point3f OffsetRayOrigin(const Point3f &p, const Vector3f &pError,
    const Normal3f &n, const Vector3f &w);

class Interaction {
public:
	Interaction() {}
    Interaction(
        const Point3f&           p,
        const Normal3f&          n,
        const Vector3f&          wo,
        const MediumInterface&   mediumInterface);

    Interaction(
        const Point3f& m_p, const Vector3f& m_pError,
        const Normal3f& m_n, const Vector3f& m_wo, 
        const MediumInterface& m_mediumInterface);

    Interaction(
        const Point3f& m_p, const Vector3f& m_wo,
        const MediumInterface& m_mediumInterface);

    Interaction(
        const Point3f& m_p, 
        const MediumInterface& m_mediumInterface);
	
    Ray SpawnToRay(const Vector3f& d) const {
        Point3f o = OffsetRayOrigin(p, pError, n, d);        
        return Ray(o, d, Float(INFINITY), GetMedium(d));
	}

	Ray SpawnToRay(const Interaction& it) const {        
        Point3f origin = OffsetRayOrigin(p, pError, n, it.p - p);
        Point3f target = OffsetRayOrigin(it.p, it.pError, it.n, origin - it.p);
        Vector3f d = target - origin;
        return Ray(origin, Normalize(d), d.Length() - Epsilon, GetMedium(d));
	}

    const Medium * GetMedium(const Vector3f& d) const {
        return Dot(n, d) > 0 ? mediumInterface.outside : mediumInterface.inside;
    }

    bool IsSurfaceInteraction() const { return n != Normal3f(); }

	Point3f p;
    Vector3f pError;
	Normal3f n;
	Vector3f wo;
    MediumInterface mediumInterface;
};

class SurfaceInteraction :public Interaction {
public:
	SurfaceInteraction() {}
    SurfaceInteraction(
        const Point3f&    p,
        const Point2f&    uv,
        const Vector3f&   wo,
        const Vector3f&   dpdu,
        const Vector3f&   dpdv,
        const Normal3f&   dndu,
        const Normal3f&   dndv,
        const Shape*      shape
    );
	SurfaceInteraction(
        const Point3f& m_p, const Vector3f& m_pError, 
        const Normal3f& m_n, const Vector3f& m_wo, const Shape *m_shape);

    void SetShadingGeometry();
	void ComputeScatteringFunctions(MemoryArena& arena);
	
	RGBSpectrum Le(const Vector3f& w) const;

    Point2f uv;
    Vector3f dpdu, dpdv;
    Normal3f dndu, dndv;

    struct {
        Normal3f n;
    } shading;
	
    const Shape *shape = nullptr;
	const Primitive *primitive = nullptr;
	
    BSDF *bsdf = nullptr;
};

class MediumInteraction :public Interaction {
public:
    MediumInteraction() {};
    MediumInteraction(
        const Point3f& m_p, const Vector3f& m_wo, 
        const Medium* m_medium, const PhaseFunction* m_phase);

    bool IsValid() const { return phase != nullptr; }

    const PhaseFunction* phase = nullptr;
};

RAINBOW_NAMESPACE_END

#endif // !__INTERACTION_H

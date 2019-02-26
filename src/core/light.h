#ifndef __RAINBOW_LIGHT_H
#define __RAINBOW_LIGHT_H

#include "interaction.h"
#include "propertylist.h"
#include "shape.h"

RAINBOW_NAMESPACE_BEGIN

class Visibility {
public:
	Visibility() {}
	Visibility(const Interaction& m_p0, const Interaction& m_p1) :p0(m_p0), p1(m_p1) {}

    bool Occluded(const Scene& scene) const;
    RGBSpectrum Tr(const Scene& scene, Sampler& sampler) const;

	Interaction p0, p1;
};

class Light {
public:
    Light(const Transform& m_LightToWorld, const MediumInterface& m_mediumInterface)
        : LightToWorld(m_LightToWorld),
        WorldToLight(Inverse(LightToWorld)),
        mediumInterface(m_mediumInterface) {}
    
    virtual Float PdfLi(const Interaction& ref,const Vector3f& wi) const = 0;
	virtual RGBSpectrum SampleLi(const Interaction& intersection, 
        const Point2f& sample, Vector3f* wi, Float* pdf, 
        Visibility* vis) const = 0;
    virtual bool IsDeltaLight() const = 0;

    const MediumInterface mediumInterface;
    const Transform LightToWorld, WorldToLight;
};

class AreaLight :public Light {
public:
    AreaLight(const Transform& m_LightToWorld,
        const MediumInterface& m_mediumInterface,
        const RGBSpectrum& m_Le,
        const std::shared_ptr<Shape>& m_shape)
        : Light(m_LightToWorld, m_mediumInterface), I(m_Le), shape(m_shape) {}
	
	RGBSpectrum L(const Interaction& intersection,const Vector3f &w) const;
    
    Float PdfLi(const Interaction& ref, const Vector3f& wi) const override;
    RGBSpectrum SampleLi(
        const Interaction& intersection,
        const Point2f& sample,
        Vector3f *wi,
        Float *pdf,
        Visibility *vis) const override;

    bool IsDeltaLight() const override { return false; }

    RGBSpectrum I;
	std::shared_ptr<Shape> shape;
};

std::shared_ptr<AreaLight> CreateAreaLight(
    PropertyList& list, 
    const std::shared_ptr<Shape>& shape,
    const MediumInterface& mediumInterface);

RAINBOW_NAMESPACE_END

#endif // __LIGHT_H

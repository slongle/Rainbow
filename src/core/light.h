#ifndef __LIGHT_H
#define __LIGHT_H

#include "interaction.h"
#include "propertylist.h"
#include "shape.h"

RAINBOW_NAMESPACE_BEGIN

class Light {
public:
	Light(const RGBSpectrum& m_Lemit) :Lemit(m_Lemit) {}

	virtual RGBSpectrum SampleLi(const Interaction& intersection, const Point2f& sample, 
		Vector3f* wo, Float* pdf) const = 0;

	RGBSpectrum Lemit;
};

class AreaLight :public Light {
public:
	AreaLight(const RGBSpectrum& m_Lemit, const std::shared_ptr<Shape> &m_shape) :Light(m_Lemit), shape(m_shape) {}
	
	RGBSpectrum L(const Interaction& intersection,const Vector3f &w) const ;	
	RGBSpectrum SampleLi(const Interaction& intersection, const Point2f& sample, 
		Vector3f* wi, Float* pdf) const;

	std::shared_ptr<Shape> shape;
};

std::shared_ptr<AreaLight> CreateAreaLight(PropertyList &list, const std::shared_ptr<Shape> &shape);

RAINBOW_NAMESPACE_END

#endif // __LIGHT_H
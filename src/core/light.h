#ifndef __LIGHT_H
#define __LIGHT_H

#include "interaction.h"

RAINBOW_NAMESPACE_BEGIN

class Light {
public:
	Light(const RGBSpectrum& _Lemit) :Lemit(_Lemit) {}
	RGBSpectrum Lemit;
};

class AreaLight :public Light {
public:
	AreaLight(const RGBSpectrum& _Lemit) :Light(_Lemit) {}
	RGBSpectrum L(const SurfaceInteraction& intersection,const Vector3f &w) const ;
};

RAINBOW_NAMESPACE_END

#endif // __LIGHT_H

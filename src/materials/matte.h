#ifndef __MATTE_H
#define __MATTE_H

#include "../core/material.h"

RAINBOW_NAMESPACE_BEGIN

class MatteMaterial :public Material {
public:
	MatteMaterial(const RGBSpectrum& m_R) :R(m_R) {}

	void ComputeScatteringFunctions(SurfaceInteraction* intersection);

	RGBSpectrum R;
};

MatteMaterial* CreateMatteMaterial(PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // __MATTE_H
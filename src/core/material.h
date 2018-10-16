#ifndef __MATERIAL_H
#define __MATERIAL_H

#include "propertylist.h"
#include "bsdf.h"
#include "interaction.h"


RAINBOW_NAMESPACE_BEGIN

class Material {
public:
	Material() {}
	virtual void ComputeScatteringFunctions(SurfaceInteraction* intersection);
};

RAINBOW_NAMESPACE_END

#endif // __MATERIAL_H

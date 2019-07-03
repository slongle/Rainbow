#include "matte.h"

RAINBOW_NAMESPACE_BEGIN

void MatteMaterial::ComputeScatteringFunctions(
    MemoryArena&          arena, 
    SurfaceInteraction*   intersection) 
{
    //intersection->bsdf = ARENA_ALLOCA(arena, BSDF)(intersection->n);
    intersection->bsdf = ARENA_ALLOCA(arena, BSDF)(*intersection);
    intersection->bsdf->Add(ARENA_ALLOCA(arena, LambertianReflection)(R));
}

MatteMaterial* CreateMatteMaterial(const PropertyList & list) {
	RGBSpectrum R = list.getColor("reflectance", RGBSpectrum(0.5));
	return new MatteMaterial(R);
}

RAINBOW_NAMESPACE_END

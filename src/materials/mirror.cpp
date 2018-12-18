#include "mirror.h"

RAINBOW_NAMESPACE_BEGIN

void MirrorMaterial::ComputeScatteringFunctions(MemoryArena& arena, SurfaceInteraction * intersection) {
    //FresnelNoOp *fresnel = new FresnelNoOp();
    //intersection->bsdf = new BSDF(intersection->n);
    //intersection->bsdf->Add(new SpecularReflection(R, new FresnelNoOp()));

    intersection->bsdf = ARENA_ALLOCA(arena, BSDF)(intersection->n);
    intersection->bsdf->Add(ARENA_ALLOCA(arena, SpecularReflection)(R, ARENA_ALLOCA(arena, FresnelNoOp)()));
}

MirrorMaterial* CreateMirrorMaterial(PropertyList& list) {
    RGBSpectrum R = list.getColor("reflectance", RGBSpectrum(0.5));
    return new MirrorMaterial(R);
}


RAINBOW_NAMESPACE_END

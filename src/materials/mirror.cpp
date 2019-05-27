#include "mirror.h"

RAINBOW_NAMESPACE_BEGIN

void MirrorMaterial::ComputeScatteringFunctions(
    MemoryArena&          arena, 
    SurfaceInteraction*   intersection) 
{    
    //intersection->bsdf = ARENA_ALLOCA(arena, BSDF)(intersection->n);
    intersection->bsdf = ARENA_ALLOCA(arena, BSDF)(*intersection);
    intersection->bsdf->Add(
        ARENA_ALLOCA(arena, SpecularReflection)(R, ARENA_ALLOCA(arena, FresnelNoOp)()));
}

MirrorMaterial* CreateMirrorMaterial(PropertyList& list) {
    RGBSpectrum R = list.getColor("reflectance", RGBSpectrum(1));
    return new MirrorMaterial(R);
}


RAINBOW_NAMESPACE_END

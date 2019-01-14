#include "glass.h"

RAINBOW_NAMESPACE_BEGIN

void GlassMaterial::ComputeScatteringFunctions(MemoryArena& arena, SurfaceInteraction * intersection) {
    intersection->bsdf = ARENA_ALLOCA(arena, BSDF)(intersection->n);
    //intersection->bsdf->Add(ARENA_ALLOCA(arena, SpecularReflection)(R, ARENA_ALLOCA(arena, FresnelNoOp)()));
    intersection->bsdf->Add(ARENA_ALLOCA(arena, SpecularTransmission)(T, etaI, etaT));
}

GlassMaterial* CreateGlassMaterial(PropertyList& list) {
    Float etaI = list.getFloat("etaI", Float(1));
    Float etaT = list.getFloat("etaT", Float(1.5));
    RGBSpectrum T = list.getColor("transmission", RGBSpectrum(1));
    return new GlassMaterial(T, etaI, etaT);
}


RAINBOW_NAMESPACE_END

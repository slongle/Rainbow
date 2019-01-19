#include "glass.h"

RAINBOW_NAMESPACE_BEGIN

void GlassMaterial::ComputeScatteringFunctions(MemoryArena& arena, SurfaceInteraction * intersection) {
    intersection->bsdf = ARENA_ALLOCA(arena, BSDF)(intersection->n);
    Fresnel* fresnel= ARENA_ALLOCA(arena, FresnelDielectric)(etaI, etaT);
    intersection->bsdf->Add(ARENA_ALLOCA(arena, SpecularReflection)(R, fresnel));    
    intersection->bsdf->Add(ARENA_ALLOCA(arena, SpecularTransmission)(T, etaI, etaT));
}

GlassMaterial* CreateGlassMaterial(PropertyList& list) {
    Float etaI = list.getFloat("etaI", Float(1));
    Float etaT = list.getFloat("etaT", Float(1.5));
    RGBSpectrum R = list.getColor("reflectance", RGBSpectrum(1));
    RGBSpectrum T = list.getColor("transmission", RGBSpectrum(1));
    return new GlassMaterial(R, T, etaI, etaT);
}


RAINBOW_NAMESPACE_END

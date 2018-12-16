#include "mirror.h"

RAINBOW_NAMESPACE_BEGIN

void MirrorMaterial::ComputeScatteringFunctions(SurfaceInteraction * intersection) {
    //FresnelNoOp *fresnel = new FresnelNoOp();
    intersection->bsdf = new BSDF(intersection->n);
    intersection->bsdf->Add(new SpecularReflection(R, new FresnelNoOp()));
}

MirrorMaterial* CreateMirrorMaterial(PropertyList& list) {
    RGBSpectrum R = list.getColor("reflectance", RGBSpectrum(0.5));
    return new MirrorMaterial(R);
}


RAINBOW_NAMESPACE_END

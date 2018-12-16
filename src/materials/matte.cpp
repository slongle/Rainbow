#include "matte.h"

RAINBOW_NAMESPACE_BEGIN

void MatteMaterial::ComputeScatteringFunctions(SurfaceInteraction * intersection) {
    //intersection->bsdf = (BSDF*)alloca(sizeof(BSDF));
    //*(intersection->bsdf) = BSDF(intersection->n);
    intersection->bsdf = new BSDF(intersection->n);
    intersection->bsdf->Add(new LambertianReflection(R));
}

MatteMaterial* CreateMatteMaterial(PropertyList & list) {
	RGBSpectrum R = list.getColor("reflectance", RGBSpectrum(0.5));
	return new MatteMaterial(R);
}

RAINBOW_NAMESPACE_END

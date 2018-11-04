#include "pt.h"

RAINBOW_NAMESPACE_BEGIN

RGBSpectrum PathIntegrator::Li(const Ray & ray, const Scene & scene, int depth) {
    return RGBSpectrum();
}


PathIntegrator * CreatePathIntegrator(PropertyList & list) {
    return new PathIntegrator();
}

RAINBOW_NAMESPACE_END

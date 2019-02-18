#include "scene.h"

RAINBOW_NAMESPACE_BEGIN

bool Scene::IntersectTr(
    Ray ray, Sampler& sampler, 
    SurfaceInteraction *inter, RGBSpectrum *Tr) const
{
    *Tr = RGBSpectrum(1.);
    for(;;) {
        bool hitSurface = IntersectP(ray, inter);

        // Update Tr for current ray segment
        if (ray.medium) *Tr *= ray.medium->Tr(ray, sampler);

        // No hit surface
        if (!hitSurface) return false;
        // Hit surface
        if (inter->primitive->material != nullptr) return true;

        // Update ray for continuing
        ray = inter->SpawnToRay(ray.d);
    }
}

RAINBOW_NAMESPACE_END

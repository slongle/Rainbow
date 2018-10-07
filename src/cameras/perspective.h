#ifndef __PERSPECTIVE_H
#define __PERSPECTIVE_H

#include "../core/camera.h"

RAINBOW_NAMESPACE_BEGIN

class PerspectiveCamera :public Camera {
public:
	PerspectiveCamera(const Transform& CameraToWorld,const Bounds2f& screen, const Float& fov, const std::shared_ptr<Film> film,
		const Float& nearClip, const Float& farClip);

	RGBSpectrum GenerateRay(Ray* r, const Point2f& p) const override;

	Transform CameraToScreen;
	Transform ScreenToRaster, RasterToScreen;
	Transform RasterToCamera;
};

PerspectiveCamera* CreatePerspectiveCamera(PropertyList &list, const std::shared_ptr<Film> film);

RAINBOW_NAMESPACE_END


#endif // !__PERSPECTIVE_H

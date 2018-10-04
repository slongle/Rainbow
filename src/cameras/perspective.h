#ifndef __PERSPECTIVE_H
#define __PERSPECTIVE_H

#include "../core/camera.h"

RAINBOW_NAMESPACE_BEGIN

class PerspectiveCamera :public Camera {
public:
	PerspectiveCamera(const Transform& CameraToWorld, const Film* film,
		const Float& fov, const Float& nearClip, const Float& farClip);

	RGBSpectrum GenerateRay(Ray* r, const Point2f& p) const override;

	Transform CameraToScreen;
	Transform ScreenToRaster, RasterToScreen;
	Transform RasterToCamera;
};

PerspectiveCamera* CreatePerspectiveCamera(PropertyList &list, const Film* film);

RAINBOW_NAMESPACE_END


#endif // !__PERSPECTIVE_H

#ifndef __PERSPECTIVE_H
#define __PERSPECTIVE_H

#include "../core/camera.h"

RAINBOW_NAMESPACE_BEGIN

class PerspectiveCamera :public Camera {
public:
	PerspectiveCamera(const Transform& CameraToWorld, const Float& fov, const Float& near, const Float& far);

	RGBSpectrum GenerateRay(Ray& r, const Point2f& p) const override;

	Transform CameraToScreen, ScreenToCamera;
	Transform ScreenToRaster, RasterToScreen;
	Transform RasterToCamera, CameraToRaster;
};

PerspectiveCamera* CreatePerspectiveCamera(PropertyList &list, const Film* film);

RAINBOW_NAMESPACE_END


#endif // !__PERSPECTIVE_H

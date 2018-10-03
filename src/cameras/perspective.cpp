#include "perspective.h"

RAINBOW_NAMESPACE_BEGIN

PerspectiveCamera::PerspectiveCamera(const Transform & CameraToWorld, const Film* _film,
	const Float & fov, const Float &near, const Float & far) :
	Camera(CameraToWorld) {
	CameraToScreen = Scale(Float(_film->resolution.x), Float(_film->resolution.y) * _film->aspect, 1) * Perspective(fov, near, far);	
	ScreenToRaster = Scale(1, -1, 1)*Translate(-(_film->resolution.x)*0.5f, -(_film->resolution.y)*0.5f, 0);
	RasterToScreen = Inverse(ScreenToRaster);
	RasterToCamera = Inverse(CameraToScreen)*RasterToScreen;
}


RGBSpectrum PerspectiveCamera::GenerateRay(Ray* r, const Point2f & p) const {
	Vector3f d = Normalize(RasterToCamera(Point3f(p.x, p.y, 0)));
	*r = Ray(Point3f(0, 0, 0), d);
	*r = CameraToWorld(*r);
	return RGBSpectrum(1.0);
}


PerspectiveCamera * CreatePerspectiveCamera(PropertyList & list, const Film* film) {	
	Transform CameraToWorld = list.getTransform("toWorld", Transform());	
	Float fov = list.getFloat("fov", 30.0);
	Float near = list.getFloat("nearClip", 1e-2f);
	Float far = list.getFloat("farClip", 1000);
	return new PerspectiveCamera(CameraToWorld, film, fov, near, far);
}

RAINBOW_NAMESPACE_END
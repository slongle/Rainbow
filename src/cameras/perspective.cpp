#include "perspective.h"

RAINBOW_NAMESPACE_BEGIN

PerspectiveCamera::PerspectiveCamera(const Transform & CameraToWorld, 
	const Float & fov, const Float &near, const Float & far) :
	Camera(CameraToWorld) {
	CameraToScreen = Scale(Float(film->resolution.x), Float(film->resolution.y), 1) * Perspective(fov, near, far, film->aspect);
	ScreenToCamera = Inverse(CameraToScreen);
	ScreenToRaster = Scale(1, -1, 1)*Translate(-(film->resolution.x)*0.5f, -(film->resolution.y)*0.5f, 0);
	RasterToScreen = Inverse(ScreenToRaster);
	CameraToRaster = ScreenToRaster * CameraToScreen;
	RasterToCamera = Inverse(CameraToRaster);
}


RGBSpectrum PerspectiveCamera::GenerateRay(Ray & r, const Point2f & p) const {
	Vector3f d = Normalize(RasterToCamera(Point3f(p.x, p.y, 0)));
	r.o = Point3f(0, 0, 0);
	r.d = d;
	r = CameraToWorld(r);
	return RGBSpectrum(1.0);
}


PerspectiveCamera * CreatePerspectiveCamera(PropertyList & list, const Film* film) {	
	Transform CameraToWorld = list.getTransform("toWorld", Transform());	
	Float fov = list.getFloat("fov", 30.0);
	Float near = list.getFloat("nearClip", 1e-2f);
	Float far = list.getFloat("farClip", 1000);
	return new PerspectiveCamera(CameraToWorld, fov, near, far);
}

RAINBOW_NAMESPACE_END
#include "perspective.h"

RAINBOW_NAMESPACE_BEGIN

PerspectiveCamera::PerspectiveCamera(
    const Transform&         CameraToWorld, 
	const Float&             fov, 
    const Float&             nearClip, 
    const Float&             farClip,
    std::shared_ptr<Film>    film)
    : Camera(CameraToWorld, film), m_fov(fov), m_nearClip(nearClip), m_farClip(farClip)
{
	/*
    CameraToScreen = Perspective(fov, nearClip, farClip);
    ScreenToRaster =
        Scale(film->resolution.x, film->resolution.y, 1) *
        Scale(-0.5, -0.5 * film->aspect, 1) * 
        Translate(-screen.pMax.x, -screen.pMax.y, 0);
	RasterToScreen = Inverse(ScreenToRaster);
	RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;
    */

    const Float& aspect = film->aspect;
    Transform CameraToRaster =
        Scale(film->resolution.x, film->resolution.y, 1) *
        Scale(-0.5, -0.5 * aspect, 1) *
        Translate(-1, -1. / aspect, 0) *
        Perspective(fov, nearClip, farClip);
    RasterToCamera = Inverse(CameraToRaster);
}


RGBSpectrum PerspectiveCamera::GenerateRay(Ray* r, const Point2f & p) const {
	Vector3f d = Normalize(RasterToCamera(Point3f(p.x, p.y, 0)));
	*r = Ray(Point3f(0, 0, 0), d);
	*r = CameraToWorld(*r);
	return RGBSpectrum(1.0);
}


PerspectiveCamera * CreatePerspectiveCamera(
    const PropertyList     &list, 
    std::shared_ptr<Film>   film) 
{
    Transform c2w = list.getTransform("toWorld", Transform::identityTransform);

	Float fov = list.getFloat("fov", 30.0);
	Float nearClip = list.getFloat("nearClip", 1e-4f);
	Float farClip = list.getFloat("farClip", 10000);	
	return new PerspectiveCamera(c2w, fov, nearClip, farClip, film);
}

RAINBOW_NAMESPACE_END
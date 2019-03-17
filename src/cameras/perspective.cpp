#include "perspective.h"

RAINBOW_NAMESPACE_BEGIN

PerspectiveCamera::PerspectiveCamera(
    const Transform&         CameraToWorld, 
    const Bounds2f&          screen,
	const Float&             fov, 
    const Float&             nearClip, 
    const Float&             farClip,
    std::shared_ptr<Film>    film)
    : Camera(CameraToWorld, film), fov(fov), nearClip(nearClip), farClip(farClip)
{
	CameraToScreen = Perspective(fov, nearClip, farClip);
	ScreenToRaster = 
		Scale(film->resolution.x, film->resolution.y, 1) *
		Scale(1 / (screen.pMax.x - screen.pMin.x),
			  1 / (screen.pMin.y - screen.pMax.y), 1)*
		Translate(Vector3f(-screen.pMin.x, -screen.pMax.y, 0));

	RasterToScreen = Inverse(ScreenToRaster);
	RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;
}


RGBSpectrum PerspectiveCamera::GenerateRay(Ray* r, const Point2f & p) const {
    Point2f sp(p);     
	Vector3f d = Normalize(RasterToCamera(Point3f(sp.x, sp.y, 0)));
	*r = Ray(Point3f(0, 0, 0), d);
	*r = CameraToWorld(*r);
	return RGBSpectrum(1.0);
}


PerspectiveCamera * CreatePerspectiveCamera(
    const Transform&        CameraToWorld, 
    PropertyList&           list, 
    std::shared_ptr<Film>   film) 
{
	Float fov = list.getFloat("fov", 30.0);
	Float nearClip = list.getFloat("nearClip", 1e-2f);
	Float farClip = list.getFloat("farClip", 10000);
	Float frame = film->aspect;
	Bounds2f screen;
	screen.pMin.x = -frame;
	screen.pMin.y = -1;
	screen.pMax.x = frame;
	screen.pMax.y = 1;
	return new PerspectiveCamera(CameraToWorld, screen, fov, nearClip, farClip, film);
}

RAINBOW_NAMESPACE_END
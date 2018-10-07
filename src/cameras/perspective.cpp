#include "perspective.h"

RAINBOW_NAMESPACE_BEGIN

PerspectiveCamera::PerspectiveCamera(const Transform& CameraToWorld, const Bounds2f& screen,
	const Float& fov, const std::shared_ptr<Film> _film, const Float& nearClip, const Float& farClip) :
	Camera(CameraToWorld, _film) {
	//std::cout << CameraToWorld << std::endl;

	CameraToScreen = Perspective(fov, nearClip, farClip);
	ScreenToRaster = 
		Scale(film->resolution.x, film->resolution.y, 1) *
		Scale(1 / (screen.pMax.x - screen.pMin.x),
			  1 / (screen.pMin.y - screen.pMax.y), 1)*
		Translate(Vector3f(-screen.pMin.x, -screen.pMax.y, 0));

	RasterToScreen = Inverse(ScreenToRaster);
	RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;

	//std::cout << RasterToCamera << std::endl;
	//CameraToScreen = Scale(Float(_film->resolution.x), Float(_film->resolution.y) * _film->aspect, 1) * 
	//	Perspective(fov, nearClip, farClip);	
	//ScreenToRaster = Scale(1, -1, 1)*Translate(-(_film->resolution.x)*0.5f, -(_film->resolution.y)*0.5f, 0);
	//RasterToScreen = Inverse(ScreenToRaster);
	//RasterToCamera = Inverse(CameraToScreen)*RasterToScreen;	
	//
	//
	//RasterToCamera = Inverse(Perspective(fov, nearClip, farClip))*
	//	Inverse(Translate(-1, -1 / _film->aspect, 0))*Inverse(Scale(-0.5, -0.5*_film->aspect, 1));

	//RasterToCamera = Inverse(Scale(-0.5, -0.5*_film->aspect, 1)*Translate(-1, -1 / _film->aspect, 0)*Perspective(fov, nearClip, farClip));
	//std::cout << RasterToCamera << std::endl;
}


RGBSpectrum PerspectiveCamera::GenerateRay(Ray* r, const Point2f & p) const {
	Vector3f d = Normalize(RasterToCamera(Point3f(p.x, p.y, 0)));
	*r = Ray(Point3f(0, 0, 0), d);
	*r = CameraToWorld(*r);
	return RGBSpectrum(1.0);
}


PerspectiveCamera * CreatePerspectiveCamera(PropertyList & list, const std::shared_ptr<Film> film) {
	Transform CameraToWorld = list.getTransform("toWorld", Transform());	
	Float fov = list.getFloat("fov", 30.0);
	Float nearClip = list.getFloat("nearClip", 1e-2f);
	Float farClip = list.getFloat("farClip", 1000);
	Float frame = film->aspect;
	Bounds2f screen;
	screen.pMin.x = -frame;
	screen.pMin.y = -1;
	screen.pMax.x = frame;
	screen.pMax.y = 1;
	return new PerspectiveCamera(CameraToWorld, screen, fov, film, nearClip, farClip);
}

RAINBOW_NAMESPACE_END
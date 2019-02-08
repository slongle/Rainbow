#ifndef __CAMERA_H
#define __CAMERA_H

#include "ray.h"
#include "film.h"

RAINBOW_NAMESPACE_BEGIN

class Camera{
public:
	Camera(const Transform& m_CameraToWorld, const std::shared_ptr<Film> m_film);

	virtual RGBSpectrum GenerateRay(Ray* r,const Point2f& p) const = 0;	

	Transform CameraToWorld, WorldToCamera;
	const std::shared_ptr<Film> film;
};

RAINBOW_NAMESPACE_END

#endif // !__CAMERA_H

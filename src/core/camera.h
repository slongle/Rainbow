#ifndef __CAMERA_H
#define __CAMERA_H

#include "ray.h"
#include "object.h"
#include "film.h"

RAINBOW_NAMESPACE_BEGIN

class Camera : public Object {
public:
	Camera();

	//virtual Ray GenerateRay(const Point2i p) const = 0;

	void addChild(Object *child) override;
	EClassType getClassType() const override { return ECamera; }

	Film* film;
};

RAINBOW_NAMESPACE_END

#endif // !__CAMERA_H

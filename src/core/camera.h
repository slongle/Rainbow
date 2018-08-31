#ifndef __CAMERA_H
#define __CAMERA_H

#include "common.h"
#include "vector.h"
#include "ray.h"

RAINBOW_NAMESPACE_BEGIN

class Camera {
	Camera();

	virtual Ray GenerateRay(const Point2i p) const = 0;
};

RAINBOW_NAMESPACE_END

#endif // !__CAMERA_H

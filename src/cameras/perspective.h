#ifndef __RAINBOW_PERSPECTIVE
#define __RAINBOW_PERSPECTIVE

#include "../core/camera.h"

RAINBOW_NAMESPACE_BEGIN

class Perspective :public Camera {
public:
	Perspective() {}
};

Perspective* CreatePerspectiveCamera(PropertyList &list) {
	return new Perspective();
}

RAINBOW_NAMESPACE_END


#endif // !__RAINBOW_PERSPECTIVE

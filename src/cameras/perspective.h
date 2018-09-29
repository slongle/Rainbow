#ifndef __PERSPECTIVE_H
#define __PERSPECTIVE_H

#include "../core/camera.h"

RAINBOW_NAMESPACE_BEGIN

class Perspective :public Camera {
public:
	Perspective() {}
};

Perspective* CreatePerspectiveCamera(PropertyList &list);

RAINBOW_NAMESPACE_END


#endif // !__PERSPECTIVE_H

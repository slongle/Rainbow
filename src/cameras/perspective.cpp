#include "perspective.h"

RAINBOW_NAMESPACE_BEGIN

Perspective * CreatePerspectiveCamera(PropertyList & list) {
	return new Perspective();
}

RAINBOW_NAMESPACE_END

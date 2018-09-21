#include"camera.h"

RAINBOW_NAMESPACE_BEGIN

Camera::Camera() {
}

void Camera::addChild(Object * child) {
	if (child->getClassType() == EFilm) {
		Assert(film != nullptr, "Camera has a film!");
		film = static_cast<Film*>(child);
	}
}

RAINBOW_NAMESPACE_END

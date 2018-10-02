#include"camera.h"

RAINBOW_NAMESPACE_BEGIN

Camera::Camera(const Transform& _CameraToWorld) :
	CameraToWorld(_CameraToWorld), WorldToCamera(Inverse(CameraToWorld)) {

}

void Camera::addChild(Object * child) {
	if (child->getClassType() == EFilm) {
		Assert(film == nullptr, "Camera has already had a film!");
		film = static_cast<Film*>(child);
	}
}

RAINBOW_NAMESPACE_END

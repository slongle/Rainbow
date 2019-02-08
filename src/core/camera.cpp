#include"camera.h"

RAINBOW_NAMESPACE_BEGIN

Camera::Camera(const Transform& m_CameraToWorld, const std::shared_ptr<Film> m_film) :
	CameraToWorld(m_CameraToWorld), WorldToCamera(Inverse(CameraToWorld)), film(m_film) {
}

RAINBOW_NAMESPACE_END

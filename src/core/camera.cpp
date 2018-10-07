#include"camera.h"

RAINBOW_NAMESPACE_BEGIN

Camera::Camera(const Transform& _CameraToWorld, const std::shared_ptr<Film> _film) :
	CameraToWorld(_CameraToWorld), WorldToCamera(Inverse(CameraToWorld)), film(_film) {

}


RAINBOW_NAMESPACE_END

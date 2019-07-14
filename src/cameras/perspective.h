#ifndef __PERSPECTIVE_H
#define __PERSPECTIVE_H

#include "../core/camera.h"

RAINBOW_NAMESPACE_BEGIN

class PerspectiveCamera :public Camera {
public:
	PerspectiveCamera(
        const Transform&              CameraToWorld,
        const Float&                  fov, 
		const Float&                  nearClip, 
        const Float&                  farClip,
        std::shared_ptr<Film>         film);

	RGBSpectrum GenerateRay(Ray* r, const Point2f& p) const override;

    std::string toString() const override {
        return tfm::format(
            "Perspective Camera[\n"
            "    CameraToWorld : %s,\n"
            "    Fov : %f,\n"
            "    NearClip : %f,\n"
            "    FarClip : %f,\n"
            "    Film : %s,\n"
            "]"
            , indent(CameraToWorld.toString(), 20)
            , m_fov
            , m_nearClip
            , m_farClip
            , indent(film->toString(), 11)
        );

    }

	Transform CameraToScreen;
	Transform ScreenToRaster, RasterToScreen;
	Transform RasterToCamera;
private:
    Float m_fov;
    Float m_nearClip;
    Float m_farClip;
};

PerspectiveCamera* CreatePerspectiveCamera(
    const PropertyList      &list, 
    std::shared_ptr<Film>    film);

RAINBOW_NAMESPACE_END


#endif // !__PERSPECTIVE_H

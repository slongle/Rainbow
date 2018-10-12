#include "light.h"

RAINBOW_NAMESPACE_BEGIN




RGBSpectrum AreaLight::L(const SurfaceInteraction& interaction, const Vector3f & w) const {
	return (Dot(interaction.n, w) > 0) ? Lemit : RGBSpectrum(0.0);
}


RAINBOW_NAMESPACE_END

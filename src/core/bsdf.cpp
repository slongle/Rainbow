#include"bsdf.h"

RAINBOW_NAMESPACE_BEGIN

BSDF * CreateDiffuseBSDF(PropertyList & list) {
	return new BSDF();
}

RAINBOW_NAMESPACE_END
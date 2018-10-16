#include "independent.h"

RAINBOW_NAMESPACE_BEGIN

Float Independent::Get1D() {
	return rng.nextFloat();
}

Point2f Independent::Get2D() {
	return Point2f(rng.nextFloat(), rng.nextFloat());
}

Independent* CreateIndependentSampler(PropertyList & list) {
	int SamplerCount = list.getInteger("sampleCount");
	return new Independent(SamplerCount);
}

RAINBOW_NAMESPACE_END

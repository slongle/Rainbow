#include "independent.h"

RAINBOW_NAMESPACE_BEGIN

std::unique_ptr<Sampler> Independent::Clone(const Point2i &seed) {
    std::unique_ptr<Independent> cloned(new Independent());
    cloned->sampleCount = sampleCount;
    cloned->random = random;
    cloned->random.seed(seed.x, seed.y);
    return std::move(cloned);
}

Float Independent::Next1D() {
	return random.nextFloat();
}

Point2f Independent::Next2D() {
	return Point2f(random.nextFloat(), random.nextFloat());
}

Independent* CreateIndependentSampler(const PropertyList & list) {
	int sampleCount = list.getInteger("sampleCount");
	return new Independent(sampleCount);
}

RAINBOW_NAMESPACE_END

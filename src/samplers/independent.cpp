#include "independent.h"

RAINBOW_NAMESPACE_BEGIN

void Independent::Initialize(const int &x, const int &y) {
    rng.seed(x, y);
}


std::unique_ptr<Sampler> Independent::Clone() {
    std::unique_ptr<Independent> cloned(new Independent());
    cloned->sampleCount = sampleCount;
    cloned->rng= rng;
    return std::move(cloned);
}

Float Independent::Get1D() {
	return rng.nextFloat();
}

Point2f Independent::Get2D() {
	return Point2f(rng.nextFloat(), rng.nextFloat());
}

Independent* CreateIndependentSampler(PropertyList & list) {
	int sampleCount = list.getInteger("sampleCount");
	return new Independent(sampleCount);
}

RAINBOW_NAMESPACE_END

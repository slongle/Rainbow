#include "halton.h"

RAINBOW_NAMESPACE_BEGIN

HaltonSampler::HaltonSampler(const Point2i& resolution)
    : halton_enum(resolution.x, resolution.y)
{
    maxSampleNum = halton_enum.get_max_samples_per_pixel();
    halton_sampler.init_faure();
    halton_sampler.init_random(random);
}

std::unique_ptr<Sampler> HaltonSampler::Clone(const Point2i& seed) {
    return std::unique_ptr<Sampler>(new HaltonSampler(*this));
}

int64_t HaltonSampler::GetIndexForSample(int64_t sampleNum) const {
    assert(sampleNum < maxSampleNum);
    return halton_enum.get_index(sampleNum, pixelPosition.x, pixelPosition.y);
}

Float HaltonSampler::SampleDimension(int64_t index, int dimension) const {
    /*if (dimension<=1) {
        int a = 1;
        a++;
    }*/
    assert(dimension > 1);
    return halton_sampler.sample(dimension, index);
}

Point2f HaltonSampler::GetCameraSample() {
    const float sx = halton_sampler.sample(0, currentIndex);
    const float sy = halton_sampler.sample(1, currentIndex);
    const float rx = halton_enum.scale_x(sx);
    const float ry = halton_enum.scale_y(sy);
    dimension = 2;
    return Point2f(rx, ry);
}

HaltonSampler* CreateHaltonSampler(const PropertyList& list, const Point2i& resolution) {
    //int sampleCount = list.getInteger("sampleCount");
    return new HaltonSampler(resolution);
}

RAINBOW_NAMESPACE_END

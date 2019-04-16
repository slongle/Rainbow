#ifndef __RAINBOW_HALTON
#define __RAINBOW_HALTON

#include "ext/halton/halton_enum.h"
#include "ext/halton/halton_sampler.h"

#include "core/sampler.h"

RAINBOW_NAMESPACE_BEGIN

class HaltonSampler :public DeterministicSampler{
public:
    HaltonSampler(const Point2i& resolution);

    std::unique_ptr<Sampler> Clone(const Point2i& seed) override;
    int64_t GetIndexForSample(int64_t sampleNum) const override;
    Float SampleDimension(int64_t index, int dimension) const override;
    Point2f GetCameraSample() override;

private:
    Halton_enum halton_enum;
    Halton_sampler halton_sampler;
    unsigned maxSampleNum;
};

HaltonSampler* CreateHaltonSampler(PropertyList& list, const Point2i& resolution);

RAINBOW_NAMESPACE_END


#endif // __RAINBOW_HALTON
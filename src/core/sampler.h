#ifndef __SAMPLER_H
#define __SAMPLER_H

#include "propertylist.h"

#include "ext/pcg32/pcg32.h"

RAINBOW_NAMESPACE_BEGIN

class Sampler {
public:
    Sampler() {};
    virtual void SetPixel(const Point2i& position);
    virtual void SetSampleNumber(const int& sampleNum);
    virtual void StartNextSample();
    virtual Point2f GetCameraSample();
    virtual std::unique_ptr<Sampler> Clone(const Point2i &seed) = 0;
	virtual Float Next1D() = 0;
	virtual Point2f Next2D() = 0;

protected:
    int currentSampleNum;
    Point2i pixelPosition;
    pcg32 random;
};

class DeterministicSampler : public Sampler {
public:    
    DeterministicSampler() {}

    virtual void SetPixel(const Point2i& position);
    virtual void SetSampleNumber(const int& sampleNum);
    virtual void StartNextSample();
    Float Next1D() override;
    Point2f Next2D() override;

    virtual Point2f GetCameraSample() = 0;

    /*
     * For the pixel now we sampled in£¬return the index of sample in the vector
     */
    virtual int64_t GetIndexForSample(int64_t sampleNum) const = 0;
    /*
     * Return the number in [0, 1) at dimension's index_th
     */
    virtual Float SampleDimension(int64_t index, int dimension) const = 0;

protected:    
    int64_t currentIndex;
    int dimension;

};

RAINBOW_NAMESPACE_END

#endif // __SAMPLER_H

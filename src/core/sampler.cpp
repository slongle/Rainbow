#include "sampler.h"

RAINBOW_NAMESPACE_BEGIN

void Sampler::SetPixel(const Point2i& position) {
    pixelPosition = position;
}

void Sampler::SetSampleNumber(const int& sampleNum) {
    currentSampleNum = sampleNum;
}

void Sampler::StartNextSample() {
    currentSampleNum++;
}

Point2f Sampler::GetCameraSample() {
    Point2f ret = Point2f(pixelPosition) + Next2D();
    return ret;
}

void DeterministicSampler::SetPixel(const Point2i& position) {
    Sampler::SetPixel(position);
    currentSampleNum = 0;
    currentIndex = GetIndexForSample(0);
    dimension = 0;
}

void DeterministicSampler::SetSampleNumber(const int& sampleNum) {
    currentSampleNum = sampleNum;
    currentIndex = GetIndexForSample(sampleNum);
}

void DeterministicSampler::StartNextSample() {
    Sampler::StartNextSample();
    currentIndex = GetIndexForSample(currentSampleNum);
    dimension = 0;
}

Float DeterministicSampler::Next1D() {
    return SampleDimension(currentIndex, dimension++);
}

Point2f DeterministicSampler::Next2D() {
    Point2f ret = Point2f(SampleDimension(currentIndex, dimension),
        SampleDimension(currentIndex, dimension + 1));
    dimension += 2;
    return ret;
}

RAINBOW_NAMESPACE_END

#pragma once
#ifndef __RAINBOW_MEDIUM_H
#define __RAINBOW_MEDIUM_H

#include "spectrum.h"
#include "ray.h"

RAINBOW_NAMESPACE_BEGIN

class Medium{
public:
    virtual RGBSpectrum Tr(Ray ray, Sampler &sampler) const = 0;
    virtual RGBSpectrum Sample(
        const Ray &ray, Sampler &sampler,
        MemoryArena &arena, MediumInteraction *mi) const = 0;
};

class MediumInterface {
public:
    MediumInterface() :inside(nullptr), outside(nullptr) {}
    MediumInterface(const Medium *medium) :inside(medium), outside(medium) {}
    MediumInterface(const Medium *m_inside, const Medium *m_outside) :inside(m_inside), outside(m_outside) {}
    bool IsMediumTransition() const { return inside != outside; }
    const Medium *inside, *outside;
};

class PhaseFunction {
public:
    virtual ~PhaseFunction() = default;
    virtual Float SampleP(const Vector3f& wo, Vector3f *wi, const Point2f& u) const = 0;
    virtual Float P(const Vector3f& wo, const Vector3f& wi) const = 0;
};

class HenyeyGreenstein :public PhaseFunction {
public:
    HenyeyGreenstein(Float m_g) :g(m_g) {};
    Float SampleP(const Vector3f& wo, Vector3f *wi, const Point2f& u) const override;
    Float P(const Vector3f& wo, const Vector3f& wi) const override;
    Float g;
};

RAINBOW_NAMESPACE_END

#endif // __RAINBOW_MEDIUM_H

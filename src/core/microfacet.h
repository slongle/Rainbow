#pragma once
#ifndef __RAINBOW_MICROFACET_H
#define __RAINBOW_MICROFACET_H

#include "bsdf.h"

RAINBOW_NAMESPACE_BEGIN

enum EMicrofacetDistributionType {
    EBeckmann
};

class MicrofacetDistribution{
public:
    MicrofacetDistribution(const Float& m_alpha) :alphaU(m_alpha), alphaV(m_alpha) {}

    MicrofacetDistribution(const Float& m_alphaU, const Float& m_alphaV) :alphaU(m_alphaU), alphaV(m_alphaV) {}

    virtual Float D(const Vector3f &wh) const = 0;
    virtual Float Lambda(const Vector3f &w) const = 0;
    Float G1(const Vector3f &w) const {
        return 1 / (1 + Lambda(w));
    }
    Float G(const Vector3f &wo, const Vector3f &wi) const {
        return 1 / (1 + Lambda(wo) + Lambda(wi));
    }
    virtual Vector3f SampleWh(const Vector3f &wo, const Point2f &u) const = 0;
    Float Pdf(const Vector3f &wo, const Vector3f &wh) const;

    EMicrofacetDistributionType type;
    Float alphaU, alphaV;
};

class BeckmannDistribution :public MicrofacetDistribution {
public:
    BeckmannDistribution(const Float& m_alpha) :MicrofacetDistribution(m_alpha) {}
    BeckmannDistribution(const Float& m_alphaU, const Float& m_alphaV) :MicrofacetDistribution(m_alphaU, m_alphaV) {}
    Float D(const Vector3f &wh) const override;
    Float Lambda(const Vector3f &w) const override;
    Vector3f SampleWh(const Vector3f &wo, const Point2f &u) const override;    
};

RAINBOW_NAMESPACE_END

#endif //__RAINBOW_MICROFACET_H
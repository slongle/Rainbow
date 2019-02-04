#include "microfacet.h"

RAINBOW_NAMESPACE_BEGIN

Float MicrofacetDistribution::Pdf(const Vector3f & wo, const Vector3f & wh) const {
    return D(wh) * Frame::AbsCosTheta(wh);
}

Float BeckmannDistribution::D(const Vector3f & wh) const {    
    Float tan2Theta = Frame::Tan2Theta(wh);
    if (std::isinf(tan2Theta)) return 0.;
    Float cos4Theta = Frame::Cos2Theta(wh) * Frame::Cos2Theta(wh);
    return std::exp(-tan2Theta * (Frame::Cos2Phi(wh) / (alphaU * alphaU) +
        Frame::Sin2Phi(wh) / (alphaV * alphaV))) /
        (M_PI * alphaU * alphaV * cos4Theta);    
}

Float BeckmannDistribution::Lambda(const Vector3f & w) const {    
    Float absTanTheta = std::abs(Frame::TanTheta(w));
    if (std::isinf(absTanTheta)) return 0.;
        Float alpha = std::sqrt(Frame::Cos2Phi(w) * alphaU * alphaU +
                                Frame::Sin2Phi(w) * alphaV * alphaV);
    
    Float alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);
    return (-1 + std::sqrt(1.f + alpha2Tan2Theta)) / 2;
}

Vector3f BeckmannDistribution::SampleWh(const Vector3f & wo, const Point2f & u) const {
    Float tan2Theta, phi;
    if (alphaU == alphaV) {
        Float logSample = std::log(1 - u[0]);
        Assert(!std::isinf(logSample), "Beckmann Sample Failed!");
        tan2Theta = -alphaU * alphaU * logSample;
        phi = u[1] * 2 * M_PI;
    }
    else {
        Assert(false, "alphaU is not equal to alphaV!");
    }
    Float cosTheta = 1 / std::sqrt(1 + tan2Theta);
    Float sinTheta = std::sqrt(std::max((Float)0, 1 - cosTheta * cosTheta));
    Vector3f wh = Frame::SphericalDirection(sinTheta, cosTheta, phi);
    if (!Frame::SameHemisphere(wo, wh)) wh = -wh;
    return wh;    
}

RAINBOW_NAMESPACE_END

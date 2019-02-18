#include "medium.h"
#include "vector.h"
#include "frame.h"

RAINBOW_NAMESPACE_BEGIN

inline Float PhaseHG(Float cosTheta, Float g) {
    Float denom = 1 + g * g + 2 * g * cosTheta;
    return INV_FOURPI * (1 - g * g) / (denom * std::sqrt(denom));
}

Float HenyeyGreenstein::SampleP(const Vector3f & wo, Vector3f * wi, const Point2f & u) const {
    Float cosTheta;
    if (std::abs(g) < 1e-3)
        cosTheta = 1 - 2 * u[0];
    else {
        Float sqrTerm = (1 - g * g) / (1 - g + 2 * g * u[0]);
        cosTheta = (1 + g * g - sqrTerm * sqrTerm) / (2 * g);
    }

    if (isNaN(cosTheta)) {
        cout << g << endl;
        Float sqrTerm = (1 - g * g) / (1 - g + 2 * g * u[0]);
        cout << sqrTerm << endl;
        cosTheta = (1 + g * g - sqrTerm * sqrTerm) / (2 * g);
    }

    // Compute direction _wi_ for Henyey--Greenstein sample
    Float sinTheta = std::sqrt(std::max((Float)0, 1 - cosTheta * cosTheta));
    Float phi = 2 * M_PI * u[1];
    
    Frame frame = Frame(Normal3f(wo));
    *wi = frame.toWorld(Frame::SphericalDirection(sinTheta, cosTheta, phi));    

    return PhaseHG(cosTheta, g);
}

Float HenyeyGreenstein::P(const Vector3f & wo, const Vector3f & wi) const {
    return PhaseHG(Dot(wo, wi), g);
}

RAINBOW_NAMESPACE_END

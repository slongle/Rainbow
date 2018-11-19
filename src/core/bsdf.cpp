#include"bsdf.h"

RAINBOW_NAMESPACE_BEGIN

Float FrDielectric(Float cosThetaI, Float etaI, Float etaT) {
	cosThetaI = Clamp(cosThetaI, -1, 1);

	// Check and swap the indices of refraction
	bool enter = cosThetaI > 0.0;
	if (!enter) {
		std::swap(etaI, etaT);
		cosThetaI = std::abs(cosThetaI);
	}

	// Compute sin and cos terms using Snell's law
	Float sinThetaI = std::sqrt(std::max(Float(0), 1 - cosThetaI * cosThetaI));
	Float sinThetaT = etaI / etaT * sinThetaI;
	
	// Handle total internal reflection
	if (sinThetaT >= 1) return 1;

	Float cosThetaT = std::sqrt(std::max(Float(0), 1 - sinThetaT * sinThetaT));

	// R_parallel
	Float Rparl = (etaT*cosThetaI - etaI * cosThetaT) / 
		          (etaT*cosThetaI + etaI * cosThetaT);
	// R_perpendicular
	Float Rperp = (etaI*cosThetaI - etaT * cosThetaT) / 
		          (etaI*cosThetaI + etaT * cosThetaT);
	// Fr
	Float Fr = (Rparl*Rparl + Rperp * Rperp) * 0.5;

	return Fr;
}

inline bool Refract(const Vector3f & wi, const Normal3f & n, Float eta, Vector3f* wt) {
	Float cosThetaI = Dot(wi, n);
	Float sin2ThetaI = std::max(0.f, 1.f - cosThetaI * cosThetaI);
	Float sin2ThetaT = eta * eta*sin2ThetaI;
	if (sin2ThetaT >= 1) return false;
	Float cosThetaT = std::sqrt(1 - sin2ThetaT);
	*wt = -eta * wi + (eta * cosThetaI - cosThetaT) * n;
	return true;
}

RGBSpectrum FresnelDielectric::Evaluate(Float cosThetaI) const {
	return FrDielectric(cosThetaI, etaI, etaT);
}


RGBSpectrum SpecularReflection::f(const Vector3f & wo, const Vector3f & wi) {
	return RGBSpectrum(0.0);
}

RGBSpectrum SpecularReflection::SampleF(const Vector3f & wo, Vector3f * wi, const Point2f & sample, Float * pdf) {
	*pdf = 1;
	*wi = Vector3f(-wo.x, -wo.y, wo.z);
	return fresnel->Evaluate(CosTheta(*wi)) * R / AbsCosTheta(*wi);
}

Float SpecularReflection::Pdf(const Vector3f & wo, const Vector3f & wi) {
    return 0;
}

RGBSpectrum SpecularTransmission::f(const Vector3f & wo, const Vector3f & wi) {
	return RGBSpectrum(0.0);
}

RGBSpectrum SpecularTransmission::SampleF(const Vector3f & wo, Vector3f * wi, const Point2f & sample, Float * pdf) {
	bool enter = CosTheta(wo) > 0;
	Float etaI = enter ? etaA : etaB;	
	Float etaT = enter ? etaB : etaA;

	if (!Refract(wo, FaceForward(Normal3f(0, 0, 1), wo), etaI / etaT, wi))
		return RGBSpectrum(0.0);
	*pdf = 1;

	return (etaI*etaI) / (etaT*etaT) * (RGBSpectrum(1.0) - fresnel.Evaluate(CosTheta(*wi))) * T / AbsCosTheta(*wi);
}

Float SpecularTransmission::Pdf(const Vector3f & wo, const Vector3f & wi) {
    return 0;
}


RGBSpectrum LambertianReflection::f(const Vector3f & wo, const Vector3f & wi) {
	return R * INV_PI;
}

RGBSpectrum LambertianReflection::SampleF(const Vector3f & wo, Vector3f * wi, const Point2f & sample, Float * pdf) {
	//Assert(false,"No Implement!");
	*wi = CosineSampleHemisphere(sample);
	if (wo.z < 0) wi->z *= -1;
	*pdf = SameHemisphere(wo, *wi) ? AbsCosTheta(*wi) * INV_PI : 0;
	return R * INV_PI;
}

Float LambertianReflection::Pdf(const Vector3f & wo, const Vector3f & wi) {
    return INV_TWOPI;
}

RAINBOW_NAMESPACE_END

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

int BSDF::NumComponents(const BxDFType& flags) const {
    int num = 0;
    for(int i=0;i<nBxDFs;i++) {
        if (bxdfs[i]->MatchFlags(flags)) {
            num++;
        }
    }
    return num;
}

RGBSpectrum BSDF::f(const Vector3f& woWorld, const Vector3f& wiWorld, const BxDFType& type) {
    Vector3f woLocal = frame.toLocal(woWorld), wiLocal= frame.toLocal(wiWorld);
    RGBSpectrum f(0.);
    for (int i = 0; i < nBxDFs; i++) {
        if (bxdfs[i]->MatchFlags(type)) {
            f += bxdfs[i]->f(woLocal, wiLocal);
        }
    }
    return f;
}

Float BSDF::Pdf(const Vector3f& woWorld, const Vector3f& wiWorld, const BxDFType& type) {
    Vector3f woLocal = frame.toLocal(woWorld), wiLocal = frame.toLocal(wiWorld);
    Float pdf = 0;
    int matchComp = 0;
    for (int i = 0; i < nBxDFs; i++) {
        if  (bxdfs[i]->MatchFlags(type)) {
            pdf += bxdfs[i]->Pdf(woLocal, wiLocal);
            matchComp++;
        }
    }
    return matchComp > 0 ? pdf / matchComp : 0;
}

RGBSpectrum BSDF::SampleF(const Vector3f& woWorld, Vector3f* wiWorld, const Point2f& sample,
    Float* pdf, const BxDFType& type, BxDFType* sampleType) {
    Vector3f woLocal = frame.toLocal(woWorld), wiLocal;

    int matchComp = NumComponents(type);
    if (matchComp == 0) {
        *pdf = 0;
        return RGBSpectrum(0.);
    }

    int chooseComp = std::min(int(sample[0] * matchComp), matchComp - 1), counter = chooseComp, idx;
    for (int i = 0; i < nBxDFs; i++) {
        if (bxdfs[i]->MatchFlags(type) && counter-- == 0) {
            idx = i;
            break;
        }
    }

    Point2f remapSample(sample[0] * matchComp - chooseComp, sample[1]);
    RGBSpectrum f(0.);
    f = bxdfs[idx]->SampleF(woLocal, &wiLocal, remapSample, pdf);

    if (*pdf == 0) {
        if (sampleType) *sampleType = BxDFType(0);
        return RGBSpectrum(0.);
    }
    *wiWorld = frame.toWorld(wiLocal);

    if (!(bxdfs[idx]->MatchFlags(BSDF_SPECULAR)) && matchComp > 1) {
        for (int i = 0; i < nBxDFs; i++) {
            if (i != idx && bxdfs[i]->MatchFlags(type)) {
                *pdf += bxdfs[i]->Pdf(woLocal, wiLocal);
            }
        }
    }
    if (matchComp > 1) *pdf /= matchComp;

    if (!(bxdfs[idx]->MatchFlags(BSDF_SPECULAR))) {
        for (int i = 0; i < nBxDFs; i++) {
            if (i != idx && bxdfs[i]->MatchFlags(type)) {
                f += bxdfs[i]->f(woLocal, wiLocal);
            }
        }
    }

    return f;
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
	return fresnel->Evaluate(Frame::CosTheta(*wi)) * R / Frame::AbsCosTheta(*wi);
}

Float SpecularReflection::Pdf(const Vector3f & wo, const Vector3f & wi) {
    return 0;
}

RGBSpectrum SpecularTransmission::f(const Vector3f & wo, const Vector3f & wi) {
	return RGBSpectrum(0.0);
}

RGBSpectrum SpecularTransmission::SampleF(const Vector3f & wo, Vector3f * wi, const Point2f & sample, Float * pdf) {
	bool enter = Frame::CosTheta(wo) > 0;
	Float etaI = enter ? etaA : etaB;	
	Float etaT = enter ? etaB : etaA;

	if (!Refract(wo, FaceForward(Normal3f(0, 0, 1), wo), etaI / etaT, wi))
		return RGBSpectrum(0.0);
	*pdf = 1;

	return (etaI*etaI) / (etaT*etaT) * (RGBSpectrum(1.0) - fresnel.Evaluate(Frame::CosTheta(*wi))) * T / Frame::AbsCosTheta(*wi);
}

Float SpecularTransmission::Pdf(const Vector3f & wo, const Vector3f & wi) {
    return 0;
}


RGBSpectrum LambertianReflection::f(const Vector3f & wo, const Vector3f & wi) {
	return R * INV_PI;
}

RGBSpectrum LambertianReflection::SampleF(const Vector3f & woWorld, Vector3f * wiWorld, const Point2f & sample, Float * pdf) {	
    Vector3f wo=woWorld, wi = CosineSampleHemisphere(sample);
	if (wo.z < 0) wi.z *= -1;
	*pdf = Frame::SameHemisphere(wo, wi) ? Frame::AbsCosTheta(wi) * INV_PI : 0;
    *wiWorld = wi;
	return R * INV_PI;
}

Float LambertianReflection::Pdf(const Vector3f & wo, const Vector3f & wi) {
    return INV_TWOPI;
}

RAINBOW_NAMESPACE_END

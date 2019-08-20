#include "bsdf.h"
#include "microfacet.h"

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

RGBSpectrum FrConductor(
    Float                cosThetaI, 
    const RGBSpectrum&   etai,
    const RGBSpectrum&   etat, 
    const RGBSpectrum&   k) 
{
    cosThetaI = Clamp(cosThetaI, -1, 1);
    RGBSpectrum eta = etat / etai;
    RGBSpectrum etak = k / etai;

    Float cosThetaI2 = cosThetaI * cosThetaI;
    Float sinThetaI2 = 1. - cosThetaI2;
    RGBSpectrum eta2 = eta * eta;
    RGBSpectrum etak2 = etak * etak;

    RGBSpectrum t0 = eta2 - etak2 - sinThetaI2;
    RGBSpectrum a2plusb2 = Sqrt(t0 * t0 + 4 * eta2 * etak2);
    RGBSpectrum t1 = a2plusb2 + cosThetaI2;
    RGBSpectrum a = Sqrt(0.5f * (a2plusb2 + t0));
    RGBSpectrum t2 = (Float)2 * cosThetaI * a;
    RGBSpectrum Rs = (t1 - t2) / (t1 + t2);

    RGBSpectrum t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
    RGBSpectrum t4 = t2 * sinThetaI2;
    RGBSpectrum Rp = Rs * (t3 - t4) / (t3 + t4);

    return 0.5 * (Rp + Rs);
}

inline bool Refract(
    const Vector3f&   wi, 
    const Normal3f&   n, 
    Float             eta, 
    Vector3f*         wt) 
{
	Float cosThetaI = Dot(wi, n);
	Float sin2ThetaI = std::max(0.f, 1.f - cosThetaI * cosThetaI);
	Float sin2ThetaT = eta * eta * sin2ThetaI;
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

RGBSpectrum BSDF::f(
    const Vector3f&   woWorld, 
    const Vector3f&   wiWorld, 
    const BxDFType&   type) 
{
    Vector3f woLocal = frame.toLocal(woWorld), wiLocal= frame.toLocal(wiWorld);
    bool reflect = Dot(ng, wiWorld)*Dot(ng, woWorld) > 0;
    RGBSpectrum f(0.);
    for (int i = 0; i < nBxDFs; i++) {
        if (bxdfs[i]->MatchFlags(type) && 
            ((reflect && (bxdfs[i]->type & BxDFType::BSDF_REFLECTION)) ||
           ((!reflect && (bxdfs[i]->type & BxDFType::BSDF_REFRACTION))))) {
            f += bxdfs[i]->f(woLocal, wiLocal);
        }
    }
    return f;
}

Float BSDF::Pdf(
    const Vector3f&   woWorld, 
    const Vector3f&   wiWorld, 
    const BxDFType&   type) 
{
    if (nBxDFs == 0) return 0;
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

/*
 * SampleF
 * 
 * Param type
 * the BxDF type wanted
 * 
 * Param sampleType
 * the BxDF type gotten
 */
RGBSpectrum BSDF::SampleF(
    const Vector3f&   woWorld, 
    Vector3f*         wiWorld, 
    const Point2f&    sample,
    Float*            pdf, 
    const BxDFType&   type, 
    BxDFType*         sampleType) 
{    
    int matchComp = NumComponents(type);
    if (matchComp == 0) {
        *pdf = 0;
        if (sampleType) *sampleType = BxDFType(0);
        return RGBSpectrum(0.);
    }

    int chooseComp = std::min((int)std::floor(sample[0] * matchComp), matchComp - 1), 
        counter = chooseComp;

    BxDF* bxdf = nullptr;
    for (int i = 0; i < nBxDFs; i++) {
        if (bxdfs[i]->MatchFlags(type) && counter-- == 0) {
            bxdf = bxdfs[i];
            break;
        }
    }

    Point2f remappedSample(std::min(sample[0] * matchComp - chooseComp, 1 - Epsilon), sample[1]);

    RGBSpectrum f(0.);
    Vector3f woLocal = frame.toLocal(woWorld), wiLocal;
    if (woLocal.x < -1 || woLocal.x>1) {
        frame.toLocal(woWorld);
    }
    if (sampleType) *sampleType = bxdf->type;
    *pdf = 0;
    f = bxdf->SampleF(woLocal, &wiLocal, remappedSample, pdf);

    if (*pdf == 0) {
        if (sampleType) *sampleType = BxDFType(0);
        return RGBSpectrum(0.);
    }
    *wiWorld = frame.toWorld(wiLocal);
    if (std::fabs(wiWorld->Length() - 1.f) > Epsilon) {
        bxdf->SampleF(woLocal, &wiLocal, remappedSample, pdf);
        frame.toWorld(wiLocal);
    }

    if (!(bxdf->type & BSDF_SPECULAR) && matchComp > 1) {
        for (int i = 0; i < nBxDFs; i++) {
            if (bxdfs[i] != bxdf && bxdf->MatchFlags(type)) {
                *pdf += bxdf->Pdf(woLocal, wiLocal);
            }
        }
    }
    if (matchComp > 1) *pdf /= matchComp;

    if (!(bxdf->type & BSDF_SPECULAR)) {
        bool reflect = Dot(ng, *wiWorld) * Dot(ng, woWorld) > 0;
        f = RGBSpectrum(0.);
        for (int i = 0; i < nBxDFs; i++) {
            if (bxdfs[i]->MatchFlags(type) &&
                ((reflect && (bxdf->type & BxDFType::BSDF_REFLECTION)) ||
                ((!reflect && (bxdf->type & BxDFType::BSDF_REFRACTION))))) {
                f += bxdf->f(woLocal, wiLocal);
            }
        }
    }

    return f;
}

RGBSpectrum FresnelDielectric::Evaluate(Float cosThetaI) const {
	return FrDielectric(cosThetaI, etaI, etaT);
}

RGBSpectrum FresnelConductor::Evaluate(Float cosThetaI) const {
    return FrConductor(cosThetaI, etaI, etaT, k);
}

RGBSpectrum SpecularReflection::SampleF(
    const Vector3f&   wo, 
    Vector3f*         wi, 
    const Point2f&    sample, 
    Float*            pdf) 
{
	*pdf = 1;
	*wi = Vector3f(-wo.x, -wo.y, wo.z);
	return fresnel->Evaluate(Frame::CosTheta(*wi)) * R;
}

RGBSpectrum SpecularRefract::SampleF(
    const Vector3f&   wo, 
    Vector3f*         wi, 
    const Point2f&    sample, 
    Float*            pdf) 
{
	bool enter = Frame::CosTheta(wo) > 0;
    
	Float etaI = enter ? etaA : etaB;	
	Float etaT = enter ? etaB : etaA;

	if (!Refract(wo, FaceForward(Normal3f(0, 0, 1), wo), etaI / etaT, wi))
		return RGBSpectrum(0.0);
    RGBSpectrum Ft = T * (RGBSpectrum(1.0) - fresnel.Evaluate(Frame::CosTheta(*wi)));
	*pdf = 1;
	return (etaI*etaI) / (etaT*etaT) * Ft;
}

RGBSpectrum SpecularTransmission::SampleF(
    const Vector3f& wo, 
    Vector3f* wi, 
    const Point2f& sample, 
    Float* pdf) 
{
    Float F = FrDielectric(Frame::CosTheta(wo), etaA, etaB);
    if (sample[0] < F) {
        // Compute specular reflection for _FresnelSpecular_

        // Compute perfect specular reflection direction
        *wi = Vector3f(-wo.x, -wo.y, wo.z);
        //if (sampledType)
        //    *sampledType = BxDFType(BSDF_SPECULAR | BSDF_REFLECTION);
        *pdf = F;
        return F * R;
    }
    else {
        // Compute specular transmission for _FresnelSpecular_

        // Figure out which $\eta$ is incident and which is transmitted
        bool entering = Frame::CosTheta(wo) > 0;
        Float etaI = entering ? etaA : etaB;
        Float etaT = entering ? etaB : etaA;

        // Compute ray direction for specular transmission
        if (!Refract(wo, FaceForward(Normal3f(0, 0, 1), wo), etaI / etaT, wi))
            return RGBSpectrum(0.);
        RGBSpectrum ft = T * (1 - F);

        // Account for non-symmetry with transmission to different medium
        //if (mode == TransportMode::Radiance)
            ft *= (etaI * etaI) / (etaT * etaT);
        //if (sampledType)
        //    *sampledType = BxDFType(BSDF_SPECULAR | BSDF_REFRACTION);
        *pdf = 1 - F;
        return ft;
    }
}

RGBSpectrum LambertianReflection::SampleF(
    const Vector3f&   wo, 
    Vector3f*         wi, 
    const Point2f&    sample, 
    Float*            pdf) 
{	
    *wi = CosineSampleHemisphere(sample);
	if (wo.z < 0) (*wi).z *= -1;
	*pdf = Frame::SameHemisphere(wo, *wi) ? Frame::AbsCosTheta(*wi) * INV_PI : 0;
	return R * INV_PI * Frame::AbsCosTheta(*wi);
}

RGBSpectrum MicrofacetReflection::f(
    const Vector3f&   wo, 
    const Vector3f&   wi) 
{
    Float cosThetaI = Frame::AbsCosTheta(wi), cosThetaO = Frame::AbsCosTheta(wo);
    Vector3f wh = wi + wo;
    if (cosThetaI == 0 || cosThetaO == 0) return RGBSpectrum(0.);
    if (wh.x == 0 && wh.y == 0 && wh.z == 0) return RGBSpectrum(0.);
    wh = Normalize(wh);
    RGBSpectrum F = fresnel->Evaluate(Dot(wo, wh)) * R;
    return distribution->D(wh)*distribution->G(wo, wi)*F / (4 * cosThetaO);
}

RGBSpectrum MicrofacetReflection::SampleF(
    const Vector3f&   wo, 
    Vector3f*         wi, 
    const Point2f&    sample, 
    Float*            pdf) 
{
    if (wo.z == 0) return 0.;
    Vector3f wh = distribution->SampleWh(wo, sample);
    *wi = Reflect(wo, wh);
    if (!Frame::SameHemisphere(wo, *wi)) return RGBSpectrum(0.);

    *pdf = distribution->Pdf(wo, wh) / (4 * Dot(wo, wh));
    return f(wo, *wi);
}

Float MicrofacetReflection::Pdf(
    const Vector3f& wo, 
    const Vector3f& wi) 
{    
    if (!Frame::SameHemisphere(wo, wi)) return 0;
    Vector3f wh = Normalize(wi + wo);
    return  distribution->Pdf(wo, wh) * (4 * Dot(wo, wh));    
}

RAINBOW_NAMESPACE_END


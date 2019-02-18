#ifndef __BSDF_H
#define __BSDF_H

#include "vector.h"
#include "spectrum.h"
#include "sampling.h"
#include "frame.h"

RAINBOW_NAMESPACE_BEGIN

inline Vector3f Reflect(const Vector3f& wi, const Normal3f& n) {
    return 2 * Dot(wi, n) * n - wi;
}

inline Vector3f Reflect(const Vector3f& wi, const Vector3f& n) {
    return 2 * Dot(wi, n) * n - wi;
}

inline bool Refract(const Vector3f & wi, const Normal3f & n, Float eta, Vector3f* wt);

inline Float FrDielectric(Float cosTheta, Float etaI, Float etaT);
inline RGBSpectrum FrConductor(Float cosThetaI, const RGBSpectrum &etai, 
    const RGBSpectrum &etat, const RGBSpectrum &k);

class Fresnel {
public:
    virtual ~Fresnel() = default;
    virtual RGBSpectrum Evaluate(Float cosThetaI) const = 0;
};

class FresnelDielectric :public Fresnel {
public:
    FresnelDielectric(const Float& m_etaI, const Float& m_etaT) :etaI(m_etaI), etaT(m_etaT) {}
    RGBSpectrum Evaluate(Float cosThetaI) const;

    Float etaI, etaT;
};

class FresnelConductor :public Fresnel {
public:
    FresnelConductor(const RGBSpectrum& m_etaI, const RGBSpectrum &m_etaT, const RGBSpectrum& m_k)
        :etaI(m_etaI), etaT(m_etaT), k(m_k) {};
    RGBSpectrum Evaluate(Float cosThetaI) const;

    RGBSpectrum etaI, etaT, k;
};

class FresnelNoOp :public Fresnel {
public:
    RGBSpectrum Evaluate(Float) const { return RGBSpectrum(1.); }
};

enum BxDFType {
    BSDF_REFLECTION = 1 << 0,
    BSDF_TRANSMISSION = 1 << 1,

    BSDF_DIFFUSE = 1 << 2,
    BSDF_GLOSSY = 1 << 3,
    BSDF_SPECULAR = 1 << 4,

    BSDF_ALL = (1 << 5) - 1,
};

class BSDF {
public:
    BSDF(const Normal3f& m_n) :nBxDFs(0), frame(m_n) {}

    void Add(BxDF* bxdf) {        
        Assert(nBxDFs < MaxBxDFs, "Too many BxDFs!");
        bxdfs[nBxDFs++] = bxdf;
    }

    int NumComponents(const BxDFType &flags = BSDF_ALL) const;

    RGBSpectrum f(const Vector3f& woWorld, const Vector3f& wiWorld, const BxDFType& type = BSDF_ALL);
    RGBSpectrum SampleF(const Vector3f& woWorld, Vector3f* wiWorld, const Point2f &sample, 
        Float *pdf, const BxDFType& type = BSDF_ALL, BxDFType* sampleType = nullptr);
    Float Pdf(const Vector3f& woWorld, const Vector3f& wiWorld, const BxDFType& type = BSDF_ALL);

private:
    static constexpr int MaxBxDFs = 8;
    int nBxDFs;
    BxDF *bxdfs[MaxBxDFs];
    Frame frame;
};

class BxDF{
public:	
    BxDF(const BxDFType& m_type) :type(m_type) {}
	virtual RGBSpectrum f(const Vector3f& wo, const Vector3f& wi) = 0;
    virtual RGBSpectrum SampleF(const Vector3f& wo, Vector3f* wi, const Point2f &sample, Float *pdf) = 0;
    virtual Float Pdf(const Vector3f& wo, const Vector3f& wi) = 0;

	bool MatchFlags(const BxDFType& t) const {
        // return true iff type is a subset of t
		return (type & t) == type;
	}	

	const BxDFType type;
};

class SpecularReflection :public BxDF {
public:
	SpecularReflection(const RGBSpectrum& m_R, Fresnel* m_fresnel) :
		R(m_R), fresnel(m_fresnel),
		BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)) {}

	RGBSpectrum f(const Vector3f& wo, const Vector3f& wi);
	RGBSpectrum SampleF(const Vector3f& wo, Vector3f* wi, const Point2f &sample, Float *pdf);
    Float Pdf(const Vector3f& wo, const Vector3f& wi);

	const RGBSpectrum R;
	const Fresnel *fresnel;
};

class SpecularTransmission :public BxDF {
public:
	SpecularTransmission(const RGBSpectrum& m_T, const Float& m_etaA, const Float& m_etaB) :
		T(m_T), etaA(m_etaA), etaB(m_etaB), fresnel(etaA, etaB),
		BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)) {}
	RGBSpectrum f(const Vector3f& wo, const Vector3f& wi);
	RGBSpectrum SampleF(const Vector3f& wo, Vector3f* wi, const Point2f &sample, Float *pdf);
    Float Pdf(const Vector3f& wo, const Vector3f& wi);

	const RGBSpectrum T;
	const Float etaA, etaB;
	const FresnelDielectric fresnel;
};

class LambertianReflection :public BxDF {
public:
	LambertianReflection(const RGBSpectrum& m_R) :
		BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(m_R) {}
	RGBSpectrum f(const Vector3f& wo, const Vector3f& wi) override;
	RGBSpectrum SampleF(const Vector3f& wo, Vector3f* wi, const Point2f &sample, Float *pdf) override;
    Float Pdf(const Vector3f& wo, const Vector3f& wi) override;

	const RGBSpectrum R;
};

class MicrofacetReflection :public BxDF {
public:
    MicrofacetReflection(
        const RGBSpectrum& m_R, 
        MicrofacetDistribution* m_dis, 
        Fresnel* m_fresnel)
        :
        BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)), 
        R(m_R),
        distribution(m_dis), 
        fresnel(m_fresnel) {}

    RGBSpectrum f(const Vector3f& wo, const Vector3f& wi) override;
    RGBSpectrum SampleF(const Vector3f& wo, Vector3f* wi, const Point2f &sample, Float *pdf) override;
    Float Pdf(const Vector3f& wo, const Vector3f& wi) override;

    RGBSpectrum R;
    MicrofacetDistribution* distribution;
    Fresnel* fresnel;
};

RAINBOW_NAMESPACE_END

#endif // !__BSDF_H

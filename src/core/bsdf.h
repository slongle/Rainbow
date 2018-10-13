#ifndef __BSDF_H
#define __BSDF_H

#include "common.h"
#include "object.h"
#include "spectrum.h"

RAINBOW_NAMESPACE_BEGIN

inline Float FrDielectric(Float cosTheta, Float etaI, Float etaT);

inline Float CosTheta(const Vector3f& v) { return v.z; }
inline Float Cos2Theta(const Vector3f& v) { return v.z*v.z; }
inline Float AbsCosTheta(const Vector3f& v) { return std::abs(v.z); }
inline Float Sin2Theta(const Vector3f& v) { return std::max(Float(0), 1 - Cos2Theta(v)); }
inline Float SinTheta(const Vector3f& v) { return std::sqrt(Sin2Theta(v)); }


inline Vector3f Reflect(const Vector3f& wi, const Normal3f& n) {
	return 2 * Dot(wi, n) * n - wi;
}

inline bool Refract(const Vector3f & wi, const Normal3f & n, Float eta, Vector3f* wt);

class BxDF{
public:
	enum BxDFType {
		BSDF_REFLECTION = 1 << 0,
		BSDF_TRANSMISSION = 1 << 1,
		BSDF_DIFFUSE = 1 << 2,
		BSDF_GLOSSY = 1 << 3,
		BSDF_SPECULAR = 1 << 4,
		BSDF_ALL = (1 << 5) - 1,
	};

	BxDF(const BxDFType& m_type) :type(m_type) {}
	virtual RGBSpectrum f(const Vector3f& wo, const Vector3f& wi) = 0;
	virtual RGBSpectrum sampleF(const Vector3f& wo, Vector3f* wi, const Point2f &sample, Float *pdf) = 0;

	bool MatchFlags(const BxDFType& t) const {
		return (type & t) == type;
	}	

	const BxDFType type;
};

class Fresnel {
public:
	virtual RGBSpectrum Evaluate(Float cosThetaI) const = 0;
};

class FresnelDielectric :public Fresnel {
public:
	FresnelDielectric(const Float& m_etaI, const Float& m_etaT) :etaI(m_etaI), etaT(m_etaT) {}
	RGBSpectrum Evaluate(Float cosThetaI) const;
	Float etaI, etaT;
};

class SpecularReflection :public BxDF {
public:
	SpecularReflection(const RGBSpectrum& m_R, Fresnel* m_fresnel) :
		R(m_R), fresnel(m_fresnel),
		BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)) {}

	RGBSpectrum f(const Vector3f& wo, const Vector3f& wi);
	RGBSpectrum sampleF(const Vector3f& wo, Vector3f* wi, const Point2f &sample, Float *pdf);

	const RGBSpectrum R;
	const Fresnel *fresnel;
};

class SpecularTransmission :public BxDF {
public:
	SpecularTransmission(const RGBSpectrum& m_T, const Float& m_etaA, const Float& m_etaB) :
		T(m_T), etaA(m_etaA), etaB(m_etaB), fresnel(etaA, etaB),
		BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)) {}
	RGBSpectrum f(const Vector3f& wo, const Vector3f& wi);
	RGBSpectrum sampleF(const Vector3f& wo, Vector3f* wi, const Point2f &sample, Float *pdf);

	const RGBSpectrum T;
	const Float etaA, etaB;
	const FresnelDielectric fresnel;
};

class LambertianReflection :public BxDF {
public:
	LambertianReflection(const RGBSpectrum& m_R) :
		BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(m_R) {}
	RGBSpectrum f(const Vector3f& wo, const Vector3f& wi);
	//RGBSpectrum sampleF(const Vector3f& wo, Vector3f* wi, const Point2f &sample, Float *pdf);

	const RGBSpectrum R;
};


RAINBOW_NAMESPACE_END

#endif // !__BSDF_H

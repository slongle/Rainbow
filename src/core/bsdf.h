#ifndef __BSDF_H
#define __BSDF_H

#include "vector.h"
#include "spectrum.h"
#include "sampling.h"

RAINBOW_NAMESPACE_BEGIN

inline Float FrDielectric(Float cosTheta, Float etaI, Float etaT);

inline Vector3f Reflect(const Vector3f& wi, const Normal3f& n) {
	return 2 * Dot(wi, n) * n - wi;
}

inline bool Refract(const Vector3f & wi, const Normal3f & n, Float eta, Vector3f* wt);

inline bool SameHemisphere(const Vector3f &w, const Vector3f &wp) {
	return w.z * wp.z > 0;
}

class Frame {
public:
    // Guarantee n is Normalized
    Frame(const Normal3f& m_n) :n(m_n) {
        if (std::abs(n.x) > std::abs(n.y)) {
            Float invLen = Float(1) / std::sqrt(n.x*n.x + n.z*n.z);
            s = Vector3f(n.z*invLen, 0, -n.x*invLen);
        }
        else {
            Float invLen = Float(1) / std::sqrt(n.y*n.y + n.z*n.z);
            s = Vector3f(0, n.z*invLen, -n.y*invLen);
        }        
        t = Cross(s, n);
    }

    Vector3f toLocal(const Vector3f &v) {
        return Vector3f(Dot(v, s), Dot(v, t), Dot(v, n));
    }

    Vector3f toWorld(const Vector3f v) {
        return v.x * s + v.y * t + v.z * n;
    }

    inline static Float CosTheta(const Vector3f& v) { return v.z; }
    inline static Float Cos2Theta(const Vector3f& v) { return v.z*v.z; }
    inline static Float AbsCosTheta(const Vector3f& v) { return std::abs(v.z); }
    inline static Float Sin2Theta(const Vector3f& v) { return std::max(Float(0), 1 - Cos2Theta(v)); }
    inline static Float SinTheta(const Vector3f& v) { return std::sqrt(Sin2Theta(v)); }


private:
    Normal3f n;
    Vector3f s, t;    
};

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
	virtual RGBSpectrum SampleF(const Vector3f& wo, Vector3f* wi, const Point2f &sample, Float *pdf) = 0;
    virtual Float Pdf(const Vector3f& wo, const Vector3f& wi) = 0;

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
	RGBSpectrum f(const Vector3f& wo, const Vector3f& wi);
	RGBSpectrum SampleF(const Vector3f& wo, Vector3f* wi, const Point2f &sample, Float *pdf);
    Float Pdf(const Vector3f& wo, const Vector3f& wi);

	const RGBSpectrum R;
};


RAINBOW_NAMESPACE_END

#endif // !__BSDF_H

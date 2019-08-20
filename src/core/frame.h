#ifndef __FRAME_H
#define __FRAME_H

#include "vector.h"

RAINBOW_NAMESPACE_BEGIN

inline void CoordinateSystem(const Normal3f& n,  Vector3f *s, Vector3f *t) {
    if (std::abs(n.x) > std::abs(n.y)) {
        Float invLen = Float(1) / std::sqrt(n.x*n.x + n.z*n.z);
        *s = Vector3f(n.z*invLen, 0, -n.x*invLen);
    }
    else {
        Float invLen = Float(1) / std::sqrt(n.y*n.y + n.z*n.z);
        *s = Vector3f(0, n.z*invLen, -n.y*invLen);
    }
    *t = Cross(*s, n);
}

class Frame {
public:
    Frame() {}

    // Guarantee n is Normalized
    Frame(const Normal3f& n) :m_n(n) {
        CoordinateSystem(m_n, &m_s, &m_t);
    }

    Frame(const Normal3f& n, const Vector3f& s, const Vector3f& t) 
        :m_n(n), m_s(s), m_t(t) {}

    Vector3f toLocal(const Vector3f &v) const {
        return Normalize(Vector3f(Dot(v, m_s), Dot(v, m_t), Dot(v, m_n)));
    }

    Vector3f toWorld(const Vector3f v) const {
        return Normalize(v.x * m_s + v.y * m_t + v.z * m_n);
    }

    inline static Float CosTheta(const Vector3f& v) { return v.z; }
    inline static Float Cos2Theta(const Vector3f& v) { return v.z*v.z; }
    inline static Float AbsCosTheta(const Vector3f& v) { return std::abs(v.z); }
    inline static Float SinTheta(const Vector3f& v) { return std::sqrt(Frame::Sin2Theta(v)); }
    inline static Float Sin2Theta(const Vector3f& v) { return std::max(Float(0), 1 - Frame::Cos2Theta(v)); }
    inline static Float TanTheta(const Vector3f& v) { return SinTheta(v) / CosTheta(v); }
    inline static Float Tan2Theta(const Vector3f& v) { return Sin2Theta(v) / Cos2Theta(v); }

    inline static Float CosPhi(const Vector3f &w) {
        Float sinTheta = SinTheta(w);
        return (sinTheta == 0) ? 1 : Clamp(w.x / sinTheta, -1, 1);
    }
    inline static Float SinPhi(const Vector3f &w) {
        Float sinTheta = SinTheta(w);
        return (sinTheta == 0) ? 0 : Clamp(w.y / sinTheta, -1, 1);
    }
    inline static Float Cos2Phi(const Vector3f &w) {
        return CosPhi(w) * CosPhi(w);
    }
    inline static Float Sin2Phi(const Vector3f &w) {
        return SinPhi(w) * SinPhi(w);
    }

    inline static Vector3f SphericalDirection(Float sinTheta, Float cosTheta, Float phi) {
        return Vector3f(sinTheta * std::cos(phi), sinTheta * std::sin(phi),
            cosTheta);
    }

    inline static bool SameHemisphere(const Vector3f &w, const Vector3f &wp) {
        return w.z * wp.z > 0;
    }

    Normal3f m_n;
    Vector3f m_s, m_t;
};

RAINBOW_NAMESPACE_END

#endif // !__FRAME_H

#pragma once
#ifndef __UTILITY_MATH_H
#define __UTILITY_MATH_H

namespace utility {
    typedef float Float;

    template<typename U,typename V>
    inline double max(U a, V b) {
        return (a > b) ? a : b;
    }

    template<typename U, typename V>
    inline double min(U a, V b) {
        return (a < b) ? a : b;
    }

    inline Float lerp(Float lValue, Float rValue, Float t) {
        return (1 - t) * lValue + t * rValue;
    }
}

#endif //__UTILITY_MATH_H

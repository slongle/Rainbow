#pragma once
#ifndef __UTILITY_MATH_H
#define __UTILITY_MATH_H

namespace utility {
    template<typename U,typename V>
    inline double max(U a, V b) {
        return (a > b) ? a : b;
    }

    template<typename U, typename V>
    inline double min(U a, V b) {
        return (a < b) ? a : b;
    }
}

#endif //__UTILITY_MATH_H

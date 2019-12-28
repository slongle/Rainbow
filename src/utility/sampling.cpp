#include "..\core\sampling.h"

RAINBOW_NAMESPACE_BEGIN

Distribution1D::Distribution1D(Float* f, int n)
    : func(f, f + n), cdf(n + 1)
{
    cdf[0] = 0;
    Float dx = 1. / n;
    for (int i = 1; i <= n; i++) {
        cdf[i] = cdf[i - 1] + func[i - 1] * dx;
    }
    funcInt = cdf[n];
    if (funcInt == 0) {
        for (int i = 1; i <= n; i++) {
            cdf[i] = Float(i) / n;
        }
    }
    else {
        Float invInt = 1. / funcInt;
        for (int i = 0; i <= n; i++) {
            cdf[i] *= invInt;
        }
    }
    //BuildAliasTable();
}

int Distribution1D::SampleDiscrete(Float u) const
{
    int l = 0, r = cdf.size();
    while (l < r) {
        int mid = (l + r) >> 1;
        if (cdf[mid] < u) {
            l = mid + 1;
        }
        else {
            r = mid;
        }
    }
    return l;
}

RAINBOW_NAMESPACE_END
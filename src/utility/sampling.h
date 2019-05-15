#pragma once
#ifndef __UTILITY_SAMPLING_H
#define __UTILITY_SAMPLING_H

#include <vector>
#include <cassert>
#include <algorithm>

namespace utility {
    

class Distribution1D {
public:
    Distribution1D(float* data,int n)
        : pdf(data, data + n), cdf(n + 1) 
    {
        sum = cdf[0] = 0;
        for (int i = 0; i < n; i++) sum += pdf[i];
        
        assert(sum > 0);

        for(int i=1;i<=n;i++) {
            cdf[i] = cdf[i - 1] + pdf[i - 1] / sum;
        }
        
    }

    int SampleContinus(float p) {              
        std::vector<float>::iterator ret = std::lower_bound(cdf.begin(), cdf.end(), p);
        return ret - cdf.begin();
    }


private:
    std::vector<float> pdf, cdf;
    float sum;  
};

}

#endif //__UTILITY_SAMPLING_H

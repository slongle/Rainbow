#ifndef __INDEPENDENT_H
#define __INDEPENDENT_H

#include "../core/sampler.h"

RAINBOW_NAMESPACE_BEGIN

class Independent :public Sampler {
public:
    Independent(){}
	Independent(int m_sampleCount) :sampleCount(m_sampleCount)
	{}
    
    std::unique_ptr<Sampler> Clone(const Point2i &seed);
	Float Get1D();
	Point2f Get2D();

	int sampleCount;
	pcg32 rng;
};

Independent* CreateIndependentSampler(PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // __INDEPENDENT_H

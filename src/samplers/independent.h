#ifndef __INDEPENDENT_H
#define __INDEPENDENT_H

#include "../core/sampler.h"

RAINBOW_NAMESPACE_BEGIN

class Independent :public Sampler {
public:
	Independent(int m_SamplerCount) :SamplerCount(m_SamplerCount) {}

	Float Get1D();
	Point2f Get2D();

	int SamplerCount;
	pcg32 rng;
};

Independent* CreateIndependentSampler(PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // __INDEPENDENT_H

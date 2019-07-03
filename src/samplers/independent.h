#ifndef __INDEPENDENT_H
#define __INDEPENDENT_H

#include "core/sampler.h"

RAINBOW_NAMESPACE_BEGIN

class Independent :public Sampler {
public:
    Independent(){}
	Independent(int m_sampleCount) 
    : sampleCount(m_sampleCount) {}
    
    std::unique_ptr<Sampler> Clone(const Point2i &seed) override;
	Float Next1D() override;
	Point2f Next2D() override;

	int sampleCount;	
};

Independent* CreateIndependentSampler(const PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // __INDEPENDENT_H

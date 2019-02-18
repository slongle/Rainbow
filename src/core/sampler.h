#ifndef __SAMPLER_H
#define __SAMPLER_H

#include "ext/pcg32/pcg32.h"

#include "vector.h"

RAINBOW_NAMESPACE_BEGIN

class Sampler {
public:
    virtual std::unique_ptr<Sampler> Clone(const Point2i &seed) = 0;
	virtual Float Get1D() = 0;
	virtual Point2f Get2D() = 0;
};

RAINBOW_NAMESPACE_END

#endif // __SAMPLER_H

#ifndef __SAMPLER_H
#define __SAMPLER_H

#include "ext/pcg32/pcg32.h"
#include "vector.h"

RAINBOW_NAMESPACE_BEGIN

class Sampler {
public:
    Sampler() {};
    virtual std::unique_ptr<Sampler> Clone(const Point2i &seed) = 0;
	virtual Float Next1D() = 0;
	virtual Point2f Next2D() = 0;

    Float nextFloat() {
        return random.nextFloat();
    }
protected:
    pcg32 random;
};

RAINBOW_NAMESPACE_END

#endif // __SAMPLER_H

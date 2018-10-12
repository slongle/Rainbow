#ifndef __BSDF_H
#define __BSDF_H

#include "common.h"
#include "object.h"

RAINBOW_NAMESPACE_BEGIN

class BxDF{
public:
	BxDF() {}	
};

class SpecularReflect :public BxDF {
public:

};

class SpecularTransmission :public BxDF {
public:

};

RAINBOW_NAMESPACE_END

#endif // !__BSDF_H

#ifndef __BSDF_H
#define __BSDF_H

#include "common.h"
#include "object.h"

RAINBOW_NAMESPACE_BEGIN

class BSDF :public Object{
public:
	BSDF() {}

	EClassType getClassType() const { return EBSDF; }
};

BSDF* CreateDiffuseBSDF(PropertyList &list) {
	return new BSDF();
}

RAINBOW_NAMESPACE_END

#endif // !__BSDF_H

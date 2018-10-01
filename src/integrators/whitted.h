#ifndef __WHITTED_H
#define __WHITTED_H

#include "../core/integrator.h"

RAINBOW_NAMESPACE_BEGIN

class WhittedIntegrator :public Integrator {
public:
	WhittedIntegrator() {}

	void Render(const Scene& scene) override;
};

WhittedIntegrator* CreateWhittedIntegrator(const PropertyList &list);

RAINBOW_NAMESPACE_END

#endif // __WHITTED_H

#ifndef __RAINBOW_GUI_H
#define __RAINBOW_GUI_H

#include "common.h"

RAINBOW_NAMESPACE_BEGIN

int show(Integrator* integrator, Scene* scene);
int AdaptiveShow(Integrator* integrator, Scene* scene);

RAINBOW_NAMESPACE_END

#endif // __RAINBOW_GUI_H

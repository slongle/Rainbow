#ifndef __RAINBOW_API_H
#define __RAINBOW_API_H

#include "propertylist.h"

RAINBOW_NAMESPACE_BEGIN

void RainbowInit();
void RainbowWorld();

void RainbowIntegrator(const std::string& type, const PropertyList& list);
void RainbowCamera(const std::string& type, const PropertyList& list);
void RainbowSampler(const std::string& type, const PropertyList& list);
void RainbowFilm(const std::string& type, const PropertyList& list);
void RainbowShape(const std::string& type, PropertyList& list);
void RainbowMaterial(const std::string& type, PropertyList& list);


RAINBOW_NAMESPACE_END

#endif // !__RAINBOW_API_H

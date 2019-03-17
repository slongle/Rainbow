#ifndef __RAINBOW_API_H
#define __RAINBOW_API_H

#include "propertylist.h"

RAINBOW_NAMESPACE_BEGIN

void RainbowSceneBegin();
void RainbowSceneEnd();

void RainbowRenderMode(const std::string& type);
void RainbowIntegrator(const std::string& type, const PropertyList& list);
void RainbowCamera(const std::string& type, const PropertyList& list);
void RainbowSampler(const std::string& type, const PropertyList& list);
void RainbowFilm(const std::string& type, const PropertyList& list);
void RainbowShape(const std::string& type, PropertyList& list);
void RainbowMaterial(const std::string& type, PropertyList& list);
void RainbowLight(const std::string& type, PropertyList& list);
void RainbowFilter(const std::string& type, PropertyList& list);
void RainbowRef(const std::string& type, PropertyList& list);
void RainbowBSDFMap(const std::string& type, PropertyList& list);
void RainbowMedium(const std::string& type, const std::string& name, PropertyList& list);

void InitialTransform();
void RainbowTransform(const Transform& ObjectToWorld);


RAINBOW_NAMESPACE_END

#endif // !__RAINBOW_API_H

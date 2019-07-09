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
void RainbowFilter(const std::string& type, PropertyList& list);
void RainbowLight(const std::string& type, PropertyList& list);
void RainbowShape(const std::string& type, const PropertyList& list);
void RainbowMaterial(const std::string& type, const PropertyList& list);
void RainbowMedium(const std::string& type, const std::string& name, const PropertyList& list);
void RainbowVolume(const std::string& type, const std::string& name, const PropertyList& list);
void RainbowNamedMaterial(const std::string &id, const std::string &type, const PropertyList &list);
void RainbowNamedMedium(const std::string &id, const std::string &type, 
                        const std::string &name, const PropertyList &list);
void RainbowRef(const std::string &name, const std::string &id);



RAINBOW_NAMESPACE_END

#endif // !__RAINBOW_API_H

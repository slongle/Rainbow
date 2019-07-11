#ifndef __PARSER_H
#define __PARSER_H

#include "api.h"
#include "ext/pugixml/pugixml.hpp"

RAINBOW_NAMESPACE_BEGIN

enum ETag {
    EHide,
    EScene,
    EMode,

    EIntegrator,
    ECamera,
    ESampler,
    EFilm,
    ERFilter,
    ELight,
    EShape,
    EBSDF,    
    EVolume,
    EMedium,
    ERef,

    EBoolean,
    EInteger,
    EFloat,
    EString,
    EVector,
    EColor,
    ETransform,
    ELookAt,
    ETranslate,
    EScale,
    ERotate,
    EMatrix
};

//void ParserXMLFile(const std::string &filename);

struct ParseRecord{
    ParseRecord(const std::string filename);

    const std::string m_filename;
    Transform m_transform;
    std::map<std::string, ETag> m_tags;

};

bool HasAttribute(const pugi::xml_node &node, const std::string &name);
std::string GetOffset(ptrdiff_t pos, ParseRecord &record);
void ParseTag(pugi::xml_node &node, PropertyList &fatherList, ParseRecord &record);
void HandleTag(pugi::xml_node &node, PropertyList &myList, PropertyList &fatherList, ParseRecord &record);
void Parse(ParseRecord &record);


RAINBOW_NAMESPACE_END

#endif // !__PARSER_H

#ifndef __PARSER_H
#define __PARSER_H

#include "api.h"
#include "object.h"

#include "../../ext/pugixml/pugixml.hpp"

RAINBOW_NAMESPACE_BEGIN

void ParserXMLFile(const std::string &filename);

RAINBOW_NAMESPACE_END

#endif // !__PARSER_H

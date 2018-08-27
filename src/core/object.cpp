#include "object.h"

RAINBOW_NAMESPACE_BEGIN

std::map<std::string, ObjectFactory::Constructor> *ObjectFactory::constructors = nullptr;

void ObjectFactory::registerClass(const std::string & name, const Constructor & constructor) {
	
	if (constructors == nullptr) constructors = new std::map<std::string, Constructor>;
	DCHECK(constructors->find(name) == constructors->end(), "Repeat Register Class");
	(*constructors)[name] = constructor;
}

Object * ObjectFactory::createInstance(const std::string & name, const PropertyList & list) {
	DCHECK((*constructors).find(name) != (*constructors).end(), "Lose " + name + "\'s Constructor");
	return (*constructors)[name](list);
}


RAINBOW_NAMESPACE_END


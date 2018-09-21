#include "object.h"

RAINBOW_NAMESPACE_BEGIN

/*std::map<std::string, ObjectFactory::Constructor> *ObjectFactory::constructors = nullptr;

void ObjectFactory::registerClass(const std::string & name, const Constructor & constructor) {
	if (constructors == nullptr) constructors = new std::map<std::string, Constructor>;
	Assert(constructors->find(name) == constructors->end(), "Repeat Register Class");
	(*constructors)[name] = constructor;
}

Object * ObjectFactory::createInstance(const std::string & name, const PropertyList & list) {
	Assert(constructors->find(name) != constructors->end(), "Missing " + name + "\'s Constructor");
	return (*constructors)[name](list);
}*/

void Object::setParent(Object * parent)
{
}

void Object::addChild(Object * child)
{
}

void Object::active()
{
}

RAINBOW_NAMESPACE_END


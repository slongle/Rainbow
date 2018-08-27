#ifndef __OBJECT_H
#define __OBJECT_H

#include "common.h"
#include "propertylist.h"

RAINBOW_NAMESPACE_BEGIN

class Object {
public:
	Object(const PropertyList &list) {}
};

class ObjectFactory {
public:
	typedef std::function<Object *(const PropertyList &)> Constructor;
	
	static void registerClass(const std::string &name, const Constructor &constructor);
	static Object* createInstance(const std::string &name, const PropertyList &list);
private:
	static std::map<std::string, Constructor> *constructors;
};

#define RAINBOW_REGISTER_CLASS(ClassName, XMLName)\
    static struct ClassName##Struct{ \
        ClassName##Struct() {\
            ObjectFactory::registerClass(XMLNAME, Create##ClassName);\
        }\
    }ClassName##_Rainbow;

RAINBOW_NAMESPACE_END

#endif // !__OBJECT_H

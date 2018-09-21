#ifndef __OBJECT_H
#define __OBJECT_H

#include "common.h"
#include "propertylist.h"

RAINBOW_NAMESPACE_BEGIN

class Object {
public:
	enum EClassType{
		EScene,
		ECamera,
		EBSDF,
		EFilm,
		EIntegrator,
		EShape,
		ESampler,
		ERFilter,
		ELight
	};

	virtual EClassType getClassType() const = 0;
	virtual void setParent(Object *parent);	
	virtual void addChild(Object *child);
	virtual void active();

	//virtual std::string toString() const = 0;

	static std::string classTypeName(EClassType type) {
		switch (type) {
			case EScene      : return "scene";    
			case ECamera     : return "camera";        
			case EBSDF       : return "BSDF";        
			case EFilm       : return "film";    
			case EIntegrator : return "integrator";  
			case EShape      : return "shape"; 
			case ESampler    : return "sampler"; 
			case ERFilter    : return "rfilter";
			default          : return "<unknown>";
		}
	}

};

/*class ObjectFactory {
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
            ObjectFactory::registerClass(XMLName, Create##ClassName);\
        }\
    }ClassName##_Rainbow;
*/

RAINBOW_NAMESPACE_END

#endif // !__OBJECT_H

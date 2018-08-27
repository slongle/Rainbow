#include"propertylist.h"

RAINBOW_NAMESPACE_BEGIN

#define ADD_PROPERTYLIST_FUNCTIONS(Type, Typename, XMLName)\
    void PropertyList::set##Typename(const std::string &name, const Type &value) { \
		Property property = list[name]; \
		property.type = Property::EP##Typename; \
		property.value.XMLName##_value = value; \
	} \
	\
    Type PropertyList::get##Typename(const std::string &name) { \
		DCHECK(list.find(name) != list.end(), "Not Found The Value Named" + name); \
		Property property = list[name]; \
		DCHECK(property.type == Property::EP##Typename, "Wrong Value's Type"); \
		return property.value.XMLName##_value; \
	}


/*void PropertyList::setBoolean(const std::string & name, const bool & value) {
	Property property = list[name];
	property.type = Property::EPBool;
	property.value.boolean_value = value;
}

bool PropertyList::getBoolean(const std::string & name) {
	DCHECK(list.find(name) != list.end(), "Not Found The Value Named" + name);
	Property property = list[name];
	DCHECK(property.type == Property::EPBool, "Wrong Value's Type");
	return property.value.boolean_value;
}*/

ADD_PROPERTYLIST_FUNCTIONS(bool, Boolean, boolean)
ADD_PROPERTYLIST_FUNCTIONS(int, Integer, integer)
ADD_PROPERTYLIST_FUNCTIONS(Float, Float, float)
ADD_PROPERTYLIST_FUNCTIONS(std::string, String, string)
ADD_PROPERTYLIST_FUNCTIONS(Vector3f, Vector, vector)
ADD_PROPERTYLIST_FUNCTIONS(Transform, Transform, transform)

RAINBOW_NAMESPACE_END

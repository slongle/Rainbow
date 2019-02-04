#include"propertylist.h"

RAINBOW_NAMESPACE_BEGIN

#define ADD_PROPERTYLIST_FUNCTIONS(Type, Typename, XMLName)\
    void PropertyList::set##Typename(const std::string &name, const Type &value) { \
		Property &property = list[name]; \
		property.type = Property::EP##Typename; \
		property.value.XMLName##_value = value; \
	} \
	\
    Type PropertyList::get##Typename(const std::string &name) { \
		Assert(list.find(name) != list.end(), "Not Found The Value Named" + name); \
		Property &property = list[name]; \
		Assert(property.type == Property::EP##Typename, "Wrong Value's Type"); \
		return property.value.XMLName##_value; \
	} \
    \
	Type PropertyList::get##Typename(const std::string &name, const Type & defaultValue) { \
		if (list.find(name) == list.end()) \
			return defaultValue; \
		Property &property = list[name]; \
		Assert(property.type == Property::EP##Typename, "Wrong Value's Type"); \
		return property.value.XMLName##_value; \
	}\
    \
    bool PropertyList::find##Typename(const std::string &name) {\
        if (list.find(name) != list.end()){\
            Assert(list[name].type == Property::EP##Typename, "Wrong Value's Type"); \
            return true;\
        }\
        else {\
            return false;\
        }\
    }\





ADD_PROPERTYLIST_FUNCTIONS(bool, Boolean, boolean)
ADD_PROPERTYLIST_FUNCTIONS(int, Integer, integer)
ADD_PROPERTYLIST_FUNCTIONS(Float, Float, float)
ADD_PROPERTYLIST_FUNCTIONS(std::string, String, string)
ADD_PROPERTYLIST_FUNCTIONS(Vector3f, Vector, vector)
ADD_PROPERTYLIST_FUNCTIONS(RGBSpectrum, Color, color)
ADD_PROPERTYLIST_FUNCTIONS(Transform, Transform, transform)


	/*void PropertyList::setString(const std::string &name, const std::string &v) {
		Property &property = list[name]; 
		property.type = Property::EPString;
		property.value.string_value = v;
	} 
	
    std::string PropertyList::getString(const std::string &name) {
		Assert(list.find(name) != list.end(), "Not Found The Value Named " + name);
		Property &property = list[name]; 
		Assert(property.type == Property::EPString, "Wrong Value's Type");
		return property.value.string_value;
	} 

	std::string PropertyList::getString(const std::string &name, const std::string & defaultValue) {
		if (list.find(name) == list.end()) 
			return defaultValue; 
		Property &property = list[name]; 
		Assert(property.type == Property::EPString, "Wrong Value's Type");
		return property.value.string_value;
	}
    
    bool PropertyList::findString(const std::string &name) {
        if (list.find(name) != list.end()){
            Assert(list[name].type == Property::EPString, "Wrong Value's Type");
            return true;
        }
        else {
            return false;
        }
    }
*/

RAINBOW_NAMESPACE_END

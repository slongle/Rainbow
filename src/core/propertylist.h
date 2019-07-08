#ifndef __PROPERTYLIST_H
#define __PROPERTYLIST_H

#include"spectrum.h"
#include"transform.h"

RAINBOW_NAMESPACE_BEGIN

struct Property {
	Property() {}

	enum {
		EPBoolean,
		EPInteger,
		EPFloat,
		EPString,
		EPVector,
		EPColor,
		EPTransform
	} type;

	struct Value {
		Value() {}

	    /*std::unique_ptr<bool>        boolean_value;
		std::unique_ptr<int>         integer_value;
		std::unique_ptr<Float>       float_value;
		std::unique_ptr<std::string> string_value;
		std::unique_ptr<Vector3f>    vector_value;
		std::shared_ptr<Transform>   transform_value;*/
		bool        boolean_value;
		int         integer_value;
		float       float_value;
		std::string string_value;
		Vector3f    vector_value;
		RGBSpectrum color_value;
		Transform   transform_value;
	} value; 
};

class PropertyList {
public:
	void setBoolean(const std::string &name, const bool &value);	
	bool getBoolean(const std::string &name) const;
	bool getBoolean(const std::string &name, const bool &defaultValue) const;
    bool findBoolean(const std::string &name) const;

	void setInteger(const std::string &name, const int &value);	
	int  getInteger(const std::string &name) const;
	int  getInteger(const std::string &name, const int &defaultValue) const;
    bool findInteger(const std::string &name) const;

	void  setFloat(const std::string &name, const Float &value);	
	Float getFloat(const std::string &name) const;
	Float getFloat(const std::string &name, const Float &defaultValue) const;
    bool findFloat(const std::string &name) const;

	void setString(const std::string &name, const std::string &value);	
	std::string getString(const std::string &name) const;
	std::string getString(const std::string &name, const std::string &defaultValue) const;
    bool findString(const std::string &name) const;

	void setVector(const std::string &name, const Vector3f &value);
	Vector3f getVector(const std::string &name) const;
	Vector3f getVector(const std::string &name, const Vector3f &defaultValue) const;
    bool findVector(const std::string &name) const;

	void setColor(const std::string &name, const RGBSpectrum &value);	
	RGBSpectrum getColor(const std::string &name) const;
	RGBSpectrum getColor(const std::string &name, const RGBSpectrum &defaultValue) const;
    bool findColor(const std::string &name) const;

	void setTransform(const std::string &name, const Transform &value);
	Transform getTransform(const std::string &name) const;
	Transform getTransform(const std::string &name, const Transform &defaultValue) const;
    bool findTransform(const std::string &name) const;

private:
	std::map<std::string, Property> list;
    
};

RAINBOW_NAMESPACE_END

#endif // !__PROPERTYLIST_H
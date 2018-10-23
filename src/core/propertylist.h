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
	bool getBoolean(const std::string &name);
	bool getBoolean(const std::string &name, const bool &defaultValue);

	void setInteger(const std::string &name, const int &value);
	int  getInteger(const std::string &name);
	int  getInteger(const std::string &name, const int &defaultValue);

	void  setFloat(const std::string &name, const Float &value);
	Float getFloat(const std::string &name);
	Float getFloat(const std::string &name, const Float &defaultValue);

	void setString(const std::string &name, const std::string &value);
	std::string getString(const std::string &name);
	std::string getString(const std::string &name, const std::string &defaultValue);

	void setVector(const std::string &name, const Vector3f &value);
	Vector3f getVector(const std::string &name);
	Vector3f getVector(const std::string &name, const Vector3f &defaultValue);

	void setColor(const std::string &name, const RGBSpectrum &value);
	RGBSpectrum getColor(const std::string &name);
	RGBSpectrum getColor(const std::string &name, const RGBSpectrum &defaultValue);

	void setTransform(const std::string &name, const Transform &value);
	Transform getTransform(const std::string &name);
	Transform getTransform(const std::string &name, const Transform &defaultValue);

private:
	std::map<std::string, Property> list;
};

RAINBOW_NAMESPACE_END

#endif // !__PROPERTYLIST_H
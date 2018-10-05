#include "parser.h"

RAINBOW_NAMESPACE_BEGIN

Scene* ParserXMLFile(const std::string & filename,Object* a) {
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.c_str());
	

	/* Helper function: map a position offset in bytes to a more readable line/column value */
	auto offset = [&](ptrdiff_t pos) -> std::string {
		std::fstream is(filename);
		char buffer[1024];
		int line = 0, linestart = 0, offset = 0;
		while (is.good()) {
			is.read(buffer, sizeof(buffer));
			for (int i = 0; i < is.gcount(); ++i) {
				if (buffer[i] == '\n') {
					if (offset + i >= pos)
						return tfm::format("line %i, col %i", line + 1, pos - linestart);
					++line;
					linestart = offset + i;
				}
			}
			offset += (int)is.gcount();
		}
		return "byte offset " + std::to_string(pos);
	};

	Assert(result, "Error while parsing \"" + filename + "\": " + result.description() + " (at " + offset(result.offset) + ")");
	
	auto checkAttribute = [&](const pugi::xml_node &node, const std::string &name) -> bool {
		for (pugi::xml_attribute &attribute : node.attributes()) {
			if (attribute.name() == name)
				return true;
		}
		return false;
	};

	enum ETag {
		EScene      = Object::EScene,   
		ECamera     = Object::ECamera,  
		EBSDF       = Object::EBSDF,          
		EFilm       = Object::EFilm,        
		EIntegrator = Object::EIntegrator,         
		EShape      = Object::EPrimitive,         
		ESampler    = Object::ESampler,         
		ERFilter    = Object::ERFilter,    
		ELight      = Object::ELight,
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

	std::map<std::string, ETag> tags;
	tags["scene"]       = EScene;
	tags["integrator"]  = EIntegrator;
	tags["sensor"]      = ECamera;
	tags["camera"]      = ECamera;
	tags["sampler"]     = ESampler;
	tags["film"]        = EFilm;
	tags["bsdf"]        = EBSDF;
	tags["shape"]       = EShape;
	tags["emitter"]     = ELight;
	tags["rfilter"]     = ERFilter;
	tags["ref"]         = ERef;
					    
	tags["bool"]        = EBoolean;
	tags["integer"]     = EInteger;
	tags["float"]       = EFloat;
	tags["string"]      = EString;
	tags["vector"]      = EVector;
	tags["spectrum"]    = EColor;
	tags["rgb"]         = EColor;

	tags["transform"]  = ETransform;
	tags["lookAt"]     = ELookAt;
	tags["translate"]  = ETranslate;
	tags["scale"]      = EScale;
	tags["rotate"]     = ERotate;
	tags["matrix"]     = EMatrix;


	Transform m_transform;
	std::map<std::string, Object*> references;

	std::function<Object *(pugi::xml_node, PropertyList&, int)> ParserTag  =
		[&](pugi::xml_node node, PropertyList &list, int num) -> Object* {
		//std::cout << node.find_attribute("id").value() << std::endl;
		//std::cout << node.name() << std::endl;

		if (node.type() == pugi::node_comment || node.type() == pugi::node_declaration)
			return nullptr;

		if (tags.find(node.name()) == tags.end())
			return nullptr;


		ETag tag = tags[node.name()];
		//std::cout << tag << std::endl;
		if (tag == EScene) {
			node.append_attribute("type");
			node.attribute("type").set_value("scene");
		}
		else if (tag == ETransform)
			m_transform.Identify();
		else if (tag == ERef)
			return references[node.attribute("id").value()];
		
		PropertyList m_list;
		std::vector<Object *> children;
		for (pugi::xml_node &child : node.children()) {
			Object *childObject = ParserTag(child, m_list, num + 4);
			if (childObject) {
				children.push_back(childObject);
				if (tag == EScene && checkAttribute(child,"id"))
					references[child.attribute("id").value()] = childObject;
			}
		}

		Object *result = nullptr;

		bool isObject = tag < EBoolean;

		if (isObject) {
			Assert(checkAttribute(node, "type"), "Missing attribute \"type\" in " + 
				                        static_cast<std::string>(node.name()));
			 
			if (tag == ERef) result = references[static_cast<std::string>(node.attribute("id").value())];
			else {
				std::string name = static_cast<std::string>(node.attribute("type").value());
				switch (tag) {
					case EScene:
						for (Object *child : children) {

						}
						result = MakeScene();
						break;
					case EIntegrator:
						result = MakeIntegrator(name, m_list);
						break;
					case ECamera: 
						{
						Film* film = nullptr;
						for (Object *child : children) {							
							if (child->getClassType() == EFilm) {
								film = static_cast<Film*>(child);
								break;
							}
						}
						Assert(film != nullptr, "No Film for Camera!");
						result = MakeCamera(name, m_list, film);
						break;
						}
					//case ESampler:
					//	result = MakeSampler();
					//	break;
					case EFilm:
						result = MakeFilm(name, m_list);
						break;
					case EBSDF:
						result = MakeBSDF(name, m_list);
						break;
					case EShape:
						result = &MakeShape(name, m_list);						
						break;
					//case ELight:
					//	result = MakeLight();
					//	break;
				}
			}

			///*
			//if (tag == EFilm) node.attribute("type").set_value("film");

			//result = ObjectFactory::createInstance(name, 
                                                   //m_list);

			if (result != nullptr) {
				for (Object *child : children) {
					//child->setParent(result);
					result->addChild(child);
				}
				if (result->getClassType() == EScene) {
					Scene* scene = static_cast<Scene*>(result);
					scene->aggregate = new Aggregate();
					scene->aggregate->primitives = scene->primitives;
				}
			}

			//if (node.find_attribute("id")!=pugi::xml_attribute()) {
			//	references[static_cast<std::string>(node.attribute("id").value())] = result;
			//}
			//result->active();
			
		}
		else {
			switch (tag) {
				case EBoolean: {
					list.setBoolean(node.attribute("name").value(),toBoolean(node.attribute("value").value()));
					break;
				}
				case EInteger: {
					list.setInteger(node.attribute("name").value(), toInteger(node.attribute("value").value()));
					break;
				}
				case EFloat: {
					list.setFloat(node.attribute("name").value(), toFloat(node.attribute("value").value()));
					break;
				}
				case EString:{
					list.setString(node.attribute("name").value(), node.attribute("value").value());
					break;
				}
				case EVector: {
					list.setVector(node.attribute("name").value(), toVector(node.attribute("value").value()));
					break;
				}
				case EColor: {
					if (node.name() == "spectrum") {
						// TODO: Fix Spectrum declared with wavelength
					}
					else if (node.name() == "rgb") {
						list.setColor(node.attribute("name").value(), toColor(node.attribute("value").value()));
					}
					break;
				}
				case ETransform: {
					list.setTransform(node.attribute("name").value(), m_transform);
					break;
				}
				case ELookAt: {
					Vector3f target = toVector(node.attribute("target").value());
					Vector3f origin = toVector(node.attribute("origin").value());
					Vector3f up = toVector(node.attribute("up").value());
					m_transform *= LookAt(target, origin, up);
					break;
				}
				case ETranslate: {
					Vector3f delta;
					for (pugi::xml_attribute &attribute : node.attributes()) {
						if (attribute.name() == "x") delta.x = toFloat(attribute.value());
						if (attribute.name() == "y") delta.y = toFloat(attribute.value());
						if (attribute.name() == "z") delta.z = toFloat(attribute.value());
					}
					m_transform *= Translate(delta);
					break;
				}
				case EScale: {
					Vector3f scale(1);
					for (pugi::xml_attribute &attribute : node.attributes()) {
						if (attribute.name() == "x") scale.x = toFloat(attribute.value());
						if (attribute.name() == "y") scale.y = toFloat(attribute.value());
						if (attribute.name() == "z") scale.z = toFloat(attribute.value());
					}
					m_transform *= Scale(scale);
					break;
				}
				case ERotate: {
					Vector3f axis; 
					Float angle;
					for (pugi::xml_attribute &attribute : node.attributes()) {
						if (attribute.name() == "x") axis.x = toFloat(attribute.value());
						if (attribute.name() == "y") axis.y = toFloat(attribute.value());
						if (attribute.name() == "z") axis.z = toFloat(attribute.value());
						if (attribute.name() == "angle") angle = toFloat(attribute.value());
					}
					m_transform *= Rotate(angle, axis);
					break;
				}
				case EMatrix: {
					Matrix4x4 mat = toMatrix(node.attribute("value").value());
					m_transform *= Transform(mat);
					break;
				}
			}
		}

		return result;
	};

	PropertyList prop;
	Scene* scene = static_cast<Scene*>(ParserTag(*doc.begin(), prop, 0));
	return scene;
}

RAINBOW_NAMESPACE_END
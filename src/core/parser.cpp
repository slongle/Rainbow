#include "parser.h"
#include "object.h"

RAINBOW_NAMESPACE_BEGIN

void ParserXMLFile(const std::string & filename) {
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
        EMode,

		EScene      	 ,
		ECamera     	 ,
		EBSDF            ,
        EBSDFMap         ,
		EFilm         	 ,
		EIntegrator      ,    
		EShape           ,   
		ESampler         , 
		ERFilter     	 ,
		ELight      	 ,
		ERef             ,
        EHide            ,     

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
    tags["mode"]        = EMode;

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
    tags["hide"]        = EHide;
					    
	tags["bool"]        = EBoolean;
	tags["integer"]     = EInteger;
	tags["float"]       = EFloat;
	tags["string"]      = EString;
	tags["vector"]      = EVector;
	tags["spectrum"]    = EColor;
	tags["rgb"]         = EColor;
	tags["color"]       = EColor;

	tags["transform"]  = ETransform;
	tags["lookat"]     = ELookAt;
	tags["lookAt"]     = ELookAt;
	tags["translate"]  = ETranslate;
	tags["scale"]      = EScale;
	tags["rotate"]     = ERotate;
	tags["matrix"]     = EMatrix;    

	Transform m_transform;	

	std::function<void (pugi::xml_node, PropertyList&, int)> ParserTag =
		[&](pugi::xml_node node, PropertyList &list, int num) -> void {
		
		if (node.type() == pugi::node_comment || node.type() == pugi::node_declaration) 
			return;

		if (tags.find(node.name()) == tags.end())
			return;

		ETag tag = tags[node.name()];
		if (tag == EScene) {
			node.append_attribute("type");
			node.attribute("type").set_value("scene");
		}
		else if (tag == ETransform) {
			m_transform.Identify();
		}
		else if (tag == EShape) {
			InitialTransform();
		}
        else if (tag==EBSDF) {
            if (node.attribute("id") != NULL) {
                tag = EBSDFMap;                
            }
        }
        else if (tag==ERef) {
            node.append_attribute("type");
            node.attribute("type").set_value("ref");
        }

		PropertyList m_list;
		for (pugi::xml_node &child : node.children()) {
			ParserTag(child, m_list, num + 4);
		}

		

		bool isObject = tag < EBoolean;

		if (isObject) {
			Assert(checkAttribute(node, "type"), "Missing attribute \"type\" in " +
				static_cast<std::string>(node.name()));
			std::string name = static_cast<std::string>(node.attribute("type").value());
			switch (tag) {
                case EMode:
                    RainbowRenderMode(name);
                    break;
				case EScene:
					break;
				case EIntegrator:
					RainbowIntegrator(name, m_list);
					break;
				case ECamera:			
					RainbowCamera(name, m_list);
					break;			
				case ESampler:
					RainbowSampler(name, m_list);
					break;
				case EFilm:
					RainbowFilm(name, m_list);
					break;
				case EBSDF:
					RainbowMaterial(name,m_list);
					break;
				case EShape:
					RainbowShape(name, m_list);
					break;
				case ELight:
					RainbowLight(name, m_list);
					break;
                case EBSDFMap:
                    m_list.setString("id", node.attribute("id").value());
                    RainbowBSDFMap(name, m_list);
                    break;
                case ERef:
                    m_list.setString("id", node.attribute("id").value());
                    RainbowRef(name, m_list);
                    break;
			}			
		}
		else {
			switch (tag) {
			    case EBoolean: {
			    	list.setBoolean(node.attribute("name").value(), toBoolean(node.attribute("value").value()));
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
			    case EString: {
			    	list.setString(node.attribute("name").value(), node.attribute("value").value());
			    	break;
			    }
			    case EVector: {
			    	list.setVector(node.attribute("name").value(), toVector(node.attribute("value").value()));
			    	break;
			    }
			    case EColor: {
			    	if (strcmp(node.name(), "spectrum") == 0) {
			    		// TODO: Fix Spectrum declared with wavelength
			    	}
			    	else if (strcmp(node.name(), "rgb") == 0 || strcmp(node.name(), "color") == 0) {
			    		list.setColor(node.attribute("name").value(), toColor(node.attribute("value").value()));
			    	}
			    	break;
			    }
			    case ETransform: {
			    	list.setTransform(node.attribute("name").value(), m_transform);
			    	RainbowTransform(m_transform);
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
			    	Vector3f delta = toVector(node.attribute("value").value());
			    	/*for (pugi::xml_attribute &attribute : node.attributes()) {
			    		//cout << attribute.name() << ' ' << attribute.value() << endl;
			    		if (strcmp(attribute.name(), "x") == 0) delta.x = toFloat(attribute.value());
			    		if (strcmp(attribute.name(), "y") == 0) delta.y = toFloat(attribute.value());
			    		if (strcmp(attribute.name(), "z") == 0) delta.z = toFloat(attribute.value());
			    	}*/
			    	m_transform *= Translate(delta);
			    	break;
			    }
			    case EScale: {
                    Vector3f scale = toVector(node.attribute("value").value());
                    /*for (pugi::xml_attribute &attribute : node.attributes()) {                        
                        if (strcmp(attribute.name(), "x") == 0) scale.x = toFloat(attribute.value());
                        if (strcmp(attribute.name(), "y") == 0) scale.y = toFloat(attribute.value());
                        if (strcmp(attribute.name(), "z") == 0) scale.z = toFloat(attribute.value());
                    }*/
			    	Transform s = Scale(scale);
			    	m_transform *= Scale(scale);
			    	break;
			    }
			    case ERotate: {
			    	Vector3f axis;
			    	Float angle;
			    	for (pugi::xml_attribute &attribute : node.attributes()) {
			    		if (strcmp(attribute.name(), "x") == 0) axis.x = toFloat(attribute.value());
			    		if (strcmp(attribute.name(), "y") == 0) axis.y = toFloat(attribute.value());
			    		if (strcmp(attribute.name(), "z") == 0) axis.z = toFloat(attribute.value());
			    		if (strcmp(attribute.name(), "angle") == 0) angle = toFloat(attribute.value());
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
	};

	PropertyList prop;
	ParserTag(*doc.begin(), prop, 0);

	RainbowWorld();
}

RAINBOW_NAMESPACE_END
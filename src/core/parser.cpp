#include "parser.h"
#include "ext/pugixml/pugixml.hpp"

RAINBOW_NAMESPACE_BEGIN

/*
 * shape
 * |-bsdf
 * |-medium
 */

ParseRecord::ParseRecord(const std::string filename) :m_filename(filename) {
    m_tags["hide"] = EHide;
    m_tags["mode"] = EMode;
    m_tags["scene"] = EScene;

    m_tags["integrator"] = EIntegrator;
    m_tags["sensor"] = ECamera;
    m_tags["camera"] = ECamera;
    m_tags["sampler"] = ESampler;
    m_tags["film"] = EFilm;
    m_tags["rfilter"] = ERFilter;
    m_tags["emitter"] = ELight;
    m_tags["shape"] = EShape;
    m_tags["bsdf"] = EBSDF;
    m_tags["medium"] = EMedium;
    m_tags["volume"] = EVolume;
    m_tags["ref"] = ERef;

    m_tags["bool"] = EBoolean;
    m_tags["boolean"] = EBoolean;
    m_tags["integer"] = EInteger;
    m_tags["float"] = EFloat;
    m_tags["string"] = EString;
    m_tags["vector"] = EVector;
    m_tags["spectrum"] = EColor;
    m_tags["rgb"] = EColor;
    m_tags["color"] = EColor;
    m_tags["transform"] = ETransform;
    m_tags["lookat"] = ELookAt;
    m_tags["lookAt"] = ELookAt;
    m_tags["translate"] = ETranslate;
    m_tags["scale"] = EScale;
    m_tags["rotate"] = ERotate;
    m_tags["matrix"] = EMatrix;    
}

bool HasAttribute(const pugi::xml_node& node, const std::string& name) {
    for (auto &attribute : node.attributes()) {
        if (strcmp(attribute.name(), name.c_str()) == 0) {
            return true;
        }
    }
    return false;
}

std::string GetValue(const pugi::xml_node& node, const std::string& name, const std::string &defaultValue) {
    for (auto &attribute : node.attributes()) {
        if (strcmp(attribute.name(), name.c_str()) == 0) {
            return attribute.value();
        }
    }
    return defaultValue;
}

/**
 * \brief Helper function: map a position offset in bytes to a more readable line/column value 
 * \param pos 
 * \param record 
 * \return 
 */
std::string GetOffset(ptrdiff_t pos, ParseRecord &record) {
    const std::string &m_filename = record.m_filename;
    std::fstream is(m_filename);
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
}



void HandleTag(    
    pugi::xml_node& node, 
    PropertyList& myList, 
    PropertyList& parentList, 
    ParseRecord &record)
{
    auto &m_tags = record.m_tags;
    auto &m_transform = record.m_transform;

    // If isObject, add the value in children to myList, then create and add the object to fatherList
    // Otherwise, add the value to fatherList

    std::string nodeName = node.name();
    const ETag tag = m_tags[nodeName];
    const bool isObject = tag < EBoolean && tag >= EIntegrator;

    // Get name, type and id
    const std::string name = GetValue(node, "name", "");
    const std::string type = GetValue(node, "type", "");
    const std::string id   = GetValue(node, "id"  , "");

    if (tag == EMode) {
        RainbowRenderMode(type);
    }
    else if (isObject) {
        switch (tag) {
        case EIntegrator:
            RainbowIntegrator(type, myList);
            break;
        case ECamera:
            RainbowCamera(type, myList);
            break;
        case ESampler:
            RainbowSampler(type, myList);
            break;
        case EFilm:
            RainbowFilm(type, myList);
            break;
        case EShape:
            RainbowShape(type, myList);
            if (myList.findString("filename"))
                cout << myList.getString("filename") << endl;
            break;
        case ELight:
            RainbowLight(type, myList);
            break;
        case ERFilter:
            RainbowFilter(type, myList);
            break;
        case EBSDF:
            if (id == "") {
                RainbowMaterial(type, myList);
            } else {
                RainbowNamedMaterial(id, type, myList);
            }
            break;
        case EMedium: {           
            if (id == "") {
                RainbowMedium(type, name, myList);
            } else {
                RainbowNamedMedium(id, type, name, myList);
            }
            break;
        }
        case EVolume:
            RainbowVolume(type, name, myList);
            break;
        case ERef:            
            RainbowRef(name, id);
            break;
        }
    }
    else {
        std::string value = GetValue(node, "value", "");
        switch (tag) {
        case EBoolean: {
            parentList.setBoolean(name, toBoolean(value));
            break;
        }
        case EInteger: {
            parentList.setInteger(name, toInteger(value));
            break;
        }
        case EFloat: {
            parentList.setFloat(name, toFloat(value));
            break;
        }
        case EString: {
            parentList.setString(name, value);
            break;
        }
        case EVector: {
            parentList.setVector(name, toVector(value));
            break;
        }
        case EColor: {
            if (strcmp(node.name(), "spectrum") == 0) {
                // TODO: Fix Spectrum declared with wavelength
                Assert(false, "No implemented!");
            }
            else if (strcmp(node.name(), "rgb") == 0 || strcmp(node.name(), "color") == 0) {
                parentList.setColor(name, toColor(value));
            }
            break;
        }
        case ETransform: {
            parentList.setTransform(name, m_transform);
            break;
        }
        case ELookAt: {
            const Vector3f target = toVector(node.attribute("target").value());
            const Vector3f origin = toVector(node.attribute("origin").value());
            const Vector3f up = toVector(node.attribute("up").value());
            m_transform *= LookAt(target, origin, up);
            break;
        }
        case ETranslate: {
            if (value == "") {
                const Float x = toFloat(GetValue(node, "x", "0"));
                const Float y = toFloat(GetValue(node, "y", "0"));
                const Float z = toFloat(GetValue(node, "z", "0"));
                m_transform *= Translate(Vector3f(x, y, z));
            }
            else {
                const Vector3f delta = toVector(value);
                m_transform *= Translate(delta);
            }
            break;
        }
        case EScale: {
            if (value == "") {
                const Float x = toFloat(GetValue(node, "x", "1"));
                const Float y = toFloat(GetValue(node, "y", "1"));
                const Float z = toFloat(GetValue(node, "z", "1"));
                m_transform *= Scale(Vector3f(x, y, z));
            }
            else {
                const Vector3f scale = toVector(value);
                m_transform *= Scale(scale);
            }
            break;
        }
        case ERotate: {
            const Float x = toFloat(GetValue(node, "x", "0"));
            const Float y = toFloat(GetValue(node, "y", "0"));
            const Float z = toFloat(GetValue(node, "z", "0"));
            const Float angle = toFloat(GetValue(node, "angle", "0"));            
            m_transform *= Rotate(angle, Vector3f(x, y, z));
            break;
        }
        case EMatrix: {
            const Matrix4x4 mat = toMatrix(value);
            m_transform *= Transform(mat);
            break;
        }
        }
    }
}

void ParseTag(pugi::xml_node& node, PropertyList& parentList, ParseRecord &record) {
    auto &m_tags = record.m_tags;
    auto &m_transform = record.m_transform;

    // Skip the comment and other information like version and encoding.
    if (node.type() == pugi::node_comment ||
        node.type() == pugi::node_declaration) return;
    // Check the name of tag
    Assert(m_tags.count(node.name()) == 1, std::string("Unexpected type ") + node.name());
    const ETag tag = m_tags[node.name()];

    // If isObject, add the value in children to myList, then create and add the object to fatherList
    // But EShape isn't added into fatherList, it is added into RenderOptions
    // Otherwise, add the value to fatherList

    // Initialize transform
    if (tag == ETransform) {
        m_transform.Identify();
    }
    else if (tag == EHide) {
        return;
    }

    // Add children nodes' value/object to myList
    PropertyList myList;
    for (pugi::xml_node &child : node.children()) {
        ParseTag(child, myList, record);
    }

    // Handle tags
    HandleTag(node, myList, parentList, record);
}


void Parse(ParseRecord &record) {
    const auto &m_filename = record.m_filename;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(m_filename.c_str());

    Assert(result, "Error while parsing \"" + m_filename + "\": " +
        result.description() + " (at " + GetOffset(result.offset, record) + ")");

    RainbowSceneBegin();
    PropertyList list;
    ParseTag(*doc.begin(), list, record);
    RainbowSceneEnd();
}


/*void ParserXMLFile(const std::string & filename) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename.c_str());


    // Helper function: map a position offset in bytes to a more readable line/column value 
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

        EBSDF,
        EBSDFMap,
        ECamera,
        EFilm,
        EHide,
        EIntegrator,
        ELight,
        EMode,
        EMedium,
        ERef,
        ERFilter,
        ESampler,
        EScene,
        EShape,
        EVolume,

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
    tags["mode"] = EMode;

    tags["scene"] = EScene;
    tags["integrator"] = EIntegrator;
    tags["sensor"] = ECamera;
    tags["camera"] = ECamera;
    tags["sampler"] = ESampler;
    tags["film"] = EFilm;
    tags["bsdf"] = EBSDF;
    tags["medium"] = EMedium;
    tags["shape"] = EShape;
    tags["emitter"] = ELight;
    tags["rfilter"] = ERFilter;
    tags["volume"] = EVolume;
    tags["ref"] = ERef;
    tags["hide"] = EHide;

    tags["bool"] = EBoolean;
    tags["integer"] = EInteger;
    tags["float"] = EFloat;
    tags["string"] = EString;
    tags["vector"] = EVector;
    tags["spectrum"] = EColor;
    tags["rgb"] = EColor;
    tags["color"] = EColor;

    tags["transform"] = ETransform;
    tags["lookat"] = ELookAt;
    tags["lookAt"] = ELookAt;
    tags["translate"] = ETranslate;
    tags["scale"] = EScale;
    tags["rotate"] = ERotate;
    tags["matrix"] = EMatrix;

    Transform m_transform;

    std::function<void(pugi::xml_node, PropertyList&, int)> ParserTag =
        [&](pugi::xml_node node, PropertyList &list, int num) -> void {
        if (node.type() == pugi::node_comment || node.type() == pugi::node_declaration)
            return;

        if (tags.find(node.name()) == tags.end())
            return;

        ETag tag = tags[node.name()];
        if (tag == EHide) {
            return;
        }
        else if (tag == EScene) {
            node.append_attribute("type");
            node.attribute("type").set_value("scene");
        }
        else if (tag == ETransform) {
            m_transform.Identify();
        }
        else if (tag == EBSDF) {
            if (node.attribute("id") != NULL) {
                tag = EBSDFMap;
            }
        }
        else if (tag == ERef) {
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
            std::string type = static_cast<std::string>(node.attribute("type").value());
            switch (tag) {
            case EMode:
                RainbowRenderMode(type);
                break;
            case EScene:
                break;
            case EIntegrator:
                RainbowIntegrator(type, m_list);
                break;
            case ECamera:
                RainbowCamera(type, m_list);
                break;
            case ESampler:
                RainbowSampler(type, m_list);
                break;
            case EFilm:
                RainbowFilm(type, m_list);
                break;
            case EMedium: {
                std::string name = static_cast<std::string>(node.attribute("name").value());
                RainbowMedium(type, name, m_list);
                break;
            }
            case EShape:
                RainbowShape(type, m_list);
                if (m_list.findString("filename"))
                    cout << m_list.getString("filename") << endl;
                break;
            case ELight:
                RainbowLight(type, m_list);
                break;
            case ERFilter:
                RainbowFilter(type, m_list);
                break;
            case EBSDF:
                RainbowMaterial(type, m_list);
                break;
            case EBSDFMap:
                m_list.setString("id", node.attribute("id").value());
                RainbowMaterial(type, m_list);
                break;
            case ERef:
                m_list.setString("id", node.attribute("id").value());
                RainbowRef(type, m_list);
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
                    Assert(false, "No implemented!");
                }
                else if (strcmp(node.name(), "rgb") == 0 || strcmp(node.name(), "color") == 0) {
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
                Vector3f delta = toVector(node.attribute("value").value());
                m_transform *= Translate(delta);
                break;
            }
            case EScale: {
                Vector3f scale = toVector(node.attribute("value").value());
                Transform s = Scale(scale);
                m_transform *= Scale(scale);
                break;
            }
            case ERotate: {
                Vector3f axis;
                Float angle = 0;
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

    RainbowSceneBegin();

    PropertyList prop;
    ParserTag(*doc.begin(), prop, 0);

    RainbowSceneEnd();
}*/
    


RAINBOW_NAMESPACE_END

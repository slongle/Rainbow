#include "core/common.h"
#include "core/vector.h"
#include "core/bbox.h"
#include "core/ray.h"
#include "core/transform.h"
#include "core/imageio.h"
#include "core/parser.h"
#include "core/object.h"
using namespace rainbow;

int main(int argc, char *argv[]) {
	/*int height = 100;
	int width = 100;
	Image a(width, height);
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			*a(i,j) = RGBSpectrum(static_cast<Float>(i) / width, static_cast<Float>(j) / height, 1);
	a.save("first.png");*/
	//std::map<std::string, ObjectFactory::Constructor> *a = ObjectFactory::constructors;
	
	std::vector<std::string> x;
	for (int i = 0; i < 16; i++) {
		std::string str; cin >> str;
		x.push_back(str);
	}
	sort(x.begin(), x.end());
	for (std::string &str : x)
		cout << str << endl;
	return 0;

	std::string tmp("abc\0awdf");
	cout << tmp << endl;
	cout << tmp.c_str() << endl;

	//std::string filename = "C:\\Users\\del\\Desktop\\cbox\\cbox.xml";
	std::string filename = "C:/Users/del/Desktop/cbox/cbox.xml";
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.c_str());

	pugi::xml_node rootNode=doc.child("scene");
	std::function<void (pugi::xml_node,int)> parserTag = [&](pugi::xml_node node,int num) {
		for (pugi::xml_node childNode = node.first_child(); childNode; childNode = childNode.next_sibling()) {
			for (int i = 0; i < num; i++) cout << "    ";
			cout << childNode.name() << "  ";
			for (pugi::xml_attribute attribute = childNode.first_attribute(); attribute; attribute = attribute.next_attribute()) {
				cout << attribute.name() << "  " << attribute.value() << "    ";
			}
			cout << endl;
			parserTag(childNode,num+1);
		}
	};
	parserTag(rootNode, 0);
	return 0;
}
#include "core/common.h"
#include "core/vector.h"
#include "core/bbox.h"
#include "core/ray.h"
#include "core/transform.h"
#include "core/film.h"
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
	std::unique_ptr<uint8_t[]> rgb8(new uint8_t[4 *100]);


	Matrix4x4 aa(1,2,3,5,
		7,11,13,17,
		19,23,29,31,
		37,41,43,49);
	cout << __FILE__ << endl;
	cout << __LINE__ << endl;
	Transform a(aa);
	cout << a << endl;
	Ray b;
	cout << b << endl;
	cout << __LINE__ << endl;
	Point2i c(2);
	cout << c << endl;

	std::string filename = "C:/Users/del/Desktop/cbox/cbox.xml";
	ParserXMLFile(filename);

	return 0;
}
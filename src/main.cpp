#include "core/common.h"
//#include "core/vector.h"
//#include "core/bbox.h"
//#include "core/ray.h"
//#include "core/transform.h"
//#include "core/film.h"
#include "core/parser.h"
//#include "core/object.h"
//#include "shapes/sphere.h"

using namespace rainbow;

void Render(const Scene* scene) {
	Assert(scene->integrator != nullptr, "Hasn't Parsed Integrator!");
	Integrator* integrator = scene->integrator;
	integrator->Render(*scene);
}

int main(int argc, char *argv[]) {
	/*int height = 100;
	int width = 100;
	Image a(width, height);
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			*a(i,j) = RGBSpectrum(static_cast<Float>(i) / width, static_cast<Float>(j) / height, 1);
	a.save("first.png");*/
	//std::map<std::string, ObjectFactory::Constructor> *a = ObjectFactory::constructors;
	/*std::unique_ptr<uint8_t[]> rgb8(new uint8_t[4 *100]);


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
	cout << c << endl;*/

	/*std::vector<int> d;
	d.resize(100);
	for (int i = 0; i < 10; i++)
		d[i] = i;
	int* a = &d[3];
	for (int i = 0; i < 5; i++)
		std::cout << a[i] << std::endl;*/

	//std::string filename = "C:/Users/del/Desktop/cornell-box/scene.xml";
	//std::string filename = "C:/Users/del/Desktop/cbox/cbox.xml";
	std::string filename = "C:/Users/del/Desktop/cbox-Rainbow/cbox-distributed.xml";
	Scene* scene = ParserXMLFile(filename);
	Render(scene);
	return 0;
}
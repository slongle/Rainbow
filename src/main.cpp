#include "core/common.h"
//#include "core/vector.h"
//#include "core/bbox.h"
//#include "core/ray.h"
//#include "core/transform.h"
//#include "core/film.h"
#include "core/parser.h"
//#include "core/object.h"
//#include "shapes/sphere.h"
#include "core/spectrum.h"

using namespace rainbow;

int main(int argc, char *argv[]) {

	//Transform a = Scale(-1,1,1);
	//Transform b = LookAt(Vector3f(0, 0.893051, 4.41198), Vector3f(0, 0.919769, 5.41159), Vector3f(0, 1, 0));
	//cout << (a*b) << endl;

	RainbowInit();
	std::string filename = "C:/Users/del/Desktop/cbox-Rainbow/cbox-distributed_test.xml";
	filesystem::path path(filename);
	getFileResolver()->prepend(path.parent_path());
	ParserXMLFile(filename);

	return 0;
}
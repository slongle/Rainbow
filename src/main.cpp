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

#include"shapes/sphere.h"

using namespace rainbow;

int main(int argc, char *argv[]) {
	RainbowInit();
	std::string filename = "C:/Users/del/Desktop/cbox-Rainbow/cbox-distributed_test.xml";
	filesystem::path path(filename);
	getFileResolver()->prepend(path.parent_path());
	ParserXMLFile(filename);

	return 0;
}
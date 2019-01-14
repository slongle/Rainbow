#include "core/common.h"
#include "core/parser.h"
#include "core/imageio.h"
#include <ext/filesystem/filesystem/path.h>

using namespace rainbow;

int main(int argc, char *argv[]) {
    //std::string name("a.png");
    //name.insert(name.find_last_of('.'), "_" + std::to_string(1));
    //std::cout << name << std::endl;
    

	RainbowInit();	
    // Position at School
    const std::string filename = "F:/Document/Graphics/code/Rainbow/scenes/cbox/cbox-whitted.xml";
    // Position at Home
    //const std::string filename = "C:/Users/Administrator/Desktop/Rainbow/scenes/cbox/cbox-whitted.xml";
	filesystem::path path(filename);
	getFileResolver()->prepend(path.parent_path());
	ParserXMLFile(filename);

	return 0;
}
#include "core/common.h"
#include "core/parser.h"

//#include "core/gui.h"

using namespace rainbow;

int main(int argc, char *argv[]) {


    Timer timer;
	RainbowInit();	
    // Position at School
    const std::string filename = "F:/Document/Graphics/code/Rainbow/scenes/cbox/cbox-whitted.xml";
    // Position at Home
    // const std::string filename = "C:/Users/Administrator/Desktop/Rainbow/Rainbow/scenes/cbox/cbox-whitted.xml";
	filesystem::path path(filename);
	getFileResolver()->prepend(path.parent_path());
	ParserXMLFile(filename);

    timer.ShowTime();
	return 0;
}
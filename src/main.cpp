#include "core/common.h"
#include "core/parser.h"
#include "core/imageio.h"
#include <ext/filesystem/filesystem/path.h>

using namespace rainbow;

int main(int argc, char *argv[]) {
    
    //std::string solutionDir("F:/Document/Graphics/code/");
    std::string solutionDir("C:/Users/Administrator/Desktop/");
    std::vector<std::string> scenes(100);
    scenes[0] = "Rainbow/scenes/cbox/cbox-whitted.xml";
    scenes[1] = "Rainbow/scenes/motto/motto-diffuse.xml";
    scenes[2] = "Rainbow/scenes/veach_mi/veach_mis.xml";

    std::string filename(solutionDir + scenes[0]);

	RainbowInit();	
	filesystem::path path(filename);
	getFileResolver()->prepend(path.parent_path());
	ParserXMLFile(filename);

	return 0;
}
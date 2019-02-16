#include "core/common.h"
#include "core/parser.h"
#include "core/imageio.h"
#include "ext/filesystem/filesystem/path.h"

#include "core/film.h"

using namespace rainbow;

int main(int argc, char *argv[]) {

    std::string solutionDir("F:/Document/Graphics/code/Rainbow/scenes/");
    //std::string solutionDir("C:/Users/Administrator/Desktop/Rainbow/scenes/");
    std::vector<std::string> scenes(100);
    scenes[0] = "cbox/cbox-whitted.xml";
    scenes[1] = "motto/motto-diffuse.xml";
    scenes[2] = "cornell-box/scene.xml";
    scenes[3] = "veach-mis/scene.xml";
    scenes[4] = "veach-bidir/scene.xml";
    scenes[5] = "volumetric-caustic/scene.xml";



    const std::string filename(solutionDir + scenes[2]);

	RainbowInit();	
	filesystem::path path(filename);
	getFileResolver()->prepend(path.parent_path());
	ParserXMLFile(filename);

	return 0;
}
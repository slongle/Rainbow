#include "core/common.h"
#include "core/parser.h"
#include "ext/filesystem/filesystem/path.h"

using namespace rainbow;

int main(int argc, char *argv[]) {
    std::string solutionDir("E:/Document/Graphics/code/Rainbow/scenes/");
    //std::string solutionDir("C:/Users/Administrator/Desktop/a/Rainbow/scenes/");
    std::vector<std::string> scenes(100);
    scenes[0] = "cbox/cbox(mesh).xml";
    scenes[1] = "cornell-box/scene.xml";
    scenes[2] = "veach-mi/scene.xml";
    scenes[3] = "veach-mis/scene.xml";
    scenes[4] = "veach-bidir/scene.xml";
    scenes[5] = "volumetric-caustic/scene.xml";
    scenes[6] = "water-caustic/scene.xml";
    scenes[7] = "light-test/scene.xml";
    scenes[8] = "rainbow/scene.xml";
    scenes[9] = "dragon/scene.xml";
    scenes[10] = "glass-of-water/scene.xml";
    scenes[11] = "hetvol/hetvol.xml";
    scenes[12] = "fire/fire.xml";
    scenes[13] = "scene-test/scene-test.xml";

    const std::string filename(solutionDir + scenes[13]);

	filesystem::path path(filename);
	getFileResolver()->prepend(path.parent_path());
    ParseRecord record(filename);
    Parse(record);

	return 0;
}
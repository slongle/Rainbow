#include "core/common.h"
#include "core/parser.h"
#include "ext/filesystem/filesystem/path.h"

#include <embree3/rtcore.h>
#include <embree3/rtcore_scene.h>

#include "core/film.h"
#include "core/memory.h"
#include "materials/glass.h"
#include "shapes/sphere.h"


#include "filters/box.h"
#include "samplers/independent.h"

#include "core/embreescene.h"

using namespace rainbow;

int main(int argc, char *argv[]) {
    //EmbreeSceneMain();
    //return 0;

    std::string solutionDir("F:/Document/Graphics/code/Rainbow/scenes/");
    //std::string solutionDir("C:/Users/Administrator/Desktop/a/Rainbow/scenes/");
    std::vector<std::string> scenes(100);
    scenes[0] = "cbox/cbox.xml";
    scenes[1] = "cornell-box/scene2.xml";
    scenes[2] = "veach-mis/scene.xml";
    scenes[3] = "veach-bidir/scene.xml";
    scenes[4] = "volumetric-caustic/scene.xml";
    scenes[5] = "water-caustic/scene.xml";
    scenes[6] = "light-test/scene.xml";
    scenes[7] = "rainbow/scene.xml";

    const std::string filename(solutionDir + scenes[0]);

	filesystem::path path(filename);
	getFileResolver()->prepend(path.parent_path());
	ParserXMLFile(filename);

	return 0;
}
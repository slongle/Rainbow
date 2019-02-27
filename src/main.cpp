#include "core/common.h"
#include "core/parser.h"
#include "ext/filesystem/filesystem/path.h"

#include "core/film.h"
#include "core/memory.h"
#include "materials/glass.h"
#include "shapes/sphere.h"

using namespace rainbow;

void foo() {
    MemoryArena arena;
    GlassMaterial g(RGBSpectrum(1), RGBSpectrum(1), 0.1, 0.1);
    Transform t = Scale(Vector3f(1, 1, 1)), invT = Inverse(t);
    Sphere s(&t, &invT, 1);
    SurfaceInteraction inter = SurfaceInteraction(Point3f(), Vector3f(), Normal3f(1, 0, 0), Vector3f(), &s);
    g.ComputeScatteringFunctions(arena, &inter);
    arena.Reset();
}

int main(int argc, char *argv[]) {

    std::string solutionDir("F:/Document/Graphics/code/Rainbow/scenes/");
    //std::string solutionDir("C:/Users/Administrator/Desktop/Rainbow/scenes/");
    std::vector<std::string> scenes(100);
    scenes[0] = "cbox/cbox-whitted.xml";
    scenes[1] = "cornell-box/scene.xml";
    scenes[2] = "veach-mis/scene.xml";
    scenes[3] = "veach-bidir/scene.xml";
    scenes[4] = "volumetric-caustic/scene.xml";
    scenes[5] = "water-caustic/scene.xml";
    scenes[6] = "light-test/scene.xml";
    scenes[7] = "rainbow/scene.xml";

    const std::string filename(solutionDir + scenes[7]);

	RainbowInit();	
	filesystem::path path(filename);
	getFileResolver()->prepend(path.parent_path());
	ParserXMLFile(filename);

	return 0;
}
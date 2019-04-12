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

//#include "C:\Users\del\Desktop\halton\halton_enum.h"
//#include "C:\Users\del\Desktop\halton\halton_sampler.h"

using namespace rainbow;

/*void SamplerTest(std::string filename) {    
    Point2i resolution(512, 512);
    std::shared_ptr<BoxFilter> filter(new BoxFilter(Vector2f(1, 1)));
    Film* film = new Film(filename, resolution, filter);

    Halton_sampler halton_sampler;
    halton_sampler.init_faure();

    for (int i = 0; i < 1024; i++) {        
        Point2f sample(halton_sampler.sample(0, i), halton_sampler.sample(1, i));
        Point2i position(sample.x*resolution.x, sample.y*resolution.y);
        film->SetPixel(position, RGBSpectrum(1));
    }
    film->SaveImage();
    return;

    Point2i trueResolution(30, 30);
    int scale = 10;
    Point2i imageResolution(trueResolution.x*(scale - 1) + 1, trueResolution.y*(scale - 1) + 1);
    //std::shared_ptr<BoxFilter> filter(new BoxFilter(Vector2f(1, 1)));
    //Film* film = new Film(filename, imageResolution, filter);

    for (unsigned y = 0; y < trueResolution.y; ++y) // Iterate over rows.
    {
        for (unsigned x = 0; x < trueResolution.x; ++x) // Iterate over columns.
        {
            int ix = x * (scale - 1);
            int iy = y * (scale - 1);
            for (int i = 0; i < scale; i++)
            {
                film->SetPixel(Point2i(ix + i, iy), RGBSpectrum(1));
                film->SetPixel(Point2i(ix + i, iy + scale - 1), RGBSpectrum(1));
                film->SetPixel(Point2i(ix, iy + i), RGBSpectrum(1));
                film->SetPixel(Point2i(ix + scale - 1, iy + i), RGBSpectrum(1));
            }
        }
    }
    
    Independent sampler;
    // Enumerate samples per pixel for the given resolution.
    const Halton_enum halton_enum(trueResolution.x, trueResolution.y);
    const unsigned samples_per_pixel = 4;
    assert(samples_per_pixel < halton_enum.get_max_samples_per_pixel());
    for (unsigned y = 0; y < trueResolution.y; ++y) // Iterate over rows.
    {
        for (unsigned x = 0; x < trueResolution.x; ++x) // Iterate over columns.
        {
            for (unsigned i = 0; i < samples_per_pixel; ++i) // Iterate over samples in the pixel.
            {
                // Retrieve the index of the corresponding sample.
                //const unsigned index = halton_enum.get_index(i, x, y);
                const unsigned index = i;
                //std::cout << "Index: " << index << endl;

                // Draw three components.
                const float sx = halton_sampler.sample(0, index);
                const float sy = halton_sampler.sample(1, index);
                const float sz = halton_sampler.sample(2, index);
                // Rescale the first two components to match the pixel raster.
                const float rx = halton_enum.scale_x(sx);
                const float ry = halton_enum.scale_y(sy);
                // Validate that we're inside the pixel, taking floating-point inaccuracies
                // into account.
                assert(x <= rx + 1e-3f && rx - 1e-3f < x + 1);
                assert(y <= ry + 1e-3f && ry - 1e-3f < y + 1);
                
                // Finally, write the samples to the output stream, in gnuplot format.   
                const float fx = rx - x;
                const float fy = ry - y;
                //const float fx = sampler.Next1D();
                //const float fy = sampler.Next1D();

                Point2i position(x*(scale-1) + fx*scale, y*(scale-1) + fy*scale);
                //cout << position << endl;
                film->SetPixel(position, RGBSpectrum(0,1,0));
            }
        }
    }    

    //Independent sampler;
    //for (int i = 0; i < 1024; i++) {
    //    Point2f sample = sampler.Next2D();
    //    Point2i position(sample.x * resolution.x, sample.y * resolution.y);
    //    cout << sample << ' ' << position << endl;
    //    film->SetPixel(position, RGBSpectrum(1));
    //}
    film->SaveImage();
}*/


int main(int argc, char *argv[]) {

    RTCDevice embree_device;
    RTCScene embree_scene;
    embree_device = rtcNewDevice(NULL);
    embree_scene = rtcNewScene(embree_device);

    RTCGeometry geometry = rtcNewGeometry(embree_device, RTC_GEOMETRY_TYPE_TRIANGLE);

    rtcCommitGeometry(geometry);
    unsigned geomID = rtcAttachGeometry(embree_scene, geometry);
    rtcReleaseGeometry(geometry);
    

    rtcCommitScene(embree_scene);


    std::string solutionDir("F:/Document/Graphics/code/Rainbow/scenes/");
    //std::string solutionDir("C:/Users/Administrator/Desktop/Rainbow/scenes/");
    std::vector<std::string> scenes(100);
    scenes[0] = "cbox/cbox.xml";
    scenes[1] = "cornell-box/scene.xml";
    scenes[2] = "veach-mis/scene.xml";
    scenes[3] = "veach-bidir/scene.xml";
    scenes[4] = "volumetric-caustic/scene.xml";
    scenes[5] = "water-caustic/scene.xml";
    scenes[6] = "light-test/scene.xml";
    scenes[7] = "rainbow/scene.xml";

    const std::string filename(solutionDir + scenes[6]);

	filesystem::path path(filename);
	getFileResolver()->prepend(path.parent_path());
	ParserXMLFile(filename);

	return 0;
}
#include"Integrator.h"

RAINBOW_NAMESPACE_BEGIN

void SamplerIntegrator::Render(const Scene &scene) {
    std::shared_ptr<Film> film = camera->film;
    Ray ray;

    std::cout << scene.aggregate->primitives.size() << std::endl;

    int SampleNum = 5;
    for (int y = 0; y < film->resolution.y; y++) {
        fprintf(stderr, "\rRendering (%d spp) %5.2f%%", SampleNum, 100.*(y + 1) / film->resolution.y);
        for (int x = 0; x < film->resolution.x; x++) {

            /*
            camera->GenerateRay(&ray,
                Point2f(x - film->resolution.x *0.5, y - film->resolution.y *0.5) + sampler->Get2D());
            if (scene.IntersectP(ray)) {
                film->SetPixel(Point2i(x, y), RGBSpectrum(1, 1, 1));
            }
            else
                film->SetPixel(Point2i(x, y), RGBSpectrum(0, 0, 0));

            continue;
            */

            RGBSpectrum L(0.0);            
            for (int i = 0; i < SampleNum; i++) {
                camera->GenerateRay(&ray,
                    Point2f(x - film->resolution.x *0.5, y - film->resolution.y *0.5) + sampler->Get2D());
                L += Li(ray, scene, 0);
            }
            L /= SampleNum;
            film->SetPixel(Point2i(x, y), L);
        }
    }
    film->SaveImage();
}

RAINBOW_NAMESPACE_END
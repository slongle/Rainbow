#include "pssmlt.h"

#include <atomic>
#include <thread>
#include <mutex>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

RAINBOW_NAMESPACE_BEGIN

std::unique_ptr<Sampler>
PSSMLTSampler::Clone(const Point2i& seed)
{
    return std::unique_ptr<Sampler>();
}

Float PSSMLTSampler::Next1D()
{
    EnsureReady(sampleIndex);
    return X[sampleIndex].val;
}

Point2f PSSMLTSampler::Next2D()
{
    return Point2f(Next1D(), Next1D());
}

void PSSMLTSampler::StartInteraion()
{
    currentIteration++;
    largeStep = rng.nextFloat() < largeStepProbability;    
}

void PSSMLTSampler::Accept()
{
    if (largeStep) {
        lastLargeStepIteration = currentIteration;
    }
}

void PSSMLTSampler::Reject()
{
    for (auto& xi : X) {
        if (xi.lastModifiedIteraion == currentIteration) {
            xi.Restore();
        }
    }
    currentIteration--;
}

void PSSMLTSampler::EnsureReady(int index)
{
    // Get current sample
    if (index >= X.size()) {
        X.resize(index + 1);
    }
    PrimarySample& xi = X[index];

    // Check large step
    if (xi.lastModifiedIteraion < lastLargeStepIteration) {
        xi.val = rng.nextFloat();
        xi.lastModifiedIteraion = lastLargeStepIteration;
    }

    // Check small step
    xi.Backup();
    if (largeStep) {
        xi.val = rng.nextFloat();
    }
    else {
        uint64_t nSmall = currentIteration - xi.lastModifiedIteraion;

        // Mutate
        Float normalSample = SQRT_TWO * ErfInv(2 * rng.nextFloat() - 1);

        // Compute the effective standard deviation and apply perturbation to
        // $\VEC{X}_i$
        Float effSigma = sigma * std::sqrt((Float)nSmall);
        xi.val += normalSample * effSigma;
        xi.val -= std::floor(xi.val);
    }
    xi.lastModifiedIteraion = currentIteration;
}

void PSSMLTSampler::Start()
{
    sampleIndex = 0;
}

RGBSpectrum Li(
    MemoryArena& arena,
    const Ray& r,
    const Scene& scene,
    Sampler& sampler,
    int depth)
{
    RGBSpectrum L(0.), beta(1.);
    Ray ray(r);
    bool SpecularBounce = false;

    int bounce;
    for (bounce = 0;; bounce++) {
        SurfaceInteraction inter;
        bool foundIntersect = scene.IntersectP(ray, &inter);

        MediumInteraction mi;
        if (ray.medium) {
            RGBSpectrum emission;
            RGBSpectrum weight = ray.medium->Sample(ray, sampler, arena, &mi, emission);
            L += beta * emission;
            beta *= weight;
        }
        if (beta.IsBlack()) break;

        if (mi.IsValid()) {
            if (bounce >= depth) break;

            L += beta * UniformSampleOneLight(mi, scene, sampler, true);

            Vector3f wo = -ray.d, wi;
            mi.phase->SampleP(wo, &wi, sampler.Next2D());
            ray = mi.SpawnToRay(wi);
            SpecularBounce = false;
        }
        else {
            if (bounce == 0 || SpecularBounce) {
                if (foundIntersect) {
                    L += beta * inter.Le(-ray.d);
                }
            }

            if (!foundIntersect || bounce >= depth) break;

            inter.ComputeScatteringFunctions(arena);
            if (!inter.bsdf) {
                ray = inter.SpawnToRay(ray.d);
                bounce--;
                continue;
            }

            L += beta * UniformSampleOneLight(inter, scene, sampler, true);

            Vector3f wo = -ray.d, wi;
            Float BSDFPdf;
            BxDFType BSDFType;
            RGBSpectrum f = inter.bsdf->SampleF(wo, &wi, sampler.Next2D(), &BSDFPdf, BSDF_ALL, &BSDFType);
            if (BSDFPdf == 0. || f.IsBlack()) break;
            beta *= f * AbsDot(wi, inter.n) / BSDFPdf;

            SpecularBounce = (BSDFType & BSDF_SPECULAR) != 0;

            ray = inter.SpawnToRay(wi);
        }

        // Russian roulette
        if (bounce > 3) {
            Float q = std::min((Float)0.95, beta.MaxComponent());
            if (sampler.Next1D() >= q)
                break;
            beta /= q;
        }
    }

    return L;
}

RGBSpectrum PSSMLTIntegrator::L(
    const Scene& scene, 
    MemoryArena& arena, 
    PSSMLTSampler& sampler, 
    int depth, 
    Point2f* pRaster)
{
    sampler.Start();
    Point2f u = sampler.Next2D();
    Point2i resolution = m_camera->film->resolution;
    *pRaster = Point2f(resolution.x * u.x, resolution.y * u.y);
    Ray ray;
    m_camera->GenerateRay(&ray, *pRaster);
    return Li(arena, ray, scene, sampler, depth);
}


void PSSMLTIntegrator::Render(const Scene& scene)
{
    // Phase 1 : Bootstrap
    // 1) Sample initial state    
    int nBootstrapSamples = nBootstrap * (maxDepth + 1);
    std::vector<Float> bootstrapWeights(nBootstrapSamples, 0);
    std::thread bootstrapThread([&] {
        tbb::blocked_range<int> range(0, nBootstrap);
        std::atomic<int> bootstrapFinished(0);
        auto map = [&](const tbb::blocked_range<int>& range) {
            for (int i = range.begin(); i < range.end(); ++i) {
                MemoryArena arena;                
                for (int depth = 0; depth <= maxDepth; depth++) {
                    int index = i * (maxDepth + 1) + depth;
                    PSSMLTSampler sampler(mutationsPerPixel, index, sigma, largeStepProbability);
                    Point2f pRaster;
                    bootstrapWeights[index] = L(scene, arena, sampler, depth, &pRaster).y();
                    arena.Reset();
                }
                bootstrapFinished++;
                fprintf(stderr, "\rBootstrap : %.3f%%", 100.f * bootstrapFinished / nBootstrap);
            }
        };
        tbb::parallel_for(range, map);
    }); 
    bootstrapThread.join();
    printf("\nBootstrap Phase Done\n");

    // 2) Calculate b
    // Set the constant b to the sum of average luminances for each depth.
    // Because we¡¯ve kept the contributions of different path sample depths distinct, 
    // we can preferentially sample path lengths that make the largest contribution to the image. 
    Distribution1D bootstrap(&bootstrapWeights[0], nBootstrapSamples);
    Float b = bootstrap.funcInt * (maxDepth + 1);
    printf("b = %f\n", b);

    // Phase 2 : Markov Chain
    Film& film = *m_camera->film;
    uint64_t nTotalMutations = (int64_t)mutationsPerPixel * (int64_t)film.resolution.x * (uint64_t)film.resolution.y;
    std::thread chainThread([&] {
        tbb::blocked_range<int> range(0, nChains);
        std::atomic<int> chainFinished(0);
        auto map = [&](const tbb::blocked_range<int>& range) {
            for (int i = range.begin(); i < range.end(); ++i) {                
                uint64_t nChainMutations = 
                    std::min((i + 1) * nTotalMutations / nChains, nTotalMutations) - i * nTotalMutations / nChains;
                MemoryArena arena;

                // Sample initial state
                pcg32 rng(i);
                int bootstrapIndex = bootstrap.SampleDiscrete(rng.nextFloat());
                int depth = bootstrapIndex % (maxDepth + 1);
                PSSMLTSampler sampler(mutationsPerPixel, bootstrapIndex, sigma, largeStepProbability);

                // Initialize state
                Point2f pCurrent;
                RGBSpectrum LCurrent = L(scene, arena, sampler, depth, &pCurrent);

                // Run Markov Chain
                for (int j = 0; j < nChainMutations; j++) {
                    // Calculate L
                    sampler.StartInteraion();
                    Point2f pProposed;
                    RGBSpectrum LProposed = L(scene, arena, sampler, depth, &pProposed);

                    // Compute acceptance probability for proposed sample
                    Float accept = std::min(1.f, LProposed.y() / LCurrent.y());

                    // Add contribution            
                    if (accept > 0) {
                        film.AddSplat(Point2i(pProposed),
                            LProposed * (accept + sampler.largeStep) / (LProposed.y() / b + largeStepProbability));
                    }
                    film.AddSplat(Point2i(pCurrent),
                        LCurrent * (1.f - accept) / (LCurrent.y() / b + largeStepProbability));

                    // Accept or reject
                    if (rng.nextFloat() < accept) {
                        pCurrent = pProposed;
                        LCurrent = LProposed;
                        sampler.Accept();
                    }
                    else {
                        sampler.Reject();
                    }
                    arena.Reset();
                }

                chainFinished++;
                fprintf(stderr, "\rChain : %.3f%%", 100.f * chainFinished / nChains);
            }
        };
        tbb::parallel_for(range, map);
    });
    chainThread.join();
    printf("\nMarkov Chain Phase Done\n");

    // Multiple b/n
    film.SaveImage(1.f / mutationsPerPixel);
}

PSSMLTIntegrator* CreatePSSMLTIntegrator(
    PropertyList& list, 
    std::shared_ptr<Camera> camera)
{
    int maxDepth = list.getInteger("maxDepth", 5);
    int nBootstrap = list.getInteger("bootstrapsamples", 1000);
    int64_t nChains = list.getInteger("chains", 10);
    int mutationsPerPixel = list.getInteger("mutationsperpixel", 10);
    Float largeStepProbability = list.getFloat("largestepprobability", 0.3f);
    Float sigma = list.getFloat("sigma", .01f);
    return new PSSMLTIntegrator(camera, nullptr, maxDepth, nBootstrap, nChains, mutationsPerPixel, sigma, largeStepProbability);
}

RAINBOW_NAMESPACE_END
#pragma once
#ifndef __PSSMLT_H
#define __PSSMLT_H

#include "core/integrator.h"
#include "integrators/path.h"

RAINBOW_NAMESPACE_BEGIN

class PSSMLTSampler : public Sampler {
public:
    PSSMLTSampler(
        int mutationsPerPixel,
        uint64_t seed,
        Float sigma,
        Float largeStepProbability)
        : mutationsPerPixel(mutationsPerPixel),
        rng(seed),
        sigma(sigma),
        largeStepProbability(largeStepProbability) {}
    
    std::unique_ptr<Sampler> Clone(const Point2i& seed);
    Float Next1D();    
    Point2f Next2D();

    void EnsureReady(int index);
    void Start();

    void StartInteraion();
    void Accept();
    void Reject();


    struct PrimarySample {
        Float val = 0;
        int lastModifiedIteraion = 0;
        Float valBackup = 0;
        int modifyBackup = 0;

        void Backup() {
            valBackup = val;
            modifyBackup = lastModifiedIteraion;
        }

        void Restore() {
            val = valBackup;
            lastModifiedIteraion = modifyBackup;
        }
    };

    pcg32 rng;
    const int mutationsPerPixel;
    const Float sigma, largeStepProbability;

    uint64_t currentIteration = 0;
    uint64_t lastLargeStepIteration = 0;
    bool largeStep = true;
    std::vector<PrimarySample> X;
    int sampleIndex;
};

class PSSMLTIntegrator : public Integrator {
public:
    PSSMLTIntegrator(
        std::shared_ptr<Camera>    camera,
        std::shared_ptr<Sampler>   sampler,
        int                        maxDepth,
        int                        nBootstrap,
        int                        nChains,
        int                        mutationsPerPixel,
        Float                      sigma,
        Float                      largeStepProbability
    )
        : Integrator(camera, sampler),
        maxDepth(maxDepth),
        nBootstrap(nBootstrap),
        nChains(nChains),
        mutationsPerPixel(mutationsPerPixel),
        sigma(sigma),
        largeStepProbability(largeStepProbability) {}


    void Render(const Scene& scene);

    RGBSpectrum L(
        const Scene& scene, 
        MemoryArena& arena, 
        PSSMLTSampler& sampler, 
        int depth, 
        Point2f* pRaster);


    std::string toString() const {
        return tfm::format(
            "PSSMLT Integrator[\n"
            "    MaxDepth : %d,\n"
            "    nBootstrap : %d\n"
            "    nChains : %d\n"
            "    mutationsPerPixel : %d\n"
            "    sigma : %f\n"
            "    largeStepProbability : %f\n"
            "]",
            maxDepth,
            nBootstrap,
            nChains,
            mutationsPerPixel,
            sigma,
            largeStepProbability);
    }

    int   maxDepth;
    int   nBootstrap;
    int   nChains;
    int   mutationsPerPixel;
    Float sigma;
    Float largeStepProbability;
};

PSSMLTIntegrator* CreatePSSMLTIntegrator(
    PropertyList& list,
    std::shared_ptr<Camera>    camera);

RAINBOW_NAMESPACE_END

#endif // __PSSMLT_H
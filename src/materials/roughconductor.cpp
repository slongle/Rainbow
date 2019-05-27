#include "roughconductor.h"

RAINBOW_NAMESPACE_BEGIN

RoughConductorMaterial::RoughConductorMaterial(
    const RGBSpectrum & m_R,
    const RGBSpectrum & m_eta,
    const RGBSpectrum & m_k,
    const Float & m_alphaU,
    const Float & m_alphaV,
    const std::string & m_distributionType) :
    R(m_R), eta(m_eta), k(m_k),
    alphaU(m_alphaU), alphaV(m_alphaV), distributionType(m_distributionType) {}

RoughConductorMaterial::RoughConductorMaterial(
    const RGBSpectrum & m_R, 
    const RGBSpectrum & m_eta, 
    const RGBSpectrum & m_k, 
    const Float & m_alpha, 
    const std::string & m_distributionType):
    R(m_R), eta(m_eta), k(m_k),
    alphaU(m_alpha), alphaV(m_alpha), distributionType(m_distributionType) {}

void RoughConductorMaterial::ComputeScatteringFunctions(
    MemoryArena&          arena, 
    SurfaceInteraction*   it) 
{
    //it->bsdf = ARENA_ALLOCA(arena, BSDF)(it->n);
    it->bsdf = ARENA_ALLOCA(arena, BSDF)(*it);
    Fresnel* fresnel = ARENA_ALLOCA(arena, FresnelConductor)(RGBSpectrum(1.), eta, k);
    MicrofacetDistribution* distribution;
    if (distributionType == "beckmann") {
        distribution = ARENA_ALLOCA(arena, BeckmannDistribution)(alphaU, alphaV);
    }
    else Assert(false, "No Microfacet Distribution!");
    it->bsdf->Add(ARENA_ALLOCA(arena, MicrofacetReflection)(R, distribution, fresnel));
}

RoughConductorMaterial * CreateRoughConductorMaterial(PropertyList & list) 
{
    RGBSpectrum R = list.getColor("specularReflectance", RGBSpectrum(1.));
    RGBSpectrum eta = list.getColor("eta", RGBSpectrum(0.200438, 0.924033, 1.10221));
    RGBSpectrum k = list.getColor("k", RGBSpectrum(3.91295, 2.45285, 2.14219));
    std::string distributionType = list.getString("distribution", "beckmann");
    Float alphaU;
    Float alphaV;
    if (list.findFloat("alpha")) alphaU = alphaV = list.getFloat("alpha", 0.1);
    else {
        alphaU = list.getFloat("alphaU", 0.1);
        alphaV = list.getFloat("alphaV", 0.1);
    }
    return new RoughConductorMaterial(R, eta, k, alphaU, alphaV, distributionType);
}

RAINBOW_NAMESPACE_END

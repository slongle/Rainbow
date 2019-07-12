#include "openvdbvolume.h"

#include "openvdb/tools/Interpolation.h"

RAINBOW_NAMESPACE_BEGIN

OpenVDBVolume::OpenVDBVolume(
    const std::string& filename,
    const std::string& densityVolumeName,
    const std::string& emissionVolumeName)
    : m_filename(filename), m_densityVolumeName(densityVolumeName), m_emissionVolumeName(emissionVolumeName),
    m_densityVolume(nullptr), m_emissionVolume(nullptr)
{
    LoadOpenVDBFile();
}

Float OpenVDBVolume::LookUpDensityFloat(const Point3f& p) const
{
    Float ret;

    if (m_densityVolume->valueType() == "float") {
        openvdb::FloatGrid::Ptr concreteGrid = openvdb::gridPtrCast<openvdb::FloatGrid>(m_densityVolume);
        openvdb::FloatGrid::ConstAccessor accessor = concreteGrid->getConstAccessor();
        openvdb::tools::GridSampler<openvdb::FloatGrid::ConstAccessor, openvdb::tools::BoxSampler>
            sampler(accessor, concreteGrid->transform());
        ret = sampler.isSample(openvdb::Vec3f(p.x, p.y, p.z));
    } else {
        Assert(false, tfm::format("No supporting %s grid types", m_densityVolume->valueType()));
    }

    return ret;
}

RGBSpectrum OpenVDBVolume::LookUpSpectrum(const Point3f& p) const
{
    Assert(false, "No implemention");
    return RGBSpectrum();
}

RGBSpectrum OpenVDBVolume::LookUpEmissionSpectrum(const Point3f& p) const
{
    if (m_emissionVolume->valueType() == "float") {
        openvdb::FloatGrid::Ptr concreteGrid = openvdb::gridPtrCast<openvdb::FloatGrid>(m_emissionVolume);
        openvdb::FloatGrid::ConstAccessor accessor = concreteGrid->getConstAccessor();
        openvdb::tools::GridSampler<openvdb::FloatGrid::ConstAccessor, openvdb::tools::BoxSampler>
            sampler(accessor, concreteGrid->transform());
        return RGBSpectrum(sampler.isSample(openvdb::Vec3f(p.x, p.y, p.z)));
    } else if (m_emissionVolume->valueType() == "vec3s") {
        openvdb::Vec3SGrid::Ptr concreteGrid = openvdb::gridPtrCast<openvdb::Vec3SGrid>(m_emissionVolume);
        openvdb::Vec3SGrid::ConstAccessor accessor = concreteGrid->getConstAccessor();
        openvdb::tools::GridSampler<openvdb::Vec3SGrid::ConstAccessor, openvdb::tools::BoxSampler>
            sampler(accessor, concreteGrid->transform());
        auto value = sampler.isSample(openvdb::Vec3f(p.x, p.y, p.z));
        return RGBSpectrum(value[0], value[1], value[2]);
    } else {
        Assert(false, tfm::format("No supporting %s grid types", m_emissionVolume->valueType()));
    }
    return RGBSpectrum(0.);
}

void OpenVDBVolume::LoadOpenVDBFile()
{
    filesystem::path path = getFileResolver()->resolve(m_filename);
    openvdb::initialize();
    openvdb::io::File file(path.str());
    Assert(file.open(), tfm::format("Can't open file %s", path.str()));
    if (file.hasGrid(m_densityVolumeName)) {
        m_densityVolume = file.readGrid(m_densityVolumeName);
    } else {
        Assert(false, "No density grid");
    }
    if (file.hasGrid(m_emissionVolumeName)) {
        m_emissionVolume = file.readGrid(m_emissionVolumeName);
    }
    file.close();

    // Get max density
    if (m_densityVolume->valueType() == "float") {
        openvdb::FloatGrid::Ptr concreteGrid = openvdb::gridPtrCast<openvdb::FloatGrid>(m_densityVolume);
        openvdb::FloatGrid::Accessor accessor = concreteGrid->getAccessor();
        m_maxDensity = concreteGrid->background();
        for (openvdb::FloatGrid::ValueOnCIter iter = concreteGrid->cbeginValueOn(); iter; ++iter) {
            m_maxDensity = std::max(m_maxDensity, iter.getValue());
        }
    }
    else {
        Assert(false, "No supporting other OpenVDB types");
    }

    // Get bounds
    /// Density bounds
    openvdb::CoordBBox bbox = m_densityVolume->evalActiveVoxelBoundingBox();    
    Point3f minP(bbox.min().x(), bbox.min().y(), bbox.min().z());
    Point3f maxP(bbox.max().x() + 1, bbox.max().y() + 1, bbox.max().z() + 1);
    m_bounds = Bounds3f(minP, maxP);
    /// Emission bounds
    if (m_emissionVolume) {
        openvdb::CoordBBox bbox = m_densityVolume->evalActiveVoxelBoundingBox();
        Point3f minP(bbox.min().x(), bbox.min().y(), bbox.min().z());
        Point3f maxP(bbox.max().x() + 1, bbox.max().y() + 1, bbox.max().z() + 1);
        m_bounds = Union(m_bounds, Bounds3f(minP, maxP));
    }
}


OpenVDBVolume* CreateOpenVDBVolume(const PropertyList& list)
{
    std::string filename = list.getString("filename");
    std::string densityName = list.getString("density", "density");
    std::string emissionName = list.getString("emission", "emission");
    return new OpenVDBVolume(filename, densityName, emissionName);
}

RAINBOW_NAMESPACE_END

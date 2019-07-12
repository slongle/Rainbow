#pragma once
#pragma warning(disable:4146 2109)

#ifndef __RAINBOW_OPENVDBVOLUME_H
#define __RAINBOW_OPENVDBVOLUME_H

#include "openvdb/openvdb.h"

#include "core/volume.h"

RAINBOW_NAMESPACE_BEGIN

class OpenVDBVolume :public Volume {
public:
    OpenVDBVolume(
        const std::string& filename,
        const std::string& densityVolumeName,
        const std::string& emissionVolumeName);

    Float LookUpDensityFloat(const Point3f& p) const override;
    RGBSpectrum LookUpSpectrum(const Point3f& p) const override;
    RGBSpectrum LookUpEmissionSpectrum(const Point3f& p) const override;

private:
    void LoadOpenVDBFile();

    std::string m_filename;
    std::string m_densityVolumeName;
    std::string m_emissionVolumeName;
    openvdb::GridBase::Ptr m_densityVolume;
    openvdb::GridBase::Ptr m_emissionVolume;
};

OpenVDBVolume* CreateOpenVDBVolume(const PropertyList& list);

RAINBOW_NAMESPACE_END

#endif //__RAINBOW_OPENVDBVOLUME_H
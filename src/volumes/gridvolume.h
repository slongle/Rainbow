#pragma once
#ifndef __RAINBOW_GRIDMEDIUM_H
#define __RAINBOW_GRIDMEDIUM_H

#include "core/volume.h"

RAINBOW_NAMESPACE_BEGIN

class GridDensityVolume : public Volume {
public:
    GridDensityVolume(const std::string &filename);
    Float LookUpDensityFloat(const Point3f& p) const override;
    RGBSpectrum LookUpSpectrum(const Point3f& p) const override;
    RGBSpectrum LookUpEmissionSpectrum(const Point3f& p) const override;
private:
    RGBSpectrum GetSpectrumD(const Point3i &p) const;
    Float GetFloatD(const Point3i &p) const;
    Float GetD(const Point3i &p, const int &channel) const;
    void LoadFile(const std::string& filename);

    // Volume data
    enum EVolumeType {
        EFloat32 = 1,
        EFloat16 = 2,
        EUint8 = 3,
        EDirect = 4,
    };

    Transform m_volumeToGrid;
    std::string m_filename;
    EVolumeType m_type;
    int m_channel;
    Point3i m_resolution;
    uint8_t *m_data = nullptr;
};

GridDensityVolume * CreateGridDensityVolume(const PropertyList &list);

RAINBOW_NAMESPACE_END

#endif // __RAINBOW_GRIDMEDIUM_H
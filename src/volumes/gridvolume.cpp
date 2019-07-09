#include "gridvolume.h"
#include "utility/math.h"

RAINBOW_NAMESPACE_BEGIN

/**
 * Byte 1-3     "VOL"
 * Byte 4       File format version number (currently 3)
 * Byte 5-8     Identifier (32-bit integer)
 *              1 : Dense float32-based representation
 *              2 : Dense float16-based representation (half)
 *              3 : Dense uint8-based representation
 *                  (The range 0..255 will be mapped to 0..1)
 *              4 : Dense quantized directions.
 *                  The directions are stored in spherical
 *                  co-ordinates with a total storage cost
 *                  of 16 bit per entry.
 * Byte 9-12    Number of cells along the X axis (32 bit integer)
 * Byte 13-16   Number of cells along the Y axis (32 bit integer)
 * Byte 17-20   Number of cells along the Z axis (32 bit integer)
 * Byte 21-24   Number of channels
 *              (32 bit integer, supported values: 1 or 3)
 * Byte 25-48   Axis-aligned bounding box of the data stored in single precision
 *              (order : xmin, ymin, zmin, xmax, ymax, zmax)
 * Byte 49-*    Binary data of the volume stored in the specified encoding.
 *              data[((zpos*yres + ypos)*xres + xpos)*channels + chan]
 *              where (xpos, ypos, zpos, chan) denotes the lookup location.
 **/

void GridDensityVolume::LoadFile(const std::string& filename) {
    filesystem::path path = getFileResolver()->resolve(filename);
    std::string pathName = path.str();
    //cout << pathName << endl;
    //std::string pathName = "F:/Document/Graphics/code/Rainbow/scenes/hetvol/smoke.vol";
    FILE* stream = fopen(pathName.c_str(), "r");
    char header[3];
    fread(header, sizeof(char), 3, stream);
    assert(header[0] == 'V' && header[1] == 'O' && header[2] == 'L');
    uint8_t version;
    fread(&version, sizeof(uint8_t), 1, stream);
    assert(version == 3);

    fread(&m_type, sizeof(int), 1, stream);
    assert(1 <= m_type && m_type <= 4);
    int xResolution, yResolution, zResolution;
    fread(&xResolution, sizeof(int), 1, stream);
    fread(&yResolution, sizeof(int), 1, stream);
    fread(&zResolution, sizeof(int), 1, stream);
    m_resolution = Point3i(xResolution, yResolution, zResolution);
    
    fread(&m_channel, sizeof(int), 1, stream);
    assert(m_channel == 1 || m_channel == 3);

    Float xMin, xMax, yMin, yMax, zMin, zMax;
    fread(&xMin, sizeof(float), 1, stream);
    fread(&yMin, sizeof(float), 1, stream);
    fread(&zMin, sizeof(float), 1, stream);
    fread(&xMax, sizeof(float), 1, stream);
    fread(&yMax, sizeof(float), 1, stream);
    fread(&zMax, sizeof(float), 1, stream);
    m_bounds = Bounds3f(Point3f(xMin, yMin, zMin), Point3f(xMax, yMax, zMax));

    //uint8_t *data = nullptr;
    switch (m_type) {
    case EFloat32: {   
        m_data = (uint8_t *)new float[xResolution*yResolution*zResolution*m_channel];
        fread(m_data, sizeof(float), xResolution*yResolution*zResolution*m_channel, stream);
        Float *data = reinterpret_cast<Float *>(m_data);
        for (int i = 0; i < xResolution*yResolution*zResolution*m_channel; i++, data++) {
            if (i == 0) {
                m_maxDensity = *data;
            } else {
                m_maxDensity = std::max(m_maxDensity, *data);
            }
        }
        break;
    }
    case EFloat16:
        assert(false);
        //fread(m_data,sizeof(float)/2,xres*yres*zres*channel,stream);
        break;
    case EUint8:
        assert(false);
        //fread(m_data, sizeof(uint8_t), xResolution*yResolution*zResolution*m_channel, stream);
        break;
    case EDirect:
        assert(false);
        break;
    }
}


GridDensityVolume::GridDensityVolume(const std::string& filename) 
    : m_filename(filename) 
{
    LoadFile(m_filename);
    Vector3f extent = m_bounds.Diagonal();
    m_volumeToGrid = Scale((m_resolution[0] - 1) / extent[0],
        (m_resolution[1] - 1) / extent[1],
        (m_resolution[2] - 1) / extent[2]) * Translate(-Vector3f(m_bounds.pMin));
}

Float GridDensityVolume::LookUpFloat(
    const Point3f& _p) const
{
    Point3f p = m_volumeToGrid(_p);
    int x1 = (int)std::floor(p.x);
    int y1 = (int)std::floor(p.y);
    int z1 = (int)std::floor(p.z);
    int x2 = x1 + 1;
    int y2 = y1 + 1;
    int z2 = z1 + 1;

    if (x1<0 || y2<0 || z2<0 || x2>=m_resolution.x || y2>=m_resolution.y || z2>=m_resolution.z) {
        return 0.;
    }

    Point3i pCeil(x1, y1, z1);
    Float fx = p.x - x1, fy = p.y - y1, fz = p.z - z1;

    Float d00 = utility::lerp(GetFloatD(pCeil), GetFloatD(pCeil + Point3i(1, 0, 0)), fx);
    Float d01 = utility::lerp(GetFloatD(pCeil + Point3i(0, 1, 0)), GetFloatD(pCeil + Point3i(1, 1, 0)), fx);
    Float d10 = utility::lerp(GetFloatD(pCeil + Point3i(0, 0, 1)), GetFloatD(pCeil + Point3i(1, 0, 1)), fx);
    Float d11 = utility::lerp(GetFloatD(pCeil + Point3i(0, 1, 1)), GetFloatD(pCeil + Point3i(1, 1, 1)), fx);
    // Lerp with y
    Float d0 = utility::lerp(d00, d01, fy);
    Float d1 = utility::lerp(d10, d11, fy);
    // Lerp with z
    Float d = utility::lerp(d0, d1, fz);
    return d;
}

RGBSpectrum GridDensityVolume::LookUpSpectrum(
    const Point3f& p) const
{
    Assert(false, "No Implement");
    return RGBSpectrum(0.);
}

Float GridDensityVolume::GetMaxFloatValue() const 
{
    return m_maxDensity;
}

RGBSpectrum GridDensityVolume::GetSpectrumD(const Point3i& p) const {
    Assert(m_channel == 3, tfm::format("The .vol file %s is 1 channel", m_filename));
    Float x = GetD(p, 0);
    Float y = GetD(p, 1);
    Float z = GetD(p, 2);
    return RGBSpectrum(x, y, z);
}

Float GridDensityVolume::GetFloatD(const Point3i& p) const {
    Assert(m_channel == 1, tfm::format("The .vol file %s is 3 channels", m_filename));
    return GetD(p, 0);
}

Float GridDensityVolume::GetD(const Point3i& p, const int& channel) const {
    Assert(0 <= channel && channel < m_channel, 
        tfm::format("The .vol file %s only has %d channels", m_filename, m_channel));
    return reinterpret_cast<Float *>(m_data)
        [((p.z*m_resolution.y + p.y)*m_resolution.x + p.x)*m_channel + channel];
}

GridDensityVolume* CreateGridDensityVolume(const PropertyList& list) {
    std::string filename = list.getString("filename");
    return new GridDensityVolume(filename);
}

RAINBOW_NAMESPACE_END

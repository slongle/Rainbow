#include "grid.h"

RAINBOW_NAMESPACE_BEGIN

enum EVolType {
    EFloat32 = 1,
    EFloat16 = 2,
    EUint8 = 3,
    EDirect = 4,
};

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
    FILE* stream = fopen(filename.c_str(), "r");
    char header[3];
    fread(header, sizeof(char), 3, stream);
    assert(header[0] == 'V' && header[1] == 'O' && header[2] == 'L');
    uint8_t version;
    fread(&version, sizeof(uint8_t), 1, stream);
    assert(version == 3);
    int type;
    fread(&type, sizeof(int), 1, stream);
    assert(1 <= type && type <= 4);
    int xres, yres, zres;
    fread(&xres, sizeof(int), 1, stream);
    fread(&yres, sizeof(int), 1, stream);
    fread(&zres, sizeof(int), 1, stream);
    //std::cout<<xCeils<<' '<<yCeils<<' '<<zCeils<<std::endl;
    int channel;
    fread(&channel, sizeof(int), 1, stream);
    assert(channel == 1 || channel == 3);
    //std::cout<<channel<<std::endl;
    Float xmin, xmax, ymin, ymax, zmin, zmax;
    fread(&xmin, sizeof(float), 1, stream);
    fread(&xmax, sizeof(float), 1, stream);
    fread(&ymin, sizeof(float), 1, stream);
    fread(&ymax, sizeof(float), 1, stream);
    fread(&zmin, sizeof(float), 1, stream);
    fread(&zmax, sizeof(float), 1, stream);
    uint8_t* data = nullptr;
    switch (type)
    {
    case EFloat32:
        fread(data, sizeof(float), xres*yres*zres*channel, stream);
        break;
    case EFloat16:
        assert(false);
        //fread(data,sizeof(float)/2,xres*yres*zres*channel,stream);
        break;
    case EUint8:
        fread(data, sizeof(uint8_t), xres*yres*zres*channel, stream);
        break;
    case EDirect:
        assert(false);
        break;
    }
}

Float GridDensityVolume::LookUpFloat(
    const Float& x, 
    const Float& y, 
    const Float& z) const 
{
    return 0.;
}

RGBSpectrum GridDensityVolume::LookUpSpectrum(
    const Float& x, 
    const Float& y, 
    const Float& z) const 
{
    return RGBSpectrum(0.);
}


RAINBOW_NAMESPACE_END

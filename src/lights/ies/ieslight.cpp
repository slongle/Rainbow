#include <memory.h>
#include "ieslight.h"

RAINBOW_NAMESPACE_BEGIN

/*
#ifndef _MAX_DRIVE
#define _MAX_DRIVE 3
#endif // _MAX_DRIVE

#ifndef _MAX_DIR
#define _MAX_DIR 256
#endif // _MAX_DIR

#ifndef _MAX_FNAME
#define _MAX_FNAME 256
#endif // _MAX_FNAME

#ifndef _MAX_EXT
#define _MAX_EXT 256
#endif // _MAX_EXT
*/

IESLight::IESLight(
    const Transform& m_LightToWorld,
    const MediumInterface& m_mediumInterface,
    const std::string& path,
    const RGBSpectrum& m_I)
    : Light(m_LightToWorld, m_mediumInterface),
    I(m_I), pLight(LightToWorld(Point3f(0, 0, 0)))
{
    /*char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];

    std::memset(drive, 0, sizeof(drive));
    std::memset(dir, 0, sizeof(dir));
    std::memset(fname, 0, sizeof(fname));
    std::memset(ext, 0, sizeof(ext));

    _splitpath(path.c_str(), drive, dir, fname, ext);*/

    if (!IESLoader.load(path, info)) 
    {
        std::cerr << "Can't load ies file." << std::endl;
        std::cerr << info.error() << std::endl;
        exit(1);
    }
    
    if(!info.process()) 
    {
        std::cerr << "Can't process ies file." << std::endl;
        exit(1);
    }

    invMaxValue = info.computeInvMax();
}

Float IESLight::PdfLi(
    const Interaction&   ref,
    const Vector3f&      wi) const 
{
    return 0.;
}

RGBSpectrum IESLight::SampleLi(
    const Interaction&   intersection,
    const Point2f&       sample,
    Vector3f*            wi,
    Float*               pdf,
    Visibility*          vis) const
{
    *wi = Normalize(pLight - intersection.p);
    *pdf = 1;
    *vis = Visibility(intersection, Interaction(pLight, mediumInterface));
    Vector3f woLocal = Normalize(WorldToLight(-*wi));

    //float v_angle = acos(woLocal.z);
    //float h_angle = atan2f(woLocal.x, woLocal.y) + M_PI;

    Float v_angle = acos(-woLocal.z);
    Float h_angle = atan2f(woLocal.y, woLocal.x) + M_PI;

    Float att = info.interp(h_angle, v_angle);
    att *= invMaxValue;

    return att * I / DistanceSquare(intersection.p, pLight);
}

std::shared_ptr<IESLight> CreateIESLight(const PropertyList & list)
{
    Transform lightToWorld = list.getTransform("toWorld", Transform());
    MediumInterface mediumInterface;
    RGBSpectrum I = list.getColor("intensity", RGBSpectrum(1));
    std::string path = list.getString("path");
    return std::make_shared<IESLight>(
        lightToWorld, mediumInterface, path, I);
}

RAINBOW_NAMESPACE_END

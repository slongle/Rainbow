#pragma once
#ifndef __RAINBOW_MEDIUM_H
#define __RAINBOW_MEDIUM_H

#include "spectrum.h"

RAINBOW_NAMESPACE_BEGIN

class Medium{
public:
    virtual RGBSpectrum Tr() const = 0;
    virtual RGBSpectrum Sample() const = 0;
};

class MediumInterface {
public:
    MediumInterface(const Medium *medium) :inside(medium), outside(medium) {}
    MediumInterface(const Medium *m_inside, const Medium *m_outside) :inside(m_inside), outside(m_outside) {}
    bool IsMediumTransition() const { return inside != outside; }
    const Medium *inside, *outside;
};

RAINBOW_NAMESPACE_END

#endif // __RAINBOW_MEDIUM_H

// +----------------------------------------------------------------------
// | Project : Rainbow.
// | Add new functions.
// +----------------------------------------------------------------------
// | Copyright (c) 2019-2019.
// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#include <assert.h>
#include <algorithm>
#include <functional>
#include <fstream>
#include <iostream>

#include "iesparser.h"
#include "src/core/common.h"
#include <minwindef.h>

IESFileInfo::IESFileInfo()
    : _cachedIntegral(std::numeric_limits<float>::max())
    , _error("No data loaded")
{
}

bool
IESFileInfo::valid() const
{
    return _error.empty();
}

const std::string&
IESFileInfo::error() const
{
    return _error;
}

bool IESFileInfo::process() 
{
    if (_anglesH.size() == 0 || _anglesV.size() == 0) {
        return false;
    }

    if (typeOfPhotometric == 2) {
        //if (!process_type_b()) {
        //    return false;
        //}
    }
    else {
        assert(typeOfPhotometric == 1);
        if (!process_type_c()) {
            return false;
        }
    }

    assert(_anglesV[0] == 0.0f);
    assert(_anglesH[0] == 0.0f);
    assert(_anglesH[_anglesH.size() - 1] == 360.0f);

    /* Convert from deg to rad. */
    for (int i = 0; i < _anglesV.size(); i++) {
        _anglesV[i] *= M_PI / 180.f;
    }
    for (int i = 0; i < _anglesH.size(); i++) {
        _anglesH[i] *= M_PI / 180.f;
    }

    return true;
}

bool IESFileInfo::process_type_c() 
{
    if (_anglesH[0] == 90.0f) {
        /* Some files are stored from 90бу to 270бу, so we just rotate them to the regular 0бу-180бу range here. */
        for (int i = 0; i < _anglesV.size(); i++) {
            _anglesH[i] -= 90.0f;
        }
    }

    if (_anglesH[0] != 0.0f) {
        return false;
    }

    if (_anglesH.size() == 1) {
        _anglesH.push_back(360.0f);
        _candalaValues.push_back(_candalaValues[0]);
    }

    if (_anglesH[_anglesH.size() - 1] == 90.0f) {
        /* Only one quadrant is defined, so we need to mirror twice (from one to two, then to four).
         * Since the two->four mirroring step might also be required if we get an input of two quadrants,
         * we only do the first mirror here and later do the second mirror in either case. */
        int hnum = _anglesH.size();
        for (int i = hnum - 2; i >= 0; i--) {
            _anglesH.push_back(180.0f - _anglesH[i]);
            _candalaValues.push_back(_candalaValues[i]);
        }
        anglesNumH = _anglesH.size();
    }

    if (_anglesH[_anglesH.size() - 1] == 180.0f) {
        /* Mirror half to the full range. */
        int hnum = _anglesH.size();
        for (int i = hnum - 2; i >= 0; i--) {
            _anglesH.push_back(360.0f - _anglesH[i]);
            _candalaValues.push_back(_candalaValues[i]);
        }
        anglesNumH = _anglesH.size();
    }

    /* Some files skip the 360бу entry (contrary to standard) because it's supposed to be identical to the 0бу entry.
     * If the file has a discernible order in its spacing, just fix this. */
    if (_anglesH[_anglesH.size() - 1] != 360.0f) {
        int hnum = _anglesH.size();
        float last_step = _anglesH[hnum - 1] - _anglesH[hnum - 2];
        float first_step = _anglesH[1] - _anglesH[0];
        float difference = 360.0f - _anglesH[hnum - 1];
        anglesNumH = _anglesH.size();
        if (last_step == difference || first_step == difference) {
            _anglesH.push_back(360.0f);
            _candalaValues.push_back(_candalaValues[0]);
        }
        else {
            return false;
        }
    }

    float v_first = _anglesV[0], v_last = _anglesV[_anglesV.size() - 1];
    if (v_first == 90.0f) {
        if (v_last == 180.0f) {
            /* Flip to ensure that vertical angles always start at 0бу. */
            for (int i = 0; i < _anglesV.size(); i++) {
                _anglesV[i] = 180.0f - _anglesV[i];
            }
        }
        else {
            return false;
        }
    }
    else if (v_first != 0.0f) {
        return false;
    }

    return true;
}

float IESFileInfo::interpolate_ies_vertical(
    int     v, 
    int     v_num, 
    float   v_frac, 
    int     h) const
{
    /* Since lookups are performed in spherical coordinates, clamping the coordinates at the low end of v
    * (corresponding to the north pole) would result in artifacts.
    * The proper way of dealing with this would be to lookup the corresponding value on the other side of the pole,
    * but since the horizontal coordinates might be nonuniform, this would require yet another interpolation.
    * Therefore, the assumtion is made that the light is going to be symmetrical, which means that we can just take
    * the corresponding value at the current horizontal coordinate. */

#define IES_LOOKUP(v) _candalaValues[h][v] 
    /* If v is zero, assume symmetry and read at v=1 instead of v=-1. */
    float a = IES_LOOKUP((v == 0) ? 1 : v - 1);
    float b = IES_LOOKUP(v);
    float c = IES_LOOKUP(v + 1);
    float d = IES_LOOKUP(std::min(v + 2, v_num - 1));
#undef IES_LOOKUP

    return rainbow::CubicInterp(a, b, c, d, v_frac);
}

float IESFileInfo::computeInvMax() const 
{
    float candala = *std::max_element(_candalaValues[0].begin(), _candalaValues[0].end());
    for (int i = 1; i < _candalaValues.size(); i++)
        candala = std::max(candala, *std::max_element(_candalaValues[1].begin(), _candalaValues[1].end()));

    return 1.0f / candala;
}

float IESFileInfo::interp(
    float h_angle, 
    float v_angle) const
{    
#define IES_LOOKUP_ANGLE_H(h) _anglesH[h]
#define IES_LOOKUP_ANGLE_V(v) _anglesV[v]
    
    /* Check whether the angle is within the bounds of the IES texture. */
    if (v_angle >= IES_LOOKUP_ANGLE_V(anglesNumV - 1)) {
        return 0.0f;
    }
    assert(v_angle >= IES_LOOKUP_ANGLE_V(0));
    assert(h_angle >= IES_LOOKUP_ANGLE_H(0));
    assert(h_angle <= IES_LOOKUP_ANGLE_H(anglesNumH - 1));

    /* Lookup the angles to find the table position. */
    int h_i, v_i;
    /* TODO(lukas): Consider using bisection. Probably not worth it for the vast majority of IES files. */
    for (h_i = 0; IES_LOOKUP_ANGLE_H(h_i + 1) < h_angle; h_i++);
    for (v_i = 0; IES_LOOKUP_ANGLE_V(v_i + 1) < v_angle; v_i++);

    float h_frac = rainbow::InverseLerp(IES_LOOKUP_ANGLE_H(h_i), IES_LOOKUP_ANGLE_H(h_i + 1), h_angle);
    float v_frac = rainbow::InverseLerp(IES_LOOKUP_ANGLE_V(v_i), IES_LOOKUP_ANGLE_V(v_i + 1), v_angle);

#undef IES_LOOKUP_ANGLE_H
#undef IES_LOOKUP_ANGLE_V

    /* Perform cubic interpolation along the horizontal coordinate to get the intensity value.
     * If h_i is zero, just wrap around since the horizontal angles always go over the full circle.
     * However, the last entry (360бу) equals the first one, so we need to wrap around to the one before that. */
    float a = interpolate_ies_vertical(v_i, anglesNumV, v_frac, (h_i == 0) ? anglesNumH - 2 : h_i - 1);
    float b = interpolate_ies_vertical(v_i, anglesNumV, v_frac, h_i);
    float c = interpolate_ies_vertical(v_i, anglesNumV, v_frac, h_i + 1);
    /* Same logic here, wrap around to the second element if necessary. */
    float d = interpolate_ies_vertical(v_i, anglesNumV, v_frac, (h_i + 2 == anglesNumH) ? 1 : h_i + 2);

    /* Cubic interpolation can result in negative values, so get rid of them. */
    return std::max(rainbow::CubicInterp(a, b, c, d, h_frac), 0.0f);
}

IESLoadHelper::IESLoadHelper()
{
}

IESLoadHelper::~IESLoadHelper()
{
}

bool
IESLoadHelper::load(const std::string& path, IESFileInfo& info)
{
    std::fstream stream(path, std::ios_base::binary | std::ios_base::in);    
    if (!stream.is_open())
    {        
        std::cerr << "IES2HDR Error: Failed to open file :" << path << std::endl;
        return false;
    }

    stream.seekg(0, std::ios_base::end);
    std::streamsize streamSize = stream.tellg();
    stream.seekg(0, std::ios_base::beg);

    std::vector<std::uint8_t> IESBuffer;
    IESBuffer.resize(streamSize + 1);
    stream.read((char*)IESBuffer.data(), IESBuffer.size());

    assert(!info.valid());
    std::string data((char *)IESBuffer.data(), streamSize);

    std::string dataPos;

    std::string version;
    this->getLineContent(data, dataPos, version, false, false);

    if (version.empty())
    {
        info._error = "Unknown IES version";
        return false;
    }
    else if (version == "IESNA:LM-63-1995")
        info._version = "IESNA:LM-63-1995";
    else if (version == "IESNA91")
        info._version = "IESNA91";
    else if (version == "IESNA:LM-63-2002")
        info._version = "IESNA:LM-63-2002";
    else
        info._version = version;

    while (!dataPos.empty())
    {
        std::string line;
        this->getLineContent(dataPos, dataPos, line, false, false);

        if (line.compare(0, 9, "TILT=NONE", 9) == 0 ||
            line.compare(0, 10, "TILT= NONE", 10) == 0 ||
            line.compare(0, 10, "TILT =NONE", 10) == 0 ||
            line.compare(0, 11, "TILT = NONE", 11) == 0)
        {
            break;
        }
        else if (line.compare(0, 5, "TILT=", 5) == 0 ||
            line.compare(0, 5, "TILT =", 5) == 0)
        {
            info._error = "Not supported yet.";
            return false;
        }
    }

    this->getFloat(dataPos, dataPos, info.totalLights);
    if (info.totalLights < 0 || info.totalLights > std::numeric_limits<short>::max())
    {
        info._error = "Light Count is not valid";
        return false;
    }

    this->getFloat(dataPos, dataPos, info.totalLumens);
    if (info.totalLumens < 0)
    {
        info._error = "TotalLumens is not positive number";
        return false;
    }

    this->getFloat(dataPos, dataPos, info.candalaMult);
    if (info.candalaMult < 0)
    {
        info._error = "CandalaMult is not positive number";
        return false;
    }

    this->getInt(dataPos, dataPos, info.anglesNumV);
    if (info.anglesNumV < 0 || info.anglesNumV > std::numeric_limits<short>::max())
    {
        info._error = "VAnglesNum is not valid";
        return false;
    }

    this->getInt(dataPos, dataPos, info.anglesNumH);
    if (info.anglesNumH < 0 || info.anglesNumH > std::numeric_limits<short>::max())
    {
        info._error = "HAnglesNum is not valid";
        return false;
    }

    this->getInt(dataPos, dataPos, info.typeOfPhotometric);
    this->getInt(dataPos, dataPos, info.typeOfUnit);

    this->getFloat(dataPos, dataPos, info.width);
    this->getFloat(dataPos, dataPos, info.length);
    this->getFloat(dataPos, dataPos, info.height);

    this->getFloat(dataPos, dataPos, info.ballastFactor);
    this->getFloat(dataPos, dataPos, info.futureUse);
    this->getFloat(dataPos, dataPos, info.inputWatts);

    float minSoFarV = std::numeric_limits<float>::lowest();
    float minSoFarH = std::numeric_limits<float>::lowest();

    info._anglesV.reserve(info.anglesNumV);
    info._anglesH.reserve(info.anglesNumH);

    for (std::int32_t y = 0; y < info.anglesNumV; ++y)
    {
        float value;
        this->getFloat(dataPos, dataPos, value, true, true);

        if (value < minSoFarV)
        {
            info._error = "V Values is not valid";
            return false;
        }

        minSoFarV = value;
        info._anglesV.push_back(value);
    }

    for (std::int32_t x = 0; x < info.anglesNumH; ++x)
    {
        float value;
        this->getFloat(dataPos, dataPos, value, true, true);

        if (value < minSoFarH)
        {
            info._error = "H Values is not valid";
            return false;
        }

        minSoFarH = value;
        info._anglesH.push_back(value);
    }

    info._candalaValues.resize(info.anglesNumH);

    for (std::int32_t y = 0; y < info.anglesNumH; ++y)
    {
        for (std::int32_t x = 0; x < info.anglesNumV; ++x)
        {
            float value;
            this->getFloat(dataPos, dataPos, value, true, true);
            info._candalaValues[y].push_back(value * info.candalaMult);
        }
    }

    skipSpaceAndLineEnd(dataPos, dataPos);

    if (!dataPos.empty())
    {
        std::string line;
        this->getLineContent(dataPos, dataPos, line, true, false);

        if (line == "END")
            skipSpaceAndLineEnd(dataPos, dataPos);

        if (!dataPos.empty())
        {
            info._error = "Unexpected content after END.";
            return false;
        }
    }

    info._error.clear();

    return true;
}

bool
IESLoadHelper::saveAs1D(const IESFileInfo& info, float* data, std::uint32_t width, std::uint8_t channel) noexcept
{
    assert(data);
    assert(width > 0);
    assert(channel == 1 || channel == 3 || channel == 4);
    assert(info.valid());

    float invW = 1.0f / width;
    float invMaxValue = this->computeInvMax(info._candalaValues);

    for (std::uint32_t x = 0; x < width; ++x)
    {
        float fraction = x * invW;
        float value = invMaxValue * interpolate1D(info, fraction * 180.0f);

        switch (channel)
        {
        case 1:
            *data++ = value;
            break;
        case 3:
            *data++ = value;
            *data++ = value;
            *data++ = value;
            break;
        case 4:
            *data++ = value;
            *data++ = value;
            *data++ = value;
            *data++ = 1.0f;
            break;
        default:
            return false;
        }
    }

    return true;
}

bool
IESLoadHelper::saveAs2D(const IESFileInfo& info, float* data, std::uint32_t width, std::uint32_t height, std::uint8_t channel) noexcept
{
    assert(data);
    assert(width > 0 && height > 0);
    assert(channel == 1 || channel == 3 || channel == 4);
    assert(info.valid());

    float invW = 1.0f / width;
    float invH = 1.0f / height;
    float invMaxValue = this->computeInvMax(info._candalaValues);

    for (std::uint32_t y = 0; y < height; ++y)
    {
        for (std::uint32_t x = 0; x < width; ++x)
        {
            float fractionV = x * invW * 180.0f;
            float fractionH = y * invH * 180.0f;
            float value = invMaxValue * interpolate2D(info, fractionV, fractionH);

            switch (channel)
            {
            case 1:
                *data++ = value;
                break;
            case 3:
                *data++ = value;
                *data++ = value;
                *data++ = value;
                break;
            case 4:
                *data++ = value;
                *data++ = value;
                *data++ = value;
                *data++ = 1.0;
                break;
            default:
                return false;
            }
        }
    }

    return true;
}

bool
IESLoadHelper::saveAsPreview(const IESFileInfo& info, std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint8_t channel) noexcept
{
    assert(data);
    assert(width > 0 && height > 0);
    assert(channel == 1 || channel == 3 || channel == 4);
    assert(info.valid());

    std::vector<float> ies(256);
    if (!this->saveAs1D(info, ies.data(), ies.size(), 1))
        return false;

    float maxValue = this->computeInvMax(info._candalaValues);

    auto TonemapHable = [](float x)
    {
        const float A = 0.22f;
        const float B = 0.30f;
        const float C = 0.10f;
        const float D = 0.20f;
        const float E = 0.01f;
        const float F = 0.30f;
        return ((x*(A*x + C * B) + D * E) / (x*(A*x + B) + D * F)) - E / F;
    };

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float u = ((float)x / width) * 2.0f - 1.0f;
            float v = 1.0f - ((float)y / height) * 2.0f - 1.0f;

            u *= 2.2f;
            v *= 2.4f;

            // float3(0.0f, 0.0f, -0.5f) - ray::float3(u, v, 0.0f)
            float lx = +0.0f - u;
            float ly = +0.0f - v;
            float lz = -0.5f - 0.0f;

            // normalize
            float length = std::sqrt(lx * lx + ly * ly + lz * lz);
            lx /= length;
            ly /= length;
            lz /= length;

            float angle = 1.0 - std::acos(lx * 0.0 + ly * -1.0 + lz * 0.0f) / 3.141592654;

            float intensity = ies[angle * 255] * maxValue / length;

            std::uint8_t value = std::min(std::max((int)std::floor(TonemapHable(intensity) / TonemapHable(maxValue) * 255.0f), 0), 255);

            switch (channel)
            {
            case 1:
                *data++ = value;
                break;
            case 3:
                *data++ = value;
                *data++ = value;
                *data++ = value;
                break;
            case 4:
                *data++ = value;
                *data++ = value;
                *data++ = value;
                *data++ = 1.0;
                break;
            default:
                return false;
            }
        }
    }

    return true;
}

float
IESLoadHelper::computeInvMax(const std::vector< std::vector<float> >& candalaValues) const
{
    assert(candalaValues.size());
    
    float candala = *std::max_element(candalaValues[0].begin(), candalaValues[0].end());
    for (int i = 1; i < candalaValues.size(); i++) 
        candala = std::max(candala, *std::max_element(candalaValues[1].begin(), candalaValues[1].end()));
    
    return 1.0f / candala;
}

float
IESLoadHelper::computeFilterPos(float value, const std::vector<float>& angles) const
{
    assert(angles.size());

    std::size_t start = 0;
    std::size_t end = angles.size() - 1;

    if (value < angles[start]) return 0.0f;
    if (value > angles[end]) return (float)end;

    while (start < end)
    {
        std::size_t index = (start + end + 1) / 2;

        float angle = angles[index];
        if (value >= angle)
        {
            assert(start != index);
            start = index;
        }
        else
        {
            assert(end != index - 1);
            end = index - 1;
        }
    }

    float leftValue = angles[start];
    float fraction = 0.0f;

    if (start + 1 < (std::uint32_t)angles.size())
    {
        float rightValue = angles[start + 1];
        float deltaValue = rightValue - leftValue;

        if (deltaValue > 0.0001f)
        {
            fraction = (value - leftValue) / deltaValue;
        }
    }

    return start + fraction;
}

float
IESLoadHelper::interpolate1D(const IESFileInfo& info, float angle) const
{
    float angleV = this->computeFilterPos(angle, info._anglesV);
    float anglesNum = (float)info._anglesH.size();
    float angleTotal = 0.0f;

    for (float x = 0; x < anglesNum; x++)
        angleTotal += this->interpolateBilinear(info, x, angleV);

    return angleTotal / anglesNum;
}

float
IESLoadHelper::interpolate2D(const IESFileInfo& info, float angleV, float angleH) const
{
    float u = this->computeFilterPos(angleH, info._anglesH);
    float v = this->computeFilterPos(angleV, info._anglesV);
    return this->interpolateBilinear(info, u, v);
}

float
IESLoadHelper::interpolatePoint(const IESFileInfo& info, std::uint32_t x, std::uint32_t y) const
{
    assert(x >= 0);
    assert(y >= 0);

    std::size_t anglesNumH = info._anglesH.size();
    std::size_t anglesNumV = info._anglesV.size();

    x %= anglesNumH;
    y %= anglesNumV;

    assert(x < anglesNumH);
    assert(y < anglesNumV);

    return info._candalaValues[y][x];
}

float
IESLoadHelper::interpolateBilinear(const IESFileInfo& info, float x, float y) const
{
    int ix = (int)std::floor(x);
    int iy = (int)std::floor(y);

    float fracX = x - ix;
    float fracY = y - iy;

    float p00 = this->interpolatePoint(info, ix + 0, iy + 0);
    float p10 = this->interpolatePoint(info, ix + 1, iy + 0);
    float p01 = this->interpolatePoint(info, ix + 0, iy + 1);
    float p11 = this->interpolatePoint(info, ix + 1, iy + 1);

    auto lerp = [](float t1, float t2, float t3) -> float { return t1 + (t2 - t1) * t3; };

    float p0 = lerp(p00, p01, fracY);
    float p1 = lerp(p10, p11, fracY);

    return lerp(p0, p1, fracX);
}

void
IESLoadHelper::skipSpaceAndLineEnd(const std::string& data, std::string& out, bool stopOnComma)
{
    std::size_t dataBegin = 0;
    std::size_t dataEnd = data.size();

    while (dataBegin < dataEnd)
    {
        if (data[dataBegin] != '\r' && data[dataBegin] != '\n' && data[dataBegin] > ' ')
            break;
        dataBegin++;
    }

    if (stopOnComma)
    {
        while (dataBegin < dataEnd)
        {
            if (data[dataBegin] != ',')
                break;
            dataBegin++;
        }
    }

    out = data.substr(dataBegin, data.size() - dataBegin);
}

void
IESLoadHelper::getLineContent(const std::string& data, std::string& next, std::string& line, bool stopOnWhiteSpace, bool stopOnComma)
{
    skipSpaceAndLineEnd(data, next);

    auto it = data.begin();
    auto end = data.end();

    for (; it < end; ++it)
    {
        if ((*it == '\r') ||
            (*it == '\n') ||
            (*it <= ' ' && stopOnWhiteSpace) ||
            (*it == ',' && stopOnComma))
        {
            break;
        }
    }

    line.assign(data, 0, it - data.begin());
    next.assign(data, it - data.begin(), end - it);

    skipSpaceAndLineEnd(next, next, stopOnComma);
}

void
IESLoadHelper::getFloat(const std::string& data, std::string& next, float& ret, bool stopOnWhiteSpace, bool stopOnComma)
{
    std::string line;
    getLineContent(data, next, line, stopOnWhiteSpace, stopOnComma);
    assert(!line.empty());
    ret = (float)std::atof(line.c_str());
}

void
IESLoadHelper::getInt(const std::string& data, std::string& next, std::int32_t& ret, bool stopOnWhiteSpace, bool stopOnComma)
{
    std::string line;
    getLineContent(data, next, line, stopOnWhiteSpace, stopOnComma);
    assert(!line.empty());
    ret = std::atoi(line.c_str());
}
/*
 * Copyright (c) 2022, Benichou Software
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the author nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cmath>
#include "GeoPolyline.h"

namespace
{
	int DecodeNumber(const std::string& strEncoded, size_t& index)
	{
		char car;
		int iDecode = 0;
		int iNum = 0;

		do
		{
			car = static_cast<char>(strEncoded[index] - 63);
			iDecode |= (car & 0x1f) << iNum;
			iNum += 5;

			index++;
		} while (car >= 0x20);

		return iDecode;
	}

	int DecodeSignedNumber(const std::string& strEncoded, size_t& index)
	{
		int iDecode = DecodeNumber(strEncoded, index);
		return (iDecode & 1) ? ~(iDecode >> 1) : (iDecode >> 1);
	}
}

using namespace geo;

#define DEFAULT_RGBA	0x000000FF

CGeoPolyline::CGeoPolyline() :
	m_uiRGBA(DEFAULT_RGBA)
{
}

CGeoPolyline::CGeoPolyline(const CGeoLatLngs& geoLatLngs) :
	m_geoLatLngs(geoLatLngs),
	m_uiRGBA(DEFAULT_RGBA)
{
}

CGeoPolyline::CGeoPolyline(CGeoLatLngs&& geoLatLngs) :
	m_geoLatLngs(std::move(geoLatLngs)),
	m_uiRGBA(DEFAULT_RGBA)
{
}

CGeoPolyline::CGeoPolyline(const CGeoPolyline& gPolyline) :
	m_geoLatLngs(gPolyline.m_geoLatLngs),
	m_uiRGBA(gPolyline.m_uiRGBA)
{
}

CGeoPolyline::CGeoPolyline(CGeoPolyline&& gPolyline) :
	m_geoLatLngs(std::move(gPolyline.m_geoLatLngs)),
	m_uiRGBA(gPolyline.m_uiRGBA)
{
}

CGeoPolyline& CGeoPolyline::operator+=(const CGeoPolyline& gPolyline)
{
	m_geoLatLngs += gPolyline.m_geoLatLngs;
	return *this;
}

CGeoPolyline& CGeoPolyline::operator+=(CGeoPolyline&& gPolyline)
{
	m_geoLatLngs += std::move(gPolyline.m_geoLatLngs);
	return *this;
}

CGeoPolyline& CGeoPolyline::operator=(const CGeoPolyline& gPolyline)
{
	m_geoLatLngs = gPolyline.m_geoLatLngs;
	m_uiRGBA = gPolyline.m_uiRGBA;
	return *this;
}

CGeoPolyline& CGeoPolyline::operator=(CGeoPolyline&& gPolyline)
{
	m_geoLatLngs = std::move(gPolyline.m_geoLatLngs);
	m_uiRGBA = gPolyline.m_uiRGBA;
	return *this;
}

void CGeoPolyline::clear() throw()
{
	m_geoLatLngs.clear();
	m_uiRGBA = DEFAULT_RGBA;
}

CGeoLatLngs& CGeoPolyline::getPath()
{
	return m_geoLatLngs;
}

const CGeoLatLngs& CGeoPolyline::getPath() const
{
	return m_geoLatLngs;
}

void CGeoPolyline::setPath(const CGeoLatLngs& geoLatLngs)
{
	m_geoLatLngs = geoLatLngs;
}

void CGeoPolyline::setPath(CGeoLatLngs&& geoLatLngs)
{
	m_geoLatLngs = std::move(geoLatLngs);
}

unsigned int CGeoPolyline::getColor() const throw()
{
	return m_uiRGBA & 0x00FFFFFF;
}

void CGeoPolyline::setColor(unsigned int uiColor) throw()
{
	m_uiRGBA = (m_uiRGBA & 0xFF000000) | (uiColor & 0x00FFFFFF);
}

void CGeoPolyline::fromEncoded(const std::string& strEncodedPolyline, size_t precision)
{
	m_geoLatLngs.clear();

	size_t len = strEncodedPolyline.size(); // Decode path
	size_t index = 0;
	double dPres = 1;
	int iLate = 0;
	int	iLnge = 0;

	for (size_t i = 0; i < precision; ++i)
		dPres *= 10;

	while (index < len)
	{
		iLate += DecodeSignedNumber(strEncodedPolyline, index);
		iLnge += DecodeSignedNumber(strEncodedPolyline, index);

		m_geoLatLngs.push_back(CGeoLatLng(iLate / dPres, iLnge / dPres));
	}
}

CGeoPolyline geo::operator+(const CGeoPolyline& gPolyline1, const CGeoPolyline& gPolyline2)
{
	CGeoPolyline gPolyline(gPolyline1);
	gPolyline += gPolyline2;

	return gPolyline;
}

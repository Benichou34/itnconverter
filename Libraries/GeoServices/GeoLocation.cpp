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

#include "GeoLocation.h"

using namespace geo;

CGeoLocation::CGeoLocation()
{
}

CGeoLocation::CGeoLocation(const CGeoLocation& gLocation) : CGeoLatLng(gLocation),
m_strName(gLocation.m_strName),
m_strComment(gLocation.m_strComment)
{
}

CGeoLocation::CGeoLocation(CGeoLocation&& gLocation) : CGeoLatLng(std::move(gLocation)),
m_strName(std::move(gLocation.m_strName)),
m_strComment(std::move(gLocation.m_strComment))
{
}

CGeoLocation::CGeoLocation(const CGeoLatLng& gLatLng, const std::string& strName, const std::string& strComment) : CGeoLatLng(gLatLng),
m_strName(strName),
m_strComment(strComment)
{
}

CGeoLocation::~CGeoLocation()
{
}

bool CGeoLocation::operator==(const CGeoLocation& gLocation) const
{
	return (CGeoLatLng::operator==(gLocation) && m_strName == gLocation.m_strName && m_strComment == gLocation.m_strComment);
}

bool CGeoLocation::operator!=(const CGeoLocation& gLocation) const
{
	return !operator==(gLocation);
}

CGeoLocation& CGeoLocation::operator= (const CGeoLatLng& gLatLng)
{
	CGeoLatLng::operator= (gLatLng);
	return *this;
}

CGeoLocation& CGeoLocation::operator= (const CGeoLocation& gLocation)
{
	if (&gLocation != this)
	{
		CGeoLatLng::operator= (gLocation);
		m_strName = gLocation.m_strName;
		m_strComment = gLocation.m_strComment;
	}

	return *this;
}

CGeoLocation& CGeoLocation::operator= (CGeoLocation&& gLocation)
{
	if (&gLocation != this)
	{
		CGeoLatLng::operator= (std::move(gLocation));
		m_strName = std::move(gLocation.m_strName);
		m_strComment = std::move(gLocation.m_strComment);
	}

	return *this;
}

void CGeoLocation::clear()
{
	CGeoLatLng::clear();
	m_strName.clear();
	m_strComment.clear();
}

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
 *
 * Purpose : Reader for MapFactor GPS Navigation (.xml)
 *           http://navigatorfree.mapfactor.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "stdx/string_helper.h"
#include "mpfctrHeader.h"

const std::string MapFactor::MK_ROUTE("routing_points");
const std::string MapFactor::MK_SET("set");
const std::string MapFactor::MK_NAME("name");
const std::string MapFactor::MK_LATITUDE("lat");
const std::string MapFactor::MK_LONGITUDE("lon");
const std::string MapFactor::MK_DEPARTURE("departure");
const std::string MapFactor::MK_WAYPOINT("waypoint");
const std::string MapFactor::MK_DESTINATION("destination");

const int MapFactor::COORDS_FACTOR(3600000);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapFactorContentHandler::MapFactorContentHandler(std::vector<CGpsPointArray*>& vecGpsArray) :
	m_vecGpsArray(vecGpsArray),
	m_bOnRoutingPoints(false),
	m_bOnPoint(false)
{
}

MapFactorContentHandler::~MapFactorContentHandler()
{
}

bool MapFactorContentHandler::Parse(const wchar_t* szFileName)
{
	m_bOnRoutingPoints = false;
	m_bOnPoint = false;

	std::ifstream ifsXmlFile(szFileName);
	if (!ifsXmlFile)
		return false;

	try
	{
		ifsXmlFile >> *this;
	}
	catch (XML_Error& e)
	{
		OutputDebugStringA(GetErrorString(e));
		return false;
	}

	return true;
}

void MapFactorContentHandler::OnStartElement(const XML_Char* name, const XML_Char** attrs)
{
	CSAXParser::OnStartElement(name, attrs);
	std::string str(name);

	if (str == MapFactor::MK_ROUTE)
	{
		m_bOnRoutingPoints = true;
	}
	else if (m_bOnRoutingPoints && str == MapFactor::MK_SET)
	{
		m_pGpsCurrentArray.reset(new CGpsRoute);
	}
	else if (m_pGpsCurrentArray.get() && (str == MapFactor::MK_DEPARTURE || str == MapFactor::MK_WAYPOINT || str == MapFactor::MK_DESTINATION))
	{
		m_cGpsPoint.clear();
		m_bOnPoint = true;
	}
}

void MapFactorContentHandler::OnEndElement(const XML_Char* name)
{
	std::string str(name);

	if (str == MapFactor::MK_ROUTE)
	{
		m_bOnRoutingPoints = false;
		m_bOnPoint = false;
	}
	else if (str == MapFactor::MK_SET)
	{
		m_vecGpsArray.push_back(m_pGpsCurrentArray.release());
		m_bOnPoint = false;
	}
	else if (m_pGpsCurrentArray.get() && (str == MapFactor::MK_DEPARTURE || str == MapFactor::MK_WAYPOINT || str == MapFactor::MK_DESTINATION))
	{
		m_pGpsCurrentArray->push_back(m_cGpsPoint);
		m_cGpsPoint.clear();

		m_bOnPoint = false;
	}
	else if (str == MapFactor::MK_NAME)
	{
		if (m_bOnPoint)
			m_cGpsPoint.name(getCharacterData());
		else if (m_pGpsCurrentArray.get())
			m_pGpsCurrentArray->name(getCharacterData());
	}
	else if (str == MapFactor::MK_LATITUDE)
	{
		m_cGpsPoint.lat(stdx::string_helper::string_to<double>(getCharacterData()) / MapFactor::COORDS_FACTOR);
	}
	else if (str == MapFactor::MK_LONGITUDE)
	{
		m_cGpsPoint.lng(stdx::string_helper::string_to<double>(getCharacterData()) / MapFactor::COORDS_FACTOR);
	}
}

int ReadMPFCTR(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	return MapFactorContentHandler(vecGpsArray).Parse(strPathName.c_str()) ? S_OK : S_FALSE;
}

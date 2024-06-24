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
 * Purpose : Reader for OpenStreetMap XML file (.osm)
 *           http://wiki.openstreetmap.org/wiki/Map_Features
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "stdx/string_helper.h"
#include "osmReader.h"

namespace
{
	const std::string XML_MK_OSM("osm");
	const std::string XML_MK_NODE("node");
	const std::string XML_MK_WAY("way");
	const std::string XML_MK_NODE_REF("nd");
	const std::string XML_MK_TAG("tag");

	const std::string XML_VL_ID("id");
	const std::string XML_VL_LAT("lat");
	const std::string XML_VL_LON("lon");
	const std::string XML_VL_REF("ref");
	const std::string XML_VL_KEY("k");
	const std::string XML_VL_VALUE("v");
	const std::string XML_VL_NAME("name");
	const std::string XML_VL_NOTE("note");
	const std::string XML_VL_ELE("ele");
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OSMContentHandler::OSMContentHandler(std::vector<CGpsPointArray*>& vecGpsArray) :
	m_vecGpsArray(vecGpsArray),
	m_bOnNode(false),
	m_bOnWay(false),
	m_pointIndex(0)
{
}

OSMContentHandler::~OSMContentHandler()
{
}

bool OSMContentHandler::Parse(std::istream& iss)
{
	m_bOnNode = false;
	m_bOnWay = false;
	m_pGpsWayPointArray.reset(new CGpsWaypointArray);
	m_mapIndex.clear();

	try
	{
		iss >> *this;

		if (!m_pGpsWayPointArray->empty())
			m_vecGpsArray.push_back(m_pGpsWayPointArray.release());
	}
	catch (XML_Error& e)
	{
		OutputDebugStringA(GetErrorString(e));
		return false;
	}

	return true;
}

bool OSMContentHandler::Parse(const wchar_t* szFileName)
{
	std::ifstream ifsXmlFile(szFileName);
	if (!ifsXmlFile)
		return false;

	return Parse(ifsXmlFile);
}

void OSMContentHandler::OnStartElement(const XML_Char* name, const XML_Char** attrs)
{
	CSAXParser::OnStartElement(name, attrs);
	std::string str(name);

	if (str == XML_MK_NODE)
	{
		Attributes mapAttributes;
		getAttributes(attrs, mapAttributes);

		m_bOnNode = true;
		m_pointIndex = stdx::string_helper::string_to<long long>(mapAttributes[XML_VL_ID]);
		m_cGpsPoint.lat(stdx::string_helper::string_to<double>(mapAttributes[XML_VL_LAT]));
		m_cGpsPoint.lng(stdx::string_helper::string_to<double>(mapAttributes[XML_VL_LON]));
	}
	else if (str == XML_MK_WAY)
	{
		Attributes mapAttributes;
		getAttributes(attrs, mapAttributes);

		m_bOnWay = true;
		m_vecGpsArray.push_back(new CGpsRoute);
		m_vecGpsArray.back()->name(mapAttributes[XML_VL_ID]);
	}
	else if (str == XML_MK_TAG)
	{
		Attributes mapAttributes;
		getAttributes(attrs, mapAttributes);

		const std::string& strKey = mapAttributes[XML_VL_KEY];
		const std::string& strValue = mapAttributes[XML_VL_VALUE];

		if (m_bOnNode)
		{
			if (strKey == XML_VL_NAME)
				m_cGpsPoint.name(strValue);
			else if (strKey == XML_VL_NOTE)
				m_cGpsPoint.comment(strValue);
			else if (strKey == XML_VL_ELE)
				m_cGpsPoint.alt(stdx::string_helper::string_to<double>(strValue));
		}
		else if (m_bOnWay && strKey == XML_VL_NAME)
		{
			m_vecGpsArray.back()->name(strValue);
		}
	}
	else if (str == XML_MK_NODE_REF && m_bOnWay)
	{
		Attributes mapAttributes;
		getAttributes(attrs, mapAttributes);

		long long index = stdx::string_helper::string_to<long long>(mapAttributes[XML_VL_REF]);
		m_vecGpsArray.back()->push_back(*m_mapIndex[index]);
	}
}

void OSMContentHandler::OnEndElement(const XML_Char* name)
{
	std::string str(name);

	if (str == XML_MK_NODE)
	{
		m_pGpsWayPointArray->push_back(m_cGpsPoint);
		m_mapIndex[m_pointIndex] = &m_pGpsWayPointArray->back();

		m_cGpsPoint.clear();
		m_bOnNode = false;
	}
	else if (str == XML_MK_WAY)
	{
		m_bOnWay = false;
	}
}

int ReadOSM(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	return OSMContentHandler(vecGpsArray).Parse(strPathName.c_str()) ? S_OK : S_FALSE;
}

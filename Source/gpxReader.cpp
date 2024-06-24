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
 * Purpose : Reader for GPS exchange file (.gpx)
 *           www.topografix.com/gpx.asp
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "gpxReader.h"

#define XML_MK_GPX    "gpx"
#define XML_MK_RTE    "rte"
#define XML_MK_RTEPT  "rtept"
#define XML_MK_TRK    "trk"
#define XML_MK_TRKPT  "trkpt"
#define XML_MK_WPT    "wpt"
#define XML_MK_NAME   "name"
#define XML_MK_ELE    "ele"
#define XML_MK_CMT    "cmt"
#define XML_MK_DESC   "desc"

#define XML_VL_LAT    "lat"
#define XML_VL_LON    "lon"

 //////////////////////////////////////////////////////////////////////
 // Construction/Destruction
 //////////////////////////////////////////////////////////////////////

GPXContentHandler::GPXContentHandler(std::vector<CGpsPointArray*>& vecGpsArray) :
	m_vecGpsArray(vecGpsArray),
	m_pGpsCurrentArray(nullptr),
	m_pGpsWayPointArray(nullptr),
	m_bOnPoint(false)
{
}

GPXContentHandler::~GPXContentHandler()
{
	delete m_pGpsWayPointArray;
	if (m_pGpsCurrentArray != m_pGpsWayPointArray)
		delete m_pGpsCurrentArray;
}

bool GPXContentHandler::Parse(std::istream& iss)
{
	m_bOnPoint = false;
	m_pGpsCurrentArray = nullptr;
	m_pGpsWayPointArray = nullptr;
	m_strCurrentTag.clear();

	try
	{
		iss >> *this;
	}
	catch (XML_Error& e)
	{
		OutputDebugStringA(GetErrorString(e));
		return false;
	}

	return true;
}

bool GPXContentHandler::Parse(const wchar_t* szFileName)
{
	std::ifstream ifsXmlFile(szFileName);
	if (!ifsXmlFile)
		return false;

	return Parse(ifsXmlFile);
}

void GPXContentHandler::OnStartElement(const XML_Char* name, const XML_Char** attrs)
{
	std::string str(name);
	m_strData.clear();

	if (str == XML_MK_WPT && !m_pGpsCurrentArray)
	{
		if (!m_pGpsWayPointArray)
			m_pGpsWayPointArray = new CGpsWaypointArray();

		m_pGpsCurrentArray = m_pGpsWayPointArray;
		if (m_pGpsCurrentArray->getType() == CGpsPointArray::E_ARRAY_WPT && m_strCurrentTag.empty())
			m_strCurrentTag = XML_MK_WPT;
	}

	if (str == m_strCurrentTag)
	{
		m_cGpsPoint.clear();
		m_bOnPoint = true;

		const XML_Char** it_attrs = attrs;
		while (*it_attrs)
		{
			std::string strName(*it_attrs);
			it_attrs++;
			std::string strValue(*it_attrs);
			it_attrs++;

			if (strName == XML_VL_LAT)
				m_cGpsPoint.lat(stdx::string_helper::string_to<double>(strValue));
			else if (strName == XML_VL_LON)
				m_cGpsPoint.lng(stdx::string_helper::string_to<double>(strValue));
		}
	}
	else if (str == XML_MK_RTE)
	{
		if (m_pGpsCurrentArray != m_pGpsWayPointArray)
			delete m_pGpsCurrentArray;

		m_pGpsCurrentArray = new CGpsRoute();

		if (m_pGpsCurrentArray->getType() == CGpsPointArray::E_ARRAY_ROUTE)
			m_strCurrentTag = XML_MK_RTEPT;
	}
	else if (str == XML_MK_TRK)
	{
		if (m_pGpsCurrentArray != m_pGpsWayPointArray)
			delete m_pGpsCurrentArray;

		m_pGpsCurrentArray = new CGpsTrack();

		if (m_pGpsCurrentArray->getType() == CGpsPointArray::E_ARRAY_TRACK)
			m_strCurrentTag = XML_MK_TRKPT;
	}
}

void GPXContentHandler::OnEndElement(const XML_Char* name)
{
	std::string str(name);

	if (str == m_strCurrentTag)
	{
		if (m_pGpsCurrentArray)
			m_pGpsCurrentArray->push_back(m_cGpsPoint);

		m_cGpsPoint.clear();
		m_bOnPoint = false;
	}
	else if (str == XML_MK_NAME)
	{
		if (m_bOnPoint)
			m_cGpsPoint.name(m_strData);
		else if (!m_strCurrentTag.empty() && m_pGpsCurrentArray && m_pGpsCurrentArray->name().empty())
			m_pGpsCurrentArray->name(m_strData);
	}
	else if (str == XML_MK_CMT)
	{
		m_cGpsPoint.comment(m_strData);
	}
	else if (str == XML_MK_DESC && m_cGpsPoint.comment().empty())
	{
		m_cGpsPoint.comment(m_strData);
	}
	else if (str == XML_MK_ELE)
	{
		m_cGpsPoint.alt(stdx::string_helper::string_to<double>(m_strData));
	}
	else if (str == XML_MK_RTE || str == XML_MK_TRK)
	{
		if (m_pGpsCurrentArray && !m_pGpsCurrentArray->empty())
			m_vecGpsArray.push_back(m_pGpsCurrentArray);

		m_pGpsCurrentArray = nullptr;
		m_strCurrentTag.clear();
	}
	else if (str == XML_MK_GPX)
	{
		if (m_pGpsWayPointArray && !m_pGpsWayPointArray->empty())
			m_vecGpsArray.push_back(m_pGpsWayPointArray);

		if (m_pGpsCurrentArray == m_pGpsWayPointArray)
			m_pGpsCurrentArray = nullptr;

		m_pGpsWayPointArray = nullptr;
		m_strCurrentTag.clear();
	}
}

void GPXContentHandler::OnCharacterData(const XML_Char* data, int len)
{
	m_strData.append(data, len);
}

int ReadGPX(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	return GPXContentHandler(vecGpsArray).Parse(strPathName.c_str()) ? S_OK : S_FALSE;
}

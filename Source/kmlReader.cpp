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
 * Purpose : Reader for Google Earth file (.kml)
 *           http://earth.google.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "kmlReader.h"

#define XML_MK_KML         "kml"
#define XML_MK_DOCUMENT    "Document"
#define XML_MK_FOLDER      "Folder"
#define XML_MK_PLACEMARK   "Placemark"
#define XML_MK_NAME        "name"
#define XML_MK_SNIPPET     "Snippet"
#define XML_MK_DESCRIPTION "description"
#define XML_MK_POINT       "Point"
#define XML_MK_COORDS      "coordinates"
#define XML_MK_LINESTRING  "LineString"

#define XML_MK_GXTOUR      "gx:Tour"
#define XML_MK_GXFLYTO     "gx:FlyTo"
#define XML_MK_LONGITUDE   "longitude"
#define XML_MK_LATITUDE    "latitude"
#define XML_MK_ALTITUDE    "altitude"


 //////////////////////////////////////////////////////////////////////
 // Construction/Destruction
 //////////////////////////////////////////////////////////////////////

KMLContentHandler::KMLContentHandler(std::vector<CGpsPointArray*>& vecGpsArray) :
	m_vecGpsArray(vecGpsArray),
	m_bOnPlacemark(false),
	m_bOnFlyTo(false),
	m_bOnLineString(false)
{
}

KMLContentHandler::~KMLContentHandler()
{
	while (!m_stkDocument.empty())
	{
		delete m_stkDocument.top().pGpsPointArray;
		m_stkDocument.pop();
	}
}

bool KMLContentHandler::Parse(const wchar_t* szFileName)
{
	m_bOnPlacemark = false;
	m_bOnFlyTo = false;
	m_bOnLineString = false;

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

void KMLContentHandler::OnStartElement(const XML_Char* name, const XML_Char**)
{
	std::string str(name);
	m_strData.clear();

	if (str == XML_MK_DOCUMENT || str == XML_MK_FOLDER)
	{
		sttGpsPointArray gpsPointArray = { str, new CGpsWaypointArray() };
		m_stkDocument.push(gpsPointArray);
	}
	else if (str == XML_MK_GXTOUR)
	{
		sttGpsPointArray gpsPointArray = { str, new CGpsRoute() };
		m_stkDocument.push(gpsPointArray);
	}
	else if (str == XML_MK_PLACEMARK)
	{
		m_bOnPlacemark = true;
		m_cGpsPoint.clear();
	}
	else if (str == XML_MK_GXFLYTO)
	{
		m_bOnFlyTo = true;
		m_cGpsPoint.clear();
	}
	else if (str == XML_MK_LINESTRING)
	{
		m_bOnLineString = true;
	}
}

void KMLContentHandler::OnEndElement(const XML_Char* name)
{
	std::string str(name);

	if (!m_stkDocument.empty())
	{
		CGpsPointArray* pGpsPointArray = m_stkDocument.top().pGpsPointArray;

		if (str == m_stkDocument.top().m_strTag)
		{
			pGpsPointArray->removeEmpties();
			if (pGpsPointArray->empty())
				delete pGpsPointArray;
			else
				m_vecGpsArray.push_back(pGpsPointArray);
			m_stkDocument.pop();
		}
		else if (m_bOnLineString)
		{
			if (str == XML_MK_LINESTRING)
			{
				m_bOnLineString = false;
			}
			else if (str == XML_MK_COORDS)
			{
				std::unique_ptr<CGpsPointArray> pGpsTrak(new CGpsTrack);
				pGpsTrak->name(m_cGpsPoint.name());

				stdx::string_helper::vector vecStrResult;
				stdx::string_helper::vector::iterator it;

				stdx::string_helper::split(m_strData, vecStrResult, stdx::find_first_of(" \n"), false);
				for (it = vecStrResult.begin(); it != vecStrResult.end(); ++it)
				{
					stdx::string_helper::vector vecStrCoords;
					stdx::string_helper::split(*it, vecStrCoords, stdx::find_first(","));
					if (vecStrCoords.size() == 3)
					{
						CGpsPoint cGpsPoint;

						cGpsPoint.lng(stdx::string_helper::string_to<double>(vecStrCoords[0]));
						cGpsPoint.lat(stdx::string_helper::string_to<double>(vecStrCoords[1]));
						cGpsPoint.alt(stdx::string_helper::string_to<double>(vecStrCoords[2]));

						if (cGpsPoint)
							pGpsTrak->push_back(cGpsPoint);
					}
				}

				if (!pGpsTrak->empty())
					m_vecGpsArray.push_back(pGpsTrak.release());
			}
		}
		else if (m_bOnPlacemark)
		{
			if (str == XML_MK_PLACEMARK)
			{
				pGpsPointArray->push_back(m_cGpsPoint);

				m_cGpsPoint.clear();
				m_bOnPlacemark = false;
			}
			else if (str == XML_MK_NAME)
			{
				m_cGpsPoint.name(m_strData);
			}
			else if (str == XML_MK_SNIPPET)
			{
				m_cGpsPoint.comment(m_strData);
			}
			else if (str == XML_MK_DESCRIPTION && m_cGpsPoint.comment().empty())
			{
				m_cGpsPoint.comment(m_strData);
			}
			else if (str == XML_MK_COORDS)
			{
				stdx::string_helper::vector vecStrResult;
				stdx::string_helper::split(m_strData, vecStrResult, stdx::find_first(","));
				if (vecStrResult.size() >= 2)
				{
					m_cGpsPoint.lng(stdx::string_helper::string_to<double>(vecStrResult[0]));
					m_cGpsPoint.lat(stdx::string_helper::string_to<double>(vecStrResult[1]));
				}

				if (vecStrResult.size() == 3)
					m_cGpsPoint.alt(stdx::string_helper::string_to<double>(vecStrResult[2]));
			}
		}
		else if (m_bOnFlyTo)
		{
			if (str == XML_MK_GXFLYTO)
			{
				pGpsPointArray->push_back(m_cGpsPoint);

				m_cGpsPoint.clear();
				m_bOnFlyTo = false;
			}
			else if (str == XML_MK_NAME)
			{
				m_cGpsPoint.name(m_strData);
			}
			else if (str == XML_MK_LONGITUDE)
			{
				m_cGpsPoint.lng(stdx::string_helper::string_to<double>(m_strData));
			}
			else if (str == XML_MK_LATITUDE)
			{
				m_cGpsPoint.lat(stdx::string_helper::string_to<double>(m_strData));
			}
			else if (str == XML_MK_ALTITUDE)
			{
				m_cGpsPoint.alt(stdx::string_helper::string_to<double>(m_strData));
			}
		}
		else if (str == XML_MK_NAME && pGpsPointArray->name().empty())
		{
			pGpsPointArray->name(m_strData);
		}
	}
}

void KMLContentHandler::OnCharacterData(const XML_Char* data, int len)
{
	m_strData.append(data, len);
}

int ReadKML(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	return KMLContentHandler(vecGpsArray).Parse(strPathName.c_str()) ? S_OK : S_FALSE;
}

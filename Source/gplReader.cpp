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
 * Purpose : Reader for Medion GoPal file (.xml)
 *           www.mediongopal.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "gplReader.h"

#define GPL_MK_TOUR   "tour"
#define GPL_MK_DEST   "dest"

#define GPL_VL_CITY   "City"
#define GPL_VL_LAT    "Latitude"
#define GPL_VL_LON    "Longitude"

 //////////////////////////////////////////////////////////////////////
 // Construction/Destruction
 //////////////////////////////////////////////////////////////////////
GPLContentHandler::GPLContentHandler(CGpsPointArray& cGpsPointArray) :
	m_cGpsPointArray(cGpsPointArray),
	m_bOnTour(false)
{
}

GPLContentHandler::~GPLContentHandler()
{
}

bool GPLContentHandler::Parse(const wchar_t* szFileName)
{
	m_bOnTour = false;

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

void GPLContentHandler::OnStartElement(const XML_Char* name, const XML_Char** attrs)
{
	std::string str(name);

	if (m_bOnTour && !str.compare(GPL_MK_DEST))
	{
		CGpsPoint cGpsPoint;
		geo::CGeoMercatorXY gMercatorXY;

		const XML_Char** it_attrs = attrs;
		while (*it_attrs)
		{
			std::string strName(*it_attrs);
			it_attrs++;
			std::string strValue(*it_attrs);
			it_attrs++;

			if (!strName.compare(GPL_VL_CITY))
				cGpsPoint.name(strValue);
			else if (!strName.compare(GPL_VL_LAT))
				gMercatorXY.y(stdx::string_helper::string_to<int>(strValue));
			else if (!strName.compare(GPL_VL_LON))
				gMercatorXY.x(stdx::string_helper::string_to<int>(strValue));
		}

		if (gMercatorXY)
		{
			cGpsPoint = gMercatorXY;
			if (cGpsPoint)
				m_cGpsPointArray.push_back(cGpsPoint);
		}
	}
	else if (!str.compare(GPL_MK_TOUR))
	{
		m_bOnTour = true;
	}
}

void GPLContentHandler::OnEndElement(const XML_Char* name)
{
	std::string str(name);

	if (!str.compare(GPL_MK_TOUR))
		m_bOnTour = false;
}

void GPLContentHandler::OnCharacterData(const XML_Char*, int)
{
}

int ReadGPL(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	vecGpsArray.push_back(new CGpsRoute());
	return GPLContentHandler(*vecGpsArray.back()).Parse(strPathName.c_str()) ? S_OK : S_FALSE;
}

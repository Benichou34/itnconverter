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
 * Purpose : Reader for XML file (.xml)
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "xmlReader.h"

#define XML_MK_ROUTE    "route"
#define XML_MK_LOCATION "location"

#define XML_VL_NAME   "name"
#define XML_VL_LAT    "lat"
#define XML_VL_LON    "long"

 //////////////////////////////////////////////////////////////////////
 // Construction/Destruction
 //////////////////////////////////////////////////////////////////////

XMLContentHandler::XMLContentHandler(CGpsPointArray& cGpsPointArray) :
	m_cGpsPointArray(cGpsPointArray),
	m_bOnRoute(false)
{
}

XMLContentHandler::~XMLContentHandler()
{
}

bool XMLContentHandler::Parse(const wchar_t* szFileName)
{
	m_bOnRoute = false;

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

void XMLContentHandler::OnStartElement(const XML_Char* name, const XML_Char** attrs)
{
	std::string str(name);

	if (m_bOnRoute && !str.compare(XML_MK_LOCATION))
	{
		CGpsPoint cGpsPoint;

		const XML_Char** it_attrs = attrs;
		while (*it_attrs)
		{
			std::string strName(*it_attrs);
			it_attrs++;
			std::string strValue(*it_attrs);
			it_attrs++;

			if (!strName.compare(XML_VL_NAME))
				cGpsPoint.name(strValue);
			else if (strName == XML_VL_LAT)
				cGpsPoint.lat(stdx::string_helper::string_to<double>(strValue));
			else if (strName == XML_VL_LON)
				cGpsPoint.lng(stdx::string_helper::string_to<double>(strValue));
		}

		if (cGpsPoint)
			m_cGpsPointArray.push_back(cGpsPoint);
	}
	else if (!str.compare(XML_MK_ROUTE))
	{
		m_bOnRoute = true;
	}
}

void XMLContentHandler::OnEndElement(const XML_Char* name)
{
	std::string str(name);

	if (!str.compare(XML_MK_ROUTE))
		m_bOnRoute = false;
}

void XMLContentHandler::OnCharacterData(const XML_Char*, int)
{
}

int ReadXML(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	vecGpsArray.push_back(new CGpsRoute());
	return XMLContentHandler(*vecGpsArray.back()).Parse(strPathName.c_str()) ? S_OK : S_FALSE;
}

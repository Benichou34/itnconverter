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
 * Purpose : Reader for Nokia Landmark Exchange file (.lmx)
 *           www.nokia.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "lmxReader.h"

#define XML_MK_LMX       "lm:lmx"
#define XML_MK_LANDMARK  "lm:landmark"
#define XML_MK_NAME      "lm:name"
#define XML_MK_DESC      "lm:description"
#define XML_MK_COORDS    "lm:coordinates"
#define XML_MK_LAT       "lm:latitude"
#define XML_MK_LNG       "lm:longitude"
#define XML_MK_ALT       "lm:altitude"

LMXContentHandler::LMXContentHandler(CGpsPointArray& cGpsPointArray) :
	m_cGpsPointArray(cGpsPointArray),
	m_bOnLandmark(false)
{
}

LMXContentHandler::~LMXContentHandler()
{
}

bool LMXContentHandler::Parse(const wchar_t* szFileName)
{
	m_bOnLandmark = false;

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

void LMXContentHandler::OnStartElement(const XML_Char* name, const XML_Char**)
{
	std::string str(name);
	m_strData.clear();

	if (!str.compare(XML_MK_LANDMARK))
	{
		m_bOnLandmark = true;
		m_cGpsPoint.clear();
	}
}

void LMXContentHandler::OnEndElement(const XML_Char* name)
{
	std::string str(name);

	if (!str.compare(XML_MK_LANDMARK))
	{
		m_cGpsPointArray.push_back(m_cGpsPoint);
		m_cGpsPoint.clear();

		m_bOnLandmark = false;
	}
	else if (!str.compare(XML_MK_NAME))
	{
		if (m_bOnLandmark)
			m_cGpsPoint.name(m_strData);
		else if (m_cGpsPointArray.name().empty())
			m_cGpsPointArray.name(m_strData);
	}
	else if (!str.compare(XML_MK_DESC) && m_bOnLandmark)
	{
		m_cGpsPoint.comment(m_strData);
	}
	else if (!str.compare(XML_MK_LAT) && m_bOnLandmark)
	{
		m_cGpsPoint.lat(stdx::string_helper::string_to<double>(m_strData));
	}
	else if (!str.compare(XML_MK_LNG) && m_bOnLandmark)
	{
		m_cGpsPoint.lng(stdx::string_helper::string_to<double>(m_strData));
	}
	else if (!str.compare(XML_MK_ALT) && m_bOnLandmark)
	{
		m_cGpsPoint.alt(stdx::string_helper::string_to<double>(m_strData));
	}
	else if (!str.compare(XML_MK_LMX))
	{
	}
}

void LMXContentHandler::OnCharacterData(const XML_Char* data, int len)
{
	m_strData.append(data, len);
}

int ReadLMX(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	vecGpsArray.push_back(new CGpsRoute());
	return LMXContentHandler(*vecGpsArray.back()).Parse(strPathName.c_str()) ? S_OK : S_FALSE;
}


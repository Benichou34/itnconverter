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
 * Purpose : Reader for IGN Rando file (.rdn)
 *           www.ignrando.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "rdnReader.h"

#define XML_MK_RDN         "RANDONNEE"
#define XML_MK_ETAPE       "ETAPE"
#define XML_MK_DESCRIPTION "DESCRIPTION"
#define XML_MK_POSITION    "POSITION"
#define XML_MK_DESC        "DESCRIPTION_ETAPE"
#define XML_MK_ALTITUDE    "ALTITUDE"

 //////////////////////////////////////////////////////////////////////
 // Construction/Destruction
 //////////////////////////////////////////////////////////////////////

RDNContentHandler::RDNContentHandler(CGpsPointArray& cGpsPointArray) :
	m_cGpsPointArray(cGpsPointArray),
	m_bOnEtape(false)
{
}

RDNContentHandler::~RDNContentHandler()
{
}

bool RDNContentHandler::Parse(const wchar_t* szFileName)
{
	m_bOnEtape = false;

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

void RDNContentHandler::OnStartElement(const XML_Char* name, const XML_Char**)
{
	std::string str(name);
	m_strData.clear();

	if (!str.compare(XML_MK_ETAPE))
	{
		m_bOnEtape = true;
		m_cGpsPoint.clear();
	}
}

void RDNContentHandler::OnEndElement(const XML_Char* name)
{
	std::string str(name);

	if (!str.compare(XML_MK_ETAPE))
	{
		m_cGpsPointArray.push_back(m_cGpsPoint);
		m_cGpsPoint.clear();

		m_bOnEtape = false;
	}
	else if (!str.compare(XML_MK_POSITION) && m_bOnEtape)
	{
		stdx::string_helper::vector vecStrResult;
		stdx::string_helper::split(m_strData, vecStrResult, stdx::find_first(","));
		if (vecStrResult.size() > 1)
		{
			m_cGpsPoint.lat(stdx::string_helper::string_to<double>(vecStrResult[0]));
			m_cGpsPoint.lng(stdx::string_helper::string_to<double>(vecStrResult[1]));
		}
	}
	else if (!str.compare(XML_MK_DESC) && m_bOnEtape)
	{
		m_cGpsPoint.name(m_strData);
	}
	else if (!str.compare(XML_MK_ALTITUDE) && m_bOnEtape)
	{
		m_cGpsPoint.alt(stdx::string_helper::string_to<double>(m_strData));
	}
}

void RDNContentHandler::OnCharacterData(const XML_Char* data, int len)
{
	m_strData.append(data, len);
}

int ReadRDN(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	vecGpsArray.push_back(new CGpsRoute());
	return RDNContentHandler(*vecGpsArray.back()).Parse(strPathName.c_str()) ? S_OK : S_FALSE;
}

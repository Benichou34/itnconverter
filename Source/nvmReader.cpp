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
 * Purpose : Writer for Navman XML file (.xml)
 *           www.navman.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "nvmReader.h"

#define NVM_MK_LIST   "MultistopLocations"
#define XVM_MK_ITEM   "Item"
#define XVM_MK_NAME   "name"
#define XVM_MK_LAT    "lat"
#define XVM_MK_LON    "long"
#define XVM_MK_ENAME  "entryName"
#define XVM_ATT_CLASS "class"

#define XVM_VL_LOCLIST "LocationList"
#define XVM_VL_LOC     "Location"

NVMContentHandler::NVMContentHandler(CGpsPointArray& cGpsPointArray) :
	m_cGpsPointArray(cGpsPointArray),
	m_bOnLocationList(false),
	m_bOnLocation(false)
{
}

NVMContentHandler::~NVMContentHandler()
{
}

bool NVMContentHandler::Parse(const wchar_t* szFileName)
{
	m_bOnLocationList = false;
	m_bOnLocation = false;

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

void NVMContentHandler::OnStartElement(const XML_Char* name, const XML_Char** attrs)
{
	std::string str(name);
	m_strData.clear();

	if (!str.compare(NVM_MK_LIST))
	{
		const XML_Char** it_attrs = attrs;
		while (*it_attrs)
		{
			std::string strName(*it_attrs);
			it_attrs++;
			std::string strValue(*it_attrs);
			it_attrs++;

			if (!strName.compare(XVM_ATT_CLASS) && !strValue.compare(XVM_VL_LOCLIST))
			{
				m_bOnLocationList = true;
				break;
			}
		}
	}
	else if (!str.compare(XVM_MK_ITEM) && m_bOnLocationList)
	{
		const XML_Char** it_attrs = attrs;
		while (*it_attrs)
		{
			std::string strName(*it_attrs);
			it_attrs++;
			std::string strValue(*it_attrs);
			it_attrs++;

			if (!strName.compare(XVM_ATT_CLASS) && !strValue.compare(XVM_VL_LOC))
			{
				m_cGpsPoint.clear();
				m_bOnLocation = true;
				break;
			}
		}
	}
}

void NVMContentHandler::OnEndElement(const XML_Char* name)
{
	std::string str(name);

	if (!str.compare(NVM_MK_LIST))
	{
		m_bOnLocationList = false;
	}
	else if (!str.compare(XVM_MK_ITEM))
	{
		if (m_cGpsPoint)
			m_cGpsPointArray.push_back(m_cGpsPoint);
		m_cGpsPoint.clear();

		m_bOnLocation = false;
	}
	else if (!str.compare(XVM_MK_NAME) && m_bOnLocation)
	{
		m_cGpsPoint.name(m_strData);
	}
	else if (!str.compare(XVM_MK_LAT) && m_bOnLocation)
	{
		m_cGpsPoint.lat(stdx::string_helper::string_to<int>(m_strData) / 100000.);
	}
	else if (!str.compare(XVM_MK_LON) && m_bOnLocation)
	{
		m_cGpsPoint.lng(stdx::string_helper::string_to<int>(m_strData) / 100000.);
	}
	else if (!str.compare(XVM_MK_ENAME) && m_bOnLocation)
	{
		if (m_cGpsPoint.name().empty())
			m_cGpsPoint.name(m_strData);
		else
			m_cGpsPoint.comment(m_strData);
	}
}

void NVMContentHandler::OnCharacterData(const XML_Char* data, int len)
{
	m_strData.append(data, len);
}

int ReadNVM(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	vecGpsArray.push_back(new CGpsRoute());
	return NVMContentHandler(*vecGpsArray.back()).Parse(strPathName.c_str()) ? S_OK : S_FALSE;
}

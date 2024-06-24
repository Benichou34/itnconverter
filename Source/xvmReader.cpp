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
 * Purpose : Reader for Via Michelin file (.xvm)
 *           www.viamichelin.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "xvmReader.h"

#define XVM_MK_STEP   "step"
#define XVM_MK_POI    "poi"
#define XVM_MK_DESC   "description"
#define XVM_MK_ITN    "itinerary"
#define XVM_MK_XVM    "poi_list"

#define XVM_VL_NAME   "name"
#define XVM_VL_LAT    "latitude"
#define XVM_VL_LON    "longitude"

 //////////////////////////////////////////////////////////////////////
 // Construction/Destruction
 //////////////////////////////////////////////////////////////////////

XVMContentHandler::XVMContentHandler(std::vector<CGpsPointArray*>& vecGpsArray) :
	m_vecGpsArray(vecGpsArray),
	m_pGpsCurrentArray(nullptr),
	m_pGpsWayPointArray(nullptr)
{
}

XVMContentHandler::~XVMContentHandler()
{
	delete m_pGpsWayPointArray;
	if (m_pGpsCurrentArray != m_pGpsWayPointArray)
		delete m_pGpsCurrentArray;
}

bool XVMContentHandler::Parse(const wchar_t* szFileName)
{
	m_pGpsCurrentArray = nullptr;
	m_pGpsWayPointArray = nullptr;
	m_strCurrentTag.clear();

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

void XVMContentHandler::OnStartElement(const XML_Char* name, const XML_Char** attrs)
{
	std::string str(name);
	m_strData.clear();

	if (str == XVM_MK_POI && !m_pGpsCurrentArray)
	{
		if (!m_pGpsWayPointArray)
			m_pGpsWayPointArray = new CGpsPoiArray();

		m_pGpsCurrentArray = m_pGpsWayPointArray;
		if (m_pGpsCurrentArray->getType() == CGpsPointArray::E_ARRAY_POI && m_strCurrentTag.empty())
			m_strCurrentTag = XVM_MK_POI;
	}

	if (str == m_strCurrentTag)
	{
		m_cGpsPoint.clear();

		const XML_Char** it_attrs = attrs;
		while (*it_attrs)
		{
			std::string strName(*it_attrs);
			it_attrs++;
			std::string strValue(*it_attrs);
			it_attrs++;

			if (strName == XVM_VL_NAME)
				m_cGpsPoint.name(strValue);
			else if (strName == XVM_VL_LAT)
				m_cGpsPoint.lat(stdx::string_helper::string_to<double>(strValue));
			else if (strName == XVM_VL_LON)
				m_cGpsPoint.lng(stdx::string_helper::string_to<double>(strValue));
		}
	}
	else if (str == XVM_MK_ITN)
	{
		if (m_pGpsCurrentArray != m_pGpsWayPointArray)
			delete m_pGpsCurrentArray;

		m_pGpsCurrentArray = new CGpsRoute();

		const XML_Char** it_attrs = attrs;
		while (*it_attrs)
		{
			std::string strName(*it_attrs);
			it_attrs++;
			std::string strValue(*it_attrs);
			it_attrs++;

			if (strName == XVM_VL_NAME)
				m_pGpsCurrentArray->name(strValue);
		}

		if (m_pGpsCurrentArray->getType() == CGpsPointArray::E_ARRAY_ROUTE)
			m_strCurrentTag = XVM_MK_STEP;
	}
}

void XVMContentHandler::OnEndElement(const XML_Char* name)
{
	std::string str(name);

	if (str == m_strCurrentTag)
	{
		if (m_pGpsCurrentArray)
			m_pGpsCurrentArray->push_back(m_cGpsPoint);

		m_cGpsPoint.clear();
	}
	else if (str == XVM_MK_DESC)
	{
		m_cGpsPoint.comment(m_strData);
	}
	else if (str == XVM_MK_ITN)
	{
		if (m_pGpsCurrentArray && !m_pGpsCurrentArray->empty())
			m_vecGpsArray.push_back(m_pGpsCurrentArray);

		m_pGpsCurrentArray = nullptr;
		m_strCurrentTag.clear();
	}
	else if (str == XVM_MK_XVM)
	{
		if (m_pGpsWayPointArray && !m_pGpsWayPointArray->empty())
			m_vecGpsArray.push_back(m_pGpsWayPointArray);

		if (m_pGpsCurrentArray == m_pGpsWayPointArray)
			m_pGpsCurrentArray = nullptr;

		m_pGpsWayPointArray = nullptr;
		m_strCurrentTag.clear();
	}
}

void XVMContentHandler::OnCharacterData(const XML_Char* data, int len)
{
	m_strData.append(data, len);
}

int ReadXVM(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	return XVMContentHandler(vecGpsArray).Parse(strPathName.c_str()) ? S_OK : S_FALSE;
}

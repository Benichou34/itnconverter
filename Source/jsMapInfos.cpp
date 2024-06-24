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
 */

#include "stdafx.h"
#include "jsMapInfos.h"
#include "ToolsLibrary/Internet.h"
#include "jsonParser/JsonParser.h"

jsMapStatus::jsMapStatus() :
	m_bAvailable(false),
	m_bSelected(true)
{
}

bool jsMapStatus::isAvailable() const
{
	return m_bAvailable;
}

bool jsMapStatus::isSelected() const
{
	return m_bAvailable && m_bSelected;
}

void jsMapStatus::setSelected(bool bSelected)
{
	m_bSelected = bSelected;
}

void jsMapStatus::setAvailable(bool bAvailable)
{
	m_bAvailable = bAvailable;
}

bool jsMapStatus::operator== (const jsMapStatus& mapStatus) const
{
	return m_bAvailable == mapStatus.m_bAvailable && m_bSelected == mapStatus.m_bSelected;
}

bool jsMapStatus::operator!= (const jsMapStatus& mapStatus) const
{
	return !operator== (mapStatus);
}

jsMapInfos::jsMapInfos(const std::string& strFlag, const std::string& strName, size_t icon) :
	m_strFlag(strFlag),
	m_strName(strName),
	m_icon(icon),
	m_Maps(E_MAPS_NB)
{
}

void jsMapInfos::setFlag(const std::string& strFlag)
{
	m_strFlag = strFlag;
}

void jsMapInfos::setName(const std::string& strName)
{
	m_strName = strName;
}

void jsMapInfos::setIcon(size_t icon)
{
	m_icon = icon;
}

jsMapStatus& jsMapInfos::operator[](E_MAP_TYPE mapType)
{
	return m_Maps[mapType];
}

const jsMapStatus& jsMapInfos::operator[](E_MAP_TYPE mapType) const
{
	return m_Maps[mapType];
}

bool jsMapInfos::operator== (const jsMapInfos& mapInfos) const
{
	return m_strFlag == mapInfos.m_strFlag
		&& m_strName == mapInfos.m_strName
		&& m_icon == mapInfos.m_icon
		&& m_Maps == mapInfos.m_Maps;
}

bool jsMapInfos::operator!= (const jsMapInfos& mapInfos) const
{
	return !operator== (mapInfos);
}

bool jsMapsInfos::fromJs(const std::string& strContent)
{
	CJsonParser jsParser;
	if (!strContent.empty() && jsParser.parse(strContent) == CJsonParser::JSON_SUCCESS)
	{
		// Lecture du fichier de config des cartes
		const CJsonObject& jsMaps = jsParser;
		for (size_t i = 0; i < jsMaps.size(); ++i)
		{
			const CJsonValue& jsMap = jsMaps[i];
			const std::string& strFlag = jsMap.getName();
			jsMapInfos mapInfos(strFlag, jsMap("name"));

			if (jsMap.exist("icon"))
				mapInfos.setIcon(jsMap("icon"));

			if (jsMap.exist("map") && jsMap("map"))
				mapInfos[jsMapInfos::E_MAP].setAvailable();
			if (jsMap.exist("sat") && jsMap("sat"))
				mapInfos[jsMapInfos::E_SAT].setAvailable();
			if (jsMap.exist("hybrid") && jsMap("hybrid"))
				mapInfos[jsMapInfos::E_HYBRID].setAvailable();
			if (jsMap.exist("topo") && jsMap("topo"))
				mapInfos[jsMapInfos::E_TOPO].setAvailable();

			insert(std::make_pair(strFlag, mapInfos));
		}
	}

	return true;
}

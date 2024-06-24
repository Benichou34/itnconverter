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

#ifndef _JSMAPINFOS_H_INCLUDED_
#define _JSMAPINFOS_H_INCLUDED_

#include <string>
#include <vector>
#include <map>

class jsMapStatus
{
public:
	jsMapStatus();

	bool isAvailable() const;
	bool isSelected() const;
	void setSelected(bool bSelected);

	bool operator== (const jsMapStatus& mapStatus) const;
	bool operator!= (const jsMapStatus& mapStatus) const;

protected:
	friend class jsMapsInfos;
	void setAvailable(bool bAvailable = true);

private:
	bool m_bAvailable;
	bool m_bSelected;
};

class jsMapInfos
{
public:
	typedef enum { E_MAP, E_SAT, E_HYBRID, E_TOPO, E_MAPS_NB } E_MAP_TYPE;

	jsMapInfos(const std::string& strFlag = std::string(), const std::string& strName = std::string(), size_t icon = 0);

	const std::string flag() const { return m_strFlag; }
	const std::string name() const { return m_strName; }
	size_t icon() const { return m_icon; }

	jsMapStatus& operator[](E_MAP_TYPE mapType);
	const jsMapStatus& operator[](E_MAP_TYPE mapType) const;

	bool operator== (const jsMapInfos& mapInfos) const;
	bool operator!= (const jsMapInfos& mapInfos) const;

protected:
	friend class jsMapsInfos;

	void setFlag(const std::string& strFlag);
	void setName(const std::string& strName);
	void setIcon(size_t icon);

private:
	std::string m_strFlag;
	std::string m_strName;
	size_t m_icon;
	std::vector<jsMapStatus> m_Maps;
};

class jsMapsInfos : public std::map<std::string, jsMapInfos>
{
public:
	bool fromJs(const std::string& strContent);
};

#endif // _JSMAPINFOS_H_INCLUDED_

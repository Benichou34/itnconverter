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

#include "SAXWriter.h"

namespace
{
	void escape_content(std::ostream& os, const std::string& tag_content)
	{
		for (std::string::const_iterator cit = tag_content.begin(); cit != tag_content.end(); ++cit)
		{
			switch (*cit)
			{
			case '&':
				os << "&amp;";
				break;

			case '<':
				os << "&lt;";
				break;

			case '>':
				os << "&gt;";
				break;

			case '\'':
				os << "&apos;";
				break;

			case '"':
				os << "&quot;";
				break;

			default:
				os.put(*cit);
				break;
			}
		}
	}
}

SAXWriter::Tag::Tag(std::ostream& os, bool& bEmpty, const std::string& name) :
	m_os(os),
	m_bEmpty(bEmpty),
	m_name(name)
{
	if (m_bEmpty)
		m_os << '>'; // Close element
	else
		m_bEmpty = true;

	m_os << '<' << name; // Open element
}

SAXWriter::Tag::Tag(Tag&& tag) :
	m_os(tag.m_os),
	m_bEmpty(tag.m_bEmpty),
	m_name(std::move(tag.m_name))
{
}

SAXWriter::Tag::~Tag()
{
	if (m_name.empty())
		return;

	if (m_bEmpty)
		m_os << "/>"; // Empty element
	else
		m_os << "</" << m_name << '>';

	m_bEmpty = false;
}

SAXWriter::Tag& SAXWriter::Tag::operator=(Tag&& tag)
{
	m_name.clear();
	std::swap(m_name, tag.m_name);
	return *this;
}

SAXWriter::Tag& SAXWriter::Tag::attribute(const std::string& att_name, const std::string& att_value)
{
	if (!m_bEmpty)
		throw std::invalid_argument("Not empty");

	m_os << ' ' << att_name << "=\"";
	escape_content(m_os, att_value);
	m_os << "\"";

	return *this;
}

SAXWriter::Tag& SAXWriter::Tag::attributes(const std::map<std::string, std::string>& tag_attributes)
{
	std::map<std::string, std::string>::const_iterator cit;
	for (cit = tag_attributes.begin(); cit != tag_attributes.end(); ++cit)
		attribute(cit->first, cit->second);

	return *this;
}

SAXWriter::Tag& SAXWriter::Tag::content(const std::string& tag_content)
{
	if (m_bEmpty)
	{
		m_os << '>'; // Close element
		m_bEmpty = false;
	}

	escape_content(m_os, tag_content);
	return *this;
}

SAXWriter::Tag& SAXWriter::Tag::cdata(const std::string& cdata_content)
{
	if (!m_bEmpty)
		throw std::invalid_argument("Not empty");

	if (cdata_content.empty())
		return *this;

	m_bEmpty = false;
	m_os << "><![CDATA[" << cdata_content << "]]>";
	return *this;
}

SAXWriter::Tag SAXWriter::Tag::tag(const std::string& name)
{
	return Tag(m_os, m_bEmpty, name);
}

SAXWriter::SAXWriter(std::ostream& os) :
	m_os(os),
	m_bEmpty(false)
{
}

SAXWriter::Tag SAXWriter::tag(const std::string& name)
{
	return Tag(m_os, m_bEmpty, name);
}

SAXWriter& SAXWriter::doctype(const std::string& doctype)
{
	m_os << "<!DOCTYPE " << doctype << ">";
	return *this;
}

SAXWriter& SAXWriter::declaration(const std::string& encoding, const std::string& version)
{
	m_os << "<?xml version=\"" << version << "\" encoding=\"" << encoding << "\" standalone=\"no\"?>";
	return *this;
}

SAXWriter& SAXWriter::comment(const std::string& comment)
{
	m_os << "<!--" << comment << "-->";
	return *this;
}

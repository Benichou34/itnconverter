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

#ifndef XML_SAX_WRITER_H_INCLUDED
#define XML_SAX_WRITER_H_INCLUDED

#include <ostream>
#include <string>
#include <map>

class SAXWriter
{
public:
	class Tag
	{
	public:
		Tag(Tag&& tag);
		~Tag();

		Tag& operator=(Tag&& tag);

		Tag& attribute(const std::string& att_name, const std::string& att_value);
		Tag& attributes(const std::map<std::string, std::string>& tag_attributes);
		Tag& content(const std::string& tag_content);
		Tag& cdata(const std::string& cdata_content); // <![CDATA[ and ]]>
		Tag tag(const std::string& name);

		Tag(const Tag& tag) = delete;
		Tag& operator=(const Tag&) = delete;

	private:
		friend class SAXWriter;
		Tag(std::ostream& os, bool& bEmpty, const std::string& name);

	private:
		std::ostream& m_os;
		bool& m_bEmpty;
		std::string m_name;
	};

	SAXWriter(std::ostream& os);

	SAXWriter& doctype(const std::string& doctype);
	SAXWriter& declaration(const std::string& encoding = std::string("UTF-8"), const std::string& version = std::string("1.0")); // <?xml version = "1.0" encoding = "UTF-8"?>
	SAXWriter& comment(const std::string& comment); // <!-- no need to escape <code> & such in comments -->

	Tag tag(const std::string& name);

private:
	SAXWriter(const SAXWriter&) = delete;
	SAXWriter& operator=(const SAXWriter&) = delete;

private:
	std::ostream& m_os;
	bool m_bEmpty;
};

#endif // XML_SAX_WRITER_H_INCLUDED

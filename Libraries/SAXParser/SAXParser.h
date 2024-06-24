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

#ifndef XML_SAX_PARSER_H_INCLUDED
#define XML_SAX_PARSER_H_INCLUDED

#include <map>
#include <string>
#include "expat/expat.h"

 // Base class for event handlers.
class SAXEvtHandler
{
public:
	virtual ~SAXEvtHandler() = default;

	virtual void OnStartElement(const XML_Char* /*name*/, const XML_Char** /*attrs*/) { m_strData.clear(); }
	virtual void OnEndElement(const XML_Char* /*name*/) = 0;
	virtual void OnCharacterData(const XML_Char* data, int len) { m_strData.append(data, static_cast<size_t>(len)); }
	virtual void OnProcessingInstruction(const XML_Char* /*target*/, const XML_Char* /*data*/) { }
	virtual void OnComment(const XML_Char* /*data*/) { }
	virtual void OnStartCdataSection() { }
	virtual void OnEndCdataSection() { }
	virtual void OnDefault(const XML_Char* /*data*/, int /*len*/) { }
	virtual bool OnUnknownEncoding(const XML_Char* /*name*/, XML_Encoding* pInfo) { if (pInfo) for (int i = 0; i < 256; i++) pInfo->map[i] = i; return true; }
	virtual void OnStartNamespaceDecl(const XML_Char* /*prefix*/, const XML_Char* /*uri*/) { }
	virtual void OnEndNamespaceDecl(const XML_Char* /*prefix*/) { }
	virtual void OnXmlDecl(const XML_Char* /*version*/, const XML_Char* /*encoding*/, bool /*isStandalone*/) { }
	virtual void OnStartDoctypeDecl(const XML_Char* /*doctype*/, const XML_Char* /*sysId*/, const XML_Char* /*pubId*/, bool /*hasInternalSubset*/) { }
	virtual void OnEndDoctypeDecl() { }

protected:
	typedef std::map<std::string, std::string> Attributes;
	void getAttributes(const XML_Char** attrs, Attributes& mapAttributes);

	const std::string& getCharacterData() const noexcept { return m_strData; }

private:
	std::string m_strData;
};

class SAXParser
{
public:
	SAXParser(SAXEvtHandler* pHandler, const XML_Char* encoding = nullptr);
	virtual ~SAXParser();

	// Reset the parser before next file.
	void Reset(const XML_Char* encoding = nullptr);

	// Parse the data pointed to by buf.
	void Parse(const char* buf, size_t len, bool isFinal = true);
	void Parse(const char* buf, bool isFinal = true);
	void Parse(const std::string& buf, bool isFinal = true);

	// Parse internal buffer.
	void Parse(size_t len, bool isFinal = true);

	// Get the internal buffer.
	void* GetBuffer(size_t len);

	// Parser error reporting methods
	enum XML_Error GetErrorCode() const;

	long GetCurrentByteIndex() const;
	size_t GetCurrentLineNumber() const;
	size_t GetCurrentColumnNumber() const;
	int GetCurrentByteCount() const;

	const char* GetInputContext(int* pOffset, int* pSize) const;
	const XML_LChar* GetErrorString() const;

	// Parser Interface
	static const XML_LChar* GetExpatVersion();
	static const XML_LChar* GetErrorString(enum XML_Error err);

private:
	// Create a new parser.
	void Create(const XML_Char* encoding = nullptr);

	// Destroy the current parser, if any.
	void Destroy();

private:
	XML_Parser m_parser;
	SAXEvtHandler* m_pHandler;
	bool m_bReset;
};

class CSAXParser : protected SAXEvtHandler, public SAXParser
{
public:
	CSAXParser(const XML_Char* encoding = "UTF-8") : SAXParser(this, encoding) {}
	~CSAXParser() override = default;

	friend std::istream& operator >> (std::istream& iss, CSAXParser& saxParser);
};

#endif // XML_SAX_PARSER_H_INCLUDED

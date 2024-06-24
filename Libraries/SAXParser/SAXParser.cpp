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

#include <vector>
#include <sstream>
#include <cstring>
#include "SAXParser.h"

 /* Internal Parser Callbacks */

extern "C" {

	static void StartElementHandler(void* pUserData, const XML_Char* name, const XML_Char** attrs)
	{
		static_cast<SAXEvtHandler*>(pUserData)->OnStartElement(name, attrs);
	}

	static void EndElementHandler(void* pUserData, const XML_Char* name)
	{
		static_cast<SAXEvtHandler*>(pUserData)->OnEndElement(name);
	}

	static void CharacterDataHandler(void* pUserData, const XML_Char* data, int len)
	{
		static_cast<SAXEvtHandler*>(pUserData)->OnCharacterData(data, len);
	}

	static void ProcessingInstructionHandler(void* pUserData, const XML_Char* target, const XML_Char* data)
	{
		static_cast<SAXEvtHandler*>(pUserData)->OnProcessingInstruction(target, data);
	}

	static void CommentHandler(void* pUserData, const XML_Char* data)
	{
		static_cast<SAXEvtHandler*>(pUserData)->OnComment(data);
	}

	static void StartCdataSectionHandler(void* pUserData)
	{
		static_cast<SAXEvtHandler*>(pUserData)->OnStartCdataSection();
	}

	static void EndCdataSectionHandler(void* pUserData)
	{
		static_cast<SAXEvtHandler*>(pUserData)->OnEndCdataSection();
	}

	static void DefaultHandler(void* pUserData, const XML_Char* data, int len)
	{
		static_cast<SAXEvtHandler*>(pUserData)->OnDefault(data, len);
	}

	static int UnknownEncodingHandler(void* pUserData, const XML_Char* name, XML_Encoding* pInfo)
	{
		return static_cast<SAXEvtHandler*>(pUserData)->OnUnknownEncoding(name, pInfo) ? 1 : 0;
	}

	static void StartNamespaceDeclHandler(void* pUserData, const XML_Char* prefix, const XML_Char* uri)
	{
		static_cast<SAXEvtHandler*>(pUserData)->OnStartNamespaceDecl(prefix, uri);
	}

	static void EndNamespaceDeclHandler(void* pUserData, const XML_Char* prefix)
	{
		static_cast<SAXEvtHandler*>(pUserData)->OnEndNamespaceDecl(prefix);
	}

	static void XmlDeclHandler(void* pUserData, const XML_Char* version, const XML_Char* encoding, int isStandalone)
	{
		static_cast<SAXEvtHandler*>(pUserData)->OnXmlDecl(version, encoding, isStandalone != 0);
	}

	static void StartDoctypeDeclHandler(void* pUserData, const XML_Char* doctype, const XML_Char* sysId, const XML_Char* pubId, int hasInternalSubset)
	{
		static_cast<SAXEvtHandler*>(pUserData)->OnStartDoctypeDecl(doctype, sysId, pubId, hasInternalSubset != 0);
	}

	static void EndDoctypeDeclHandler(void* pUserData)
	{
		static_cast<SAXEvtHandler*>(pUserData)->OnEndDoctypeDecl();
	}

}

void SAXEvtHandler::getAttributes(const XML_Char** attrs, Attributes& mapAttributes)
{
	const XML_Char** it_attrs = attrs;
	while (*it_attrs)
	{
		std::string strName(*it_attrs);
		it_attrs++;
		std::string strValue(*it_attrs);
		it_attrs++;

		mapAttributes[strName] = strValue;
	}
}

/* Parser Wrapper Implementation */
SAXParser::SAXParser(SAXEvtHandler* pHandler, const XML_Char* encoding) :
	m_parser(nullptr),
	m_pHandler(pHandler),
	m_bReset(false)
{
	Create(encoding);
}

SAXParser::~SAXParser()
{
	Destroy();
}

void SAXParser::Create(const XML_Char* encoding)
{
	// Destroy the old parser.
	Destroy();

	// If the encoding or seperator are empty, then nullptr.
	if (encoding && !(*encoding))
		encoding = nullptr;

	// Create the new one, if possible, and initialise.
	m_parser = XML_ParserCreate(encoding);
	if (!m_parser)
		throw XML_ERROR_NONE;
}

void SAXParser::Destroy()
{
	if (m_parser)
	{
		XML_ParserFree(m_parser);
		m_parser = nullptr;
	}
}

void SAXParser::Reset(const XML_Char* encoding)
{
	if (!m_pHandler)
		throw XML_ERROR_NONE;

	// If the encoding or seperator are empty, then nullptr.
	if (encoding && !(*encoding))
		encoding = nullptr;

	// Set the user data used in callbacks.
	XML_ParserReset(m_parser, encoding);
	XML_SetUserData(m_parser, static_cast<void*>(m_pHandler));

	// Set callbacks.
	XML_SetStartElementHandler(m_parser, StartElementHandler);
	XML_SetEndElementHandler(m_parser, EndElementHandler);
	XML_SetCharacterDataHandler(m_parser, CharacterDataHandler);

	XML_SetProcessingInstructionHandler(m_parser, ProcessingInstructionHandler);
	XML_SetCommentHandler(m_parser, CommentHandler);
	XML_SetStartCdataSectionHandler(m_parser, StartCdataSectionHandler);
	XML_SetEndCdataSectionHandler(m_parser, EndCdataSectionHandler);
	XML_SetUnknownEncodingHandler(m_parser, UnknownEncodingHandler, m_pHandler);
	XML_SetStartNamespaceDeclHandler(m_parser, StartNamespaceDeclHandler);
	XML_SetEndNamespaceDeclHandler(m_parser, EndNamespaceDeclHandler);
	XML_SetXmlDeclHandler(m_parser, XmlDeclHandler);
	XML_SetStartDoctypeDeclHandler(m_parser, StartDoctypeDeclHandler);
	XML_SetEndDoctypeDeclHandler(m_parser, EndDoctypeDeclHandler);
	XML_SetDefaultHandler(m_parser, DefaultHandler);

	m_bReset = true;
}

void SAXParser::Parse(const char* buf, size_t len, bool isFinal)
{
	if (!m_parser)
		throw XML_ERROR_NONE;

	if (!m_bReset)
		Reset();

	if (XML_Parse(m_parser, buf, static_cast<int>(len), isFinal) == XML_STATUS_ERROR)
		throw GetErrorCode();

	m_bReset = false;
}

void SAXParser::Parse(const char* buf, bool isFinal)
{
	Parse(buf, strlen(buf), isFinal);
}

void SAXParser::Parse(size_t len, bool isFinal)
{
	if (!m_parser)
		throw XML_ERROR_NONE;

	if (!m_bReset)
		Reset();

	if (XML_ParseBuffer(m_parser, static_cast<int>(len), isFinal) == XML_STATUS_ERROR)
		throw GetErrorCode();

	m_bReset = false;
}

void SAXParser::Parse(const std::string& buf, bool isFinal)
{
	Parse(buf.data(), buf.size(), isFinal);
}

void* SAXParser::GetBuffer(size_t len)
{
	if (!m_parser)
		throw XML_ERROR_NONE;

	return XML_GetBuffer(m_parser, static_cast<int>(len));
}

/* Parser error reporting methods */

enum XML_Error SAXParser::GetErrorCode() const
{
	if (!m_parser)
		throw XML_ERROR_NONE;

	return XML_GetErrorCode(m_parser);
}

long SAXParser::GetCurrentByteIndex() const
{
	if (!m_parser)
		throw XML_ERROR_NONE;

	return XML_GetCurrentByteIndex(m_parser);
}

size_t SAXParser::GetCurrentLineNumber() const
{
	if (!m_parser)
		throw XML_ERROR_NONE;

	return XML_GetCurrentLineNumber(m_parser);
}

size_t SAXParser::GetCurrentColumnNumber() const
{
	if (!m_parser)
		throw XML_ERROR_NONE;

	return XML_GetCurrentColumnNumber(m_parser);
}

int SAXParser::GetCurrentByteCount() const
{
	if (!m_parser)
		throw XML_ERROR_NONE;

	return XML_GetCurrentByteCount(m_parser);
}

const char* SAXParser::GetInputContext(int* pOffset, int* pSize) const
{
	if (!m_parser)
		throw XML_ERROR_NONE;

	return XML_GetInputContext(m_parser, pOffset, pSize);
}

const XML_LChar* SAXParser::GetErrorString() const
{
	return XML_ErrorString(GetErrorCode());
}

/* Parser Interface */

const XML_LChar* SAXParser::GetExpatVersion()
{
	return XML_ExpatVersion();
}

const XML_LChar* SAXParser::GetErrorString(enum XML_Error err)
{
	return XML_ErrorString(err);
}

std::istream& operator >> (std::istream& iss, CSAXParser& saxParser)
{
	std::stringstream ssBuffer;

	ssBuffer << iss.rdbuf();
	std::string strBuffer(ssBuffer.str());

	while (strBuffer.begin() != strBuffer.end() && *(strBuffer.begin()) != '<')
		strBuffer.erase(strBuffer.begin());

	saxParser.Parse(strBuffer.data(), strBuffer.size(), true);

	return iss;
}

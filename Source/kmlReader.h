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
 * Purpose : Reader for Google Earth file (.kml)
 *           https://developers.google.com/kml/documentation/kmlreference
 */

#if !defined(AFX_KMLREADER_H_INCLUDED_)
#define AFX_KMLREADER_H_INCLUDED_

#include <stack>
#include "SAXParser/SAXParser.h"
#include "GpsPointArray.h"

class KMLContentHandler : private CSAXParser
{
private:
	typedef struct
	{
		std::string m_strTag;
		CGpsPointArray* pGpsPointArray;
	} sttGpsPointArray;

	std::vector<CGpsPointArray*>& m_vecGpsArray;
	std::stack<sttGpsPointArray> m_stkDocument;
	CGpsPoint m_cGpsPoint;
	std::string m_strData;
	bool m_bOnPlacemark;
	bool m_bOnFlyTo;
	bool m_bOnLineString;

private:
	virtual void OnStartElement(const XML_Char* name, const XML_Char** attrs);
	virtual void OnEndElement(const XML_Char* name);
	virtual void OnCharacterData(const XML_Char* data, int len);

public:
	KMLContentHandler(std::vector<CGpsPointArray*>& vecGpsArray);
	virtual ~KMLContentHandler();

	bool Parse(const wchar_t* szFileName);
};

#endif // !defined(AFX_KMLREADER_H_INCLUDED_)

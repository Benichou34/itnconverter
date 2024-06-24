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

#include "Internet.h"
#include <sstream>
#include <fstream>
#include "HttpClient.h"
#include "windows.h"
#include "wininet.h"
#include "stdx/uri_helper.h"
#include "stdx/string_helper.h"
#if defined(_DEBUG) || defined(FORCE_LOG)
#include <iostream>
#endif

CInternetHttpSession::CInternetHttpSession() :
	m_pInternetConnection(std::make_unique<CInternetConnection>()),
	m_pHttpSession(new CHttpSession(*m_pInternetConnection)),
	m_pHttpRequest(new CHttpRequest(*m_pHttpSession))
{

}

CInternetHttpSession::CInternetHttpSession(CInternetHttpSession&& httpSession)
{
	std::swap(m_EndCallback, httpSession.m_EndCallback);
	std::swap(m_pHttpRequest, httpSession.m_pHttpRequest);
	std::swap(m_pHttpSession, httpSession.m_pHttpSession);
	std::swap(m_pInternetConnection, httpSession.m_pInternetConnection);
}

CInternetHttpSession::~CInternetHttpSession()
{
	try {
		m_pHttpRequest->close();
		m_pHttpSession->close();
	}
	catch (...) {}
}

CInternetHttpSession& CInternetHttpSession::operator= (CInternetHttpSession&& httpSession)
{
	if (&httpSession != this)
	{
		m_pHttpRequest->close();
		m_pHttpSession->close();

		std::swap(m_EndCallback, httpSession.m_EndCallback);
		std::swap(m_pHttpRequest, httpSession.m_pHttpRequest);
		std::swap(m_pHttpSession, httpSession.m_pHttpSession);
		std::swap(m_pInternetConnection, httpSession.m_pInternetConnection);
	}

	return *this;
}

void CInternetHttpSession::send(std::ostream& oss,
	const std::string& strUrl,
	const std::string& strPostData,
	const std::string& strReferrer,
	const std::string& strHost)
{
	std::string strUrlHost;
	unsigned short port;
	std::string strRequest;
	bool bSecure;

#if defined(_DEBUG) || defined(FORCE_LOG)
	std::clog << strUrl << std::endl;
	if (!strReferrer.empty())
		std::clog << "Referrer: " << strReferrer << std::endl;
	if (!strHost.empty())
		std::clog << "Host: " << strHost << std::endl;
	if (!strPostData.empty())
		std::clog << "Data: " << strPostData << std::endl;
#endif //_DEBUG

	stdx::url_helper::split(strUrl, strUrlHost, port, strRequest, bSecure);

	// Close previous session
	m_pHttpRequest->close();
	m_pHttpSession->close();

	// Open new session
	m_pHttpSession->open(strUrlHost, port);
	m_pHttpRequest->open(strRequest, bSecure, strReferrer, strPostData.empty() ? CHttpRequest::MethodGet : CHttpRequest::MethodPost);

	m_pHttpRequest->addHeader("Host", strHost.empty() ? strUrlHost : strHost);

	m_pHttpRequest->send(oss, strPostData);
}

void CInternetHttpSession::cancel()
{
	m_pHttpRequest->cancel();
}

bool CInternetHttpSession::wait(size_t msTimeOut)
{
	return m_pHttpRequest->wait(msTimeOut);
}

size_t CInternetHttpSession::getNumberOfBytesRead() const
{
	return m_pHttpRequest->getNumberOfBytesRead();
}

void CInternetHttpSession::setEndCallback(const CallbackFunction& EndCallback)
{
	m_EndCallback = EndCallback;
	m_pHttpRequest->setEndCallback([&](CHttpRequest&, const CInternetException& inetException)
		{
			if (m_EndCallback)
				m_EndCallback(*this, inetException);
		});
}

size_t CInternet::HttpToStream(std::ostream& oss, const std::string& strRequest, const std::string& strPostData, const std::string& strReferrer, const std::string& strHost)
{
	CInternetHttpSession httpSession;
	httpSession.send(oss, stdx::uri_helper::encode(strRequest), strPostData, strReferrer, strHost);
	httpSession.wait();
	return httpSession.getNumberOfBytesRead();
}

std::string CInternet::AjaxHttpRequest(const std::string& strRequest, const std::string& strReferrer, const std::string& strHost)
{
	std::ostringstream ossResponse;
	HttpToStream(ossResponse, strRequest, std::string(), strReferrer, strHost);

#if defined(_DEBUG) || defined(FORCE_LOG)
	std::clog << ossResponse.str() << std::endl;
#endif //_DEBUG

	return ossResponse.str();
}

std::wstring CInternet::HttpDownload(
	const std::string& strRequest,
	const std::string& strReferrer,
	const std::wstring& strTargetFile)
{
#if defined(_DEBUG) || defined(FORCE_LOG)
	if (!strTargetFile.empty())
		std::wclog << L"HttpDownload, Target: " << strTargetFile << std::endl;
#endif //_DEBUG

	std::wstring strFile(strTargetFile);
	std::wstring wRequest(stdx::wstring_helper::from_utf8(strRequest));

	if (strFile.empty()) // Empty file name, create a file name to the cache
	{
		// Retrieve the cache entry
		DWORD dwBufferSize = 10 * 1024; // 10K
		std::vector<char> buffer(dwBufferSize);

		LPINTERNET_CACHE_ENTRY_INFOW lpCacheEntry = reinterpret_cast<LPINTERNET_CACHE_ENTRY_INFOW>(buffer.data());
		lpCacheEntry->dwStructSize = static_cast<DWORD>(buffer.size());

		if (GetUrlCacheEntryInfoW(wRequest.c_str(), lpCacheEntry, &dwBufferSize) == TRUE)
		{
			strFile = lpCacheEntry->lpszLocalFileName;
			return strFile;
		}

		// Create the cache entry
		wchar_t szTempPath[MAX_PATH + 1];
		if (CreateUrlCacheEntryW(wRequest.c_str(), 0, L"itc", szTempPath, 0) == FALSE)
			throw GetLastError();

		strFile = szTempPath;
	}

	std::ofstream ofsFile(strFile.c_str(), std::ofstream::binary);

	CInternetHttpSession httpSession;
	httpSession.send(ofsFile, strRequest, std::string(), strReferrer);
	httpSession.wait();
	size_t length = httpSession.getNumberOfBytesRead();

	if (length && strFile != strTargetFile) // Commit the cache entry
	{
		FILETIME ftExpireTime = { 0, 0 };
		FILETIME ftModifiedTime = { 0, 0 };

		// The HTTP/1.0 200 OK\r\n\r\n should be used for the header or else IE will not read it
		if (CommitUrlCacheEntryW(wRequest.c_str(), strFile.c_str(), ftExpireTime, ftModifiedTime, NORMAL_CACHE_ENTRY, L"HTTP/1.0 200 OK\r\n\r\n", 20, nullptr, 0) == FALSE)
			throw GetLastError();
	}

	if (!length)
		strFile.clear();

	return strFile;
}

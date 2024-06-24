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

#include "windows.h"
#include "wininet.h"
#include "HttpClient.h"
#include "Internet.h"
#include <ostream>

#ifdef _MSC_VER
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "ws2_32.lib")
#endif // _MSC_VER

namespace
{
	constexpr char* HttpUserAgent = "Mozilla/5.0 (compatible; MSIE 9.0; Win32)";
	constexpr size_t SizeOfTempBuffer = 1024 * 1024; // 1Mo

	class CScopedCriticalSection
	{
	public:
		CScopedCriticalSection(CRITICAL_SECTION& CriticalSection) : m_lpCriticalSection(&CriticalSection)
		{
			EnterCriticalSection(m_lpCriticalSection);
		}

		~CScopedCriticalSection()
		{
			LeaveCriticalSection(m_lpCriticalSection);
		}

	private:
		LPCRITICAL_SECTION m_lpCriticalSection;
	};
}

const unsigned short CHttpSession::DefaultHttpPort = INTERNET_DEFAULT_HTTP_PORT;
const unsigned short CHttpSession::DefaultHttpsPort = INTERNET_DEFAULT_HTTPS_PORT;

const std::string CHttpRequest::MethodPost("POST");
const std::string CHttpRequest::MethodGet("GET");

/************************************************************************/
/* CInternetConnection                                                  */
/************************************************************************/

CInternetConnection::CInternetConnection(const std::string& strAgent) :
	m_hInternet(nullptr)
{
	open(strAgent);
}

CInternetConnection::CInternetConnection(CInternetConnection&& inetConnection) :
	m_hInternet(nullptr)
{
	std::swap(m_hInternet, inetConnection.m_hInternet);
}

CInternetConnection::~CInternetConnection()
{
	close();
}

CInternetConnection& CInternetConnection::operator=(CInternetConnection&& inetConnection)
{
	if (&inetConnection != this)
	{
		close();
		std::swap(m_hInternet, inetConnection.m_hInternet);
	}

	return *this;
}

void CInternetConnection::open(const std::string& strAgent)
{
	close();

	m_hInternet = InternetOpen(strAgent.empty() ? HttpUserAgent : strAgent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, INTERNET_FLAG_ASYNC);
	if (!m_hInternet)
		throw CInternetException(GetLastError());
}

void CInternetConnection::close()
{
	if (m_hInternet)
		InternetCloseHandle(m_hInternet);
	m_hInternet = nullptr;
}

CHttpSession CInternetConnection::getHttpSession() const
{
	return CHttpSession(*this);
}

unsigned long CInternetConnection::getLastResponse() const
{
	DWORD dwError = 0;
	DWORD dwBufferLength = 0;

	if (InternetGetLastResponseInfo(&dwError, nullptr, &dwBufferLength) == FALSE)
		throw CInternetException(GetLastError());

	return dwError;
}

std::string CInternetConnection::getLastResponseInfo() const
{
	DWORD dwError = 0;
	DWORD dwBufferLength = 0;

	if (InternetGetLastResponseInfo(&dwError, nullptr, &dwBufferLength) == FALSE)
		throw CInternetException(GetLastError());

	dwBufferLength++;
	std::vector<char> buffer(dwBufferLength);
	if (InternetGetLastResponseInfo(&dwError, buffer.data(), &dwBufferLength) == FALSE)
		throw CInternetException(GetLastError());

	return buffer.data();
}

/************************************************************************/
/* CHttpSession                                                         */
/************************************************************************/

CHttpSession::CHttpSession(const CInternetConnection& Connection) :
	m_Connection(Connection),
	m_hConnect(nullptr)
{
}

CHttpSession::CHttpSession(CHttpSession&& httpSession) :
	m_Connection(httpSession.m_Connection),
	m_hConnect(nullptr)
{
	std::swap(m_hConnect, httpSession.m_hConnect);
}

CHttpSession::~CHttpSession()
{
	close();
}

void CHttpSession::open(const std::string& strHost, unsigned short usPort)
{
	if (m_hConnect)
		throw CInternetException(ERROR_INVALID_HANDLE, "Already opened");

	m_hConnect = InternetConnect(
		m_Connection.m_hInternet,			// Handle returned by a previous call to InternetOpen.
		strHost.c_str(),					// Host name of an Internet server. 
		usPort,								// (TCP/IP) port on the server.
		nullptr,								// Name of the user to log on.
		nullptr,								// Password to use to log on.
		INTERNET_SERVICE_HTTP,				// Type of service to access.
		0,									// Options specific to the service used.
		reinterpret_cast<DWORD_PTR>(this));	// Pointer to a variable that contains an application-defined value for callback.
	if (!m_hConnect)
		throw CInternetException(GetLastError());
}

void CHttpSession::close()
{
	if (m_hConnect)
		InternetCloseHandle(m_hConnect);
	m_hConnect = nullptr;
}

CHttpRequest CHttpSession::getRequest() const
{
	return CHttpRequest(*this);
}

/************************************************************************/
/* CHttpRequest                                                         */
/************************************************************************/

CHttpRequest::CHttpRequest(const CHttpSession& Session) :
	m_Session(Session),
	m_httpException(0),
	m_lastNumberOfBytesRead(0),
	m_totalNumberOfBytesRead(0),
	m_tmpBuffer(SizeOfTempBuffer),
	m_pStream(nullptr),
	m_hRequest(nullptr),
	m_hCloseEvent(nullptr),
	m_hEndEvent(nullptr),
	m_bEndRequest(true)
{
	InitializeCriticalSection(&m_hCriticalSection);

	m_hCloseEvent = CreateEvent(nullptr, TRUE, TRUE, nullptr);
	if (!m_hCloseEvent)
		throw CInternetException(GetLastError());

	m_hEndEvent = CreateEvent(nullptr, TRUE, TRUE, nullptr);
	if (!m_hEndEvent)
	{
		CloseHandle(m_hCloseEvent);
		throw CInternetException(GetLastError());
	}
}

CHttpRequest::~CHttpRequest()
{
	try
	{
		close();
		wait();
	}
	catch (...) {}

	if (m_hCloseEvent)
		CloseHandle(m_hCloseEvent);

	if (m_hEndEvent)
		CloseHandle(m_hEndEvent);

	DeleteCriticalSection(&m_hCriticalSection);
}

void CHttpRequest::setBufferSize(size_t length)
{
	m_tmpBuffer.resize(length);
}

void CHttpRequest::setEndCallback(const CallbackFunction& EndCallback)
{
	m_EndCallback = EndCallback;
}

void CHttpRequest::open(const std::string& strRequest, bool bSecure, const std::string& strReferrer, const std::string& strMethod)
{
	CScopedCriticalSection scs(m_hCriticalSection);
	if (m_hRequest)
		throw CInternetException(ERROR_INVALID_HANDLE, "Already opened");

	LPCSTR lplpszAcceptTypes[2] = { "*/*", nullptr };

	m_hRequest = HttpOpenRequest(
		m_Session.m_hConnect,
		strMethod.empty() ? nullptr : strMethod.c_str(),
		strRequest.empty() ? nullptr : strRequest.c_str(),
		nullptr,
		strReferrer.empty() ? nullptr : strReferrer.c_str(),
		lplpszAcceptTypes,
		(bSecure ? INTERNET_FLAG_SECURE : INTERNET_FLAG_KEEP_CONNECTION) | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD,
		reinterpret_cast<DWORD_PTR>(this));	// Pointer to a variable that contains an application-defined value for callback.
	if (!m_hRequest)
		throw CInternetException(GetLastError());

	if (InternetSetStatusCallback(m_hRequest, CHttpRequest::InternetStatusCallback) == INTERNET_INVALID_STATUS_CALLBACK)
		throw CInternetException(ERROR_INVALID_PARAMETER, "Bad callback");
}

void CHttpRequest::close()
{
	cancel();

	DWORD result = WaitForSingleObject(m_hCloseEvent, INFINITE);
	if (result == WAIT_FAILED)
		throw CInternetException(GetLastError());
}

void CHttpRequest::addHeader(const std::string& key, const std::string& value)
{
	std::map<std::string, std::string> headers;
	headers[key] = value;
	addHeaders(headers);
}

void CHttpRequest::addHeaders(const std::map<std::string, std::string>& headers)
{
	std::string strHeaders;

	for (auto it = headers.begin(); it != headers.end(); ++it)
		strHeaders += it->first + ": " + it->second + "\r\n\r\n";

	if (HttpAddRequestHeaders(m_hRequest, strHeaders.c_str(), static_cast<DWORD>(strHeaders.size()), HTTP_ADDREQ_FLAG_ADD) == FALSE)
		throw CInternetException(GetLastError());
}

void CHttpRequest::setTimeout(timeout_t tTimeout, size_t msTimeOut)
{
	DWORD dwTimeOut = static_cast<DWORD>(msTimeOut);
	DWORD dwoption = 0;

	switch (tTimeout)
	{
	case ConnectTimeout:
		dwoption = INTERNET_OPTION_CONNECT_TIMEOUT;
		break;

	case SendTimeout:
		dwoption = INTERNET_OPTION_SEND_TIMEOUT;
		break;

	case ReceiveTimeout:
		dwoption = INTERNET_OPTION_RECEIVE_TIMEOUT;
		break;

	default:
		break;
	}

	if (InternetSetOption(m_hRequest, dwoption, &dwTimeOut, sizeof(DWORD)) == FALSE)
		throw CInternetException(GetLastError());
}

void CHttpRequest::send(std::ostream& oss, const std::string& strPostData)
{
	CScopedCriticalSection scs(m_hCriticalSection);
	m_totalNumberOfBytesRead = 0;
	m_strPostData = strPostData;
	m_pStream = &oss;

	if (ResetEvent(m_hCloseEvent) == FALSE)
		throw CInternetException(GetLastError());

	if (ResetEvent(m_hEndEvent) == FALSE)
		throw CInternetException(GetLastError());

	if (HttpSendRequest(m_hRequest, nullptr, 0, m_strPostData.empty() ? nullptr : (LPVOID)m_strPostData.data(), static_cast<DWORD>(m_strPostData.size())) == FALSE)
	{
		DWORD dwError = GetLastError();
		if (dwError != ERROR_IO_PENDING)
			throw CInternetException(dwError);
	}

	m_bEndRequest = false;
}

void CHttpRequest::cancel()
{
	CScopedCriticalSection scs(m_hCriticalSection);
	if (m_hRequest)
	{
		InternetCloseHandle(m_hRequest);
		m_hRequest = nullptr;
	}
}

bool CHttpRequest::wait(size_t msTimeOut)
{
	DWORD result = WaitForSingleObject(m_hEndEvent, static_cast<DWORD>(msTimeOut));
	if (result == WAIT_FAILED)
		throw CInternetException(GetLastError());

	if (m_httpException.code() != ERROR_SUCCESS)
	{
		SetLastError(m_httpException.code());
		if (m_httpException.code() != ERROR_INTERNET_OPERATION_CANCELLED)
			throw m_httpException;
	}

	return (result == WAIT_OBJECT_0);
}

size_t CHttpRequest::getNumberOfBytesRead() const
{
	return m_totalNumberOfBytesRead;
}

void CHttpRequest::signal(HINTERNET hRequest)
{
	{
		CScopedCriticalSection scs(m_hCriticalSection);
		InternetSetStatusCallback(hRequest, INTERNET_NO_CALLBACK);
		m_bEndRequest = true;

		if (SetEvent(m_hCloseEvent) == FALSE)
			throw CInternetException(GetLastError());
	}

	if (m_EndCallback)
		try { m_EndCallback(*this, m_httpException); }
	catch (...) {}

	CScopedCriticalSection scs(m_hCriticalSection);
	if (m_bEndRequest && SetEvent(m_hEndEvent) == FALSE)
		throw CInternetException(GetLastError());
}

void CHttpRequest::closeRequest(HINTERNET hRequest, const CInternetException& httpException)
{
	m_httpException = httpException;
	InternetCloseHandle(hRequest);
	if (m_hRequest == hRequest)
		m_hRequest = nullptr;
}

void CHttpRequest::processRequest(HINTERNET hRequest)
{
	bool bRet = false;
	do
	{
		if (m_lastNumberOfBytesRead > 0)
		{	// Read previous buffer
			m_pStream->write(m_tmpBuffer.data(), m_lastNumberOfBytesRead);
			m_totalNumberOfBytesRead += m_lastNumberOfBytesRead;
			m_lastNumberOfBytesRead = 0;
		}

		bRet = (InternetReadFile(hRequest, m_tmpBuffer.data(), static_cast<DWORD>(m_tmpBuffer.size()), &m_lastNumberOfBytesRead) == TRUE);
		if (!bRet && GetLastError() != ERROR_IO_PENDING)
			throw CInternetException(GetLastError());

		if (bRet && !m_lastNumberOfBytesRead)
			closeRequest(hRequest, ERROR_SUCCESS);
	} while (bRet && m_lastNumberOfBytesRead > 0);
}

/*
Callback routine for asynchronous WinInet operations

Arguments:
hInternet - The handle for which the callback function is called.
dwContext - Pointer to the application defined context.
dwInternetStatus - Status code indicating why the callback is called.
lpvStatusInformation - Pointer to a buffer holding callback specific data.
dwStatusInformationLength - Specifies size of lpvStatusInformation buffer.

Return Value: None.
*/
void CHttpRequest::InternetStatusCallback(
	HINTERNET hInternet,
	DWORD_PTR dwContext,
	DWORD dwInternetStatus,
	LPVOID lpvStatusInformation,
	DWORD)
{
	CHttpRequest* pHttpRequest = reinterpret_cast<CHttpRequest*>(dwContext);

	try
	{
		if (dwInternetStatus == INTERNET_STATUS_REQUEST_COMPLETE)
		{
			LPINTERNET_ASYNC_RESULT pAsyncResult = reinterpret_cast<LPINTERNET_ASYNC_RESULT>(lpvStatusInformation);
			if (pAsyncResult->dwResult == FALSE)
				throw CInternetException(pAsyncResult->dwError);

			pHttpRequest->processRequest(hInternet);
		}
		else if (dwInternetStatus == INTERNET_STATUS_HANDLE_CLOSING)
		{
			pHttpRequest->signal(hInternet);
		}
	}
	catch (CInternetException& httpException)
	{
		pHttpRequest->closeRequest(hInternet, httpException);
	}
}

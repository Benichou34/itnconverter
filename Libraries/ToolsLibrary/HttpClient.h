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

#ifndef HTTP_CLIENT_H_INCLUDED
#define HTTP_CLIENT_H_INCLUDED

#include <map>
#include <string>
#include <vector>
#include <functional>
#include <windows.h>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

class CHttpSession;
class CHttpRequest;

class CInternetException : public std::exception
{
public:
	CInternetException(unsigned long error) :
		m_ErrorCode(error)
	{
	}

	CInternetException(unsigned long error, const std::string& msg) :
		m_ErrorCode(error),
		m_Message(msg)
	{
	}

	CInternetException(const CInternetException& ex) :
		m_ErrorCode(ex.m_ErrorCode),
		m_Message(ex.m_Message)
	{
	}

	virtual ~CInternetException() {}

	CInternetException& operator=(const CInternetException& ex)
	{
		if (&ex != this)
		{
			m_ErrorCode = ex.m_ErrorCode;
			m_Message = ex.m_Message;
		}

		return *this;
	}

	virtual const char* what() const noexcept { return m_Message.c_str(); } // Get message associated to exception
	unsigned long code() const { return m_ErrorCode; } // Returns the error_code object associated with the exception.

private:
	unsigned long m_ErrorCode;
	std::string m_Message;
};

class CInternetConnection
{
public:
	CInternetConnection(const std::string& strAgent = std::string());
	CInternetConnection(CInternetConnection&& inetConnection);
	~CInternetConnection();

	CInternetConnection& operator=(CInternetConnection&& inetConnection);
	unsigned long getLastResponse() const;
	std::string getLastResponseInfo() const;

	void open(const std::string& strAgent = std::string());
	void close();

	CHttpSession getHttpSession() const;

private:
	friend class CHttpSession;

	CInternetConnection(const CInternetConnection&) = delete;
	CInternetConnection& operator=(const CInternetConnection&) = delete;

private:
	void* m_hInternet;
};

class CHttpSession
{
public:
	CHttpSession(CHttpSession&& httpSession);
	~CHttpSession();

	void open(const std::string& strHost, unsigned short usPort = CHttpSession::DefaultHttpPort);
	void close();

	CHttpRequest getRequest() const;

	static const unsigned short DefaultHttpPort;
	static const unsigned short DefaultHttpsPort;

private:
	friend class CInternetConnection;
	friend class CInternetHttpSession;
	friend class CHttpRequest;

	CHttpSession(const CInternetConnection& Connection);
	CHttpSession(const CHttpSession&) = delete;
	CHttpSession& operator=(const CHttpSession&) = delete;

private:
	const CInternetConnection& m_Connection;
	void* m_hConnect;
};

class CHttpRequest
{
public:
	typedef std::function<void(CHttpRequest&, const CInternetException&)> CallbackFunction;
	typedef enum
	{
		ConnectTimeout,
		SendTimeout,
		ReceiveTimeout
	} timeout_t;

	CHttpRequest(CHttpRequest&& httpRequest);
	~CHttpRequest();

	void setBufferSize(size_t length);
	void setEndCallback(const CallbackFunction& EndCallback);

	void open(const std::string& strRequest = std::string(), bool bSecure = false, const std::string& strReferrer = std::string(), const std::string& strMethod = std::string());
	void close();
	void addHeader(const std::string& key, const std::string& value);
	void addHeaders(const std::map<std::string, std::string>& headers);
	void setTimeout(timeout_t tTimeout, size_t msTimeOut);
	void send(std::ostream& oss, const std::string& strPostData = std::string());
	void cancel();
	bool wait(size_t msTimeOut = static_cast<size_t>(-1));
	size_t getNumberOfBytesRead() const;

	static const std::string MethodPost;
	static const std::string MethodGet;

private:
	friend class CHttpSession;
	friend class CInternetHttpSession;

	CHttpRequest(const CHttpSession& Session);
	CHttpRequest(const CHttpRequest&) = delete;
	CHttpRequest& operator=(const CHttpRequest&) = delete;

	void signal(void* hRequest);
	void closeRequest(void* hRequest, const CInternetException& httpException);
	void processRequest(void* hRequest);

	static VOID CALLBACK InternetStatusCallback(
		void* hInternet,
		DWORD_PTR dwContext,
		DWORD dwInternetStatus,
		LPVOID lpvStatusInformation,
		DWORD dwStatusInformationLength);

private:
	const CHttpSession& m_Session;
	CallbackFunction m_EndCallback;
	CInternetException m_httpException;
	unsigned long m_lastNumberOfBytesRead;
	size_t m_totalNumberOfBytesRead;
	std::vector<char> m_tmpBuffer;
	std::string m_strPostData;
	std::ostream* m_pStream;
	mutable CRITICAL_SECTION m_hCriticalSection;
	void* m_hRequest;
	void* m_hCloseEvent;
	void* m_hEndEvent;
	bool m_bEndRequest;
};

#endif // !HTTP_CLIENT_H_INCLUDED

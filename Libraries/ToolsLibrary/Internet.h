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

#ifndef INTERNET_H_INCLUDED
#define INTERNET_H_INCLUDED

#include <string>
#include <memory>
#include <functional>

class CInternetConnection;
class CHttpSession;
class CHttpRequest;
class CInternetException;

class CInternetHttpSession
{
public:
	typedef std::function<void(CInternetHttpSession&, const CInternetException&)> CallbackFunction;

	CInternetHttpSession();
	CInternetHttpSession(CInternetHttpSession&& httpSession);
	~CInternetHttpSession();

	CInternetHttpSession& operator=(CInternetHttpSession&& httpSession);

	void send(std::ostream& oss,
		const std::string& strUrl,
		const std::string& strPostData = std::string(),
		const std::string& strReferrer = std::string(),
		const std::string& strHost = std::string());
	void cancel();
	bool wait(size_t msTimeOut = static_cast<size_t>(-1));
	size_t getNumberOfBytesRead() const;
	void setEndCallback(const CallbackFunction& EndCallback);

private:
	friend class CInternetConnection;

	CInternetHttpSession(const CInternetHttpSession&) = delete;
	CInternetHttpSession& operator=(const CInternetHttpSession&) = delete;

private:
	CallbackFunction m_EndCallback;
	std::unique_ptr<CInternetConnection> m_pInternetConnection;
	std::unique_ptr<CHttpSession> m_pHttpSession;
	std::unique_ptr<CHttpRequest> m_pHttpRequest;
};

class CInternet
{
public:
	static size_t HttpToStream(
		std::ostream& oss,
		const std::string& strRequest,
		const std::string& strPostData = std::string(),
		const std::string& strReferrer = std::string(),
		const std::string& strHost = std::string());
	static std::string AjaxHttpRequest(
		const std::string& strRequest,
		const std::string& strReferrer = std::string(),
		const std::string& strHost = std::string());
	static std::wstring HttpDownload(
		const std::string& strRequest,
		const std::string& strReferrer = std::string(),
		const std::wstring& strTargetFile = std::wstring());
};

#endif // !INTERNET_H_INCLUDED

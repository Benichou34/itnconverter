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

#include "stdafx.h"
#include "HttpRequest.h"
#include "ToolsLibrary/HttpClient.h"
#include "Javascript.h"
#include "Navigator.h"

CAsyncHttpRequest::CAsyncHttpRequest(CNavigator& navigator) :
	m_Navigator(navigator),
	m_Requests(2),
	m_lpCallback(nullptr)
{
	m_inetHttpSession.setEndCallback([&](CInternetHttpSession&, const CInternetException& exception)
		{
			std::lock_guard<std::mutex> mlg(m_Mutex);
			auto clearSession = [&]()
			{
				m_lpCallback->Release();
				m_lpCallback = nullptr;
				m_oss.str(std::string());
			};

			try
			{
				std::unique_ptr<CJavaScript> pScript = std::make_unique<CJavaScript>(m_lpCallback);
				pScript->arg(exception.code()).arg(std::move(m_oss.str()));
				m_Navigator.PostJavaScript(pScript);
			}
			catch (...) {}

			clearSession();

			Request request;
			while (!m_lpCallback && m_Requests.try_pop_front(request) == stdx::cq_status::no_timeout)
			{
				try
				{
					m_lpCallback = request.lpCallback;
					m_inetHttpSession.send(m_oss, request.strRequest, std::string(), request.strReferrer, request.strHost);
				}
				catch (...)
				{
					clearSession();
				}
			}
		});
}

CAsyncHttpRequest::~CAsyncHttpRequest()
{
	m_Requests.clear();
	m_inetHttpSession.cancel();
	m_inetHttpSession.wait();
}

void CAsyncHttpRequest::PostRequest(const std::string& strRequest, const std::string& strReferrer, const std::string& strHost, IDispatch* lpCallback)
{
	if (!lpCallback)
		return;

	lpCallback->AddRef();

	std::lock_guard<std::mutex> mlg(m_Mutex);
	if (!m_lpCallback) // No current request
	{
		m_lpCallback = lpCallback;
		m_inetHttpSession.send(m_oss, strRequest, std::string(), strReferrer, strHost);
	}
	else
	{
		m_Requests.push_front(std::move(Request(strRequest, strReferrer, strHost, lpCallback)), false);
	}
}

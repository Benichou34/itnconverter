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

#ifndef __HTTP_REQUEST_H_
#define __HTTP_REQUEST_H_

#include <sstream>
#include "stdx/concurrent_queue.h"
#include "ToolsLibrary/Internet.h"

class CNavigator;

class CAsyncHttpRequest
{
public:
	CAsyncHttpRequest(CNavigator& navigator);
	~CAsyncHttpRequest();

	void PostRequest(const std::string& strRequest, const std::string& strReferrer, const std::string& strHost, IDispatch* lpCallback);

private:
	CAsyncHttpRequest(CAsyncHttpRequest&) = delete;
	CAsyncHttpRequest& operator=(CAsyncHttpRequest&) = delete;

	struct Request
	{
		std::string strRequest;
		std::string strReferrer;
		std::string strHost;
		IDispatch* lpCallback;

		Request() : lpCallback(nullptr) { }

		Request(const std::string& _strRequest, const std::string& _strReferrer, const std::string& _strHost, IDispatch* _lpCallback) :
			strRequest(_strRequest),
			strReferrer(_strReferrer),
			strHost(_strHost),
			lpCallback(_lpCallback)
		{
		}
	};

	CNavigator& m_Navigator;
	std::mutex m_Mutex;
	stdx::concurrent_queue<Request> m_Requests;
	CInternetHttpSession m_inetHttpSession;
	std::stringstream m_oss;
	IDispatch* m_lpCallback;
};

#endif // __HTTP_REQUEST_H_

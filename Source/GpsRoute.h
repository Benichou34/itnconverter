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

#if !defined(_GPSROUTE_H_INCLUDED_)
#define _GPSROUTE_H_INCLUDED_

#include "GpsWaypointArray.h"

class CGpsRoute : public CGpsPointArray
{
public:
	CGpsRoute();
	CGpsRoute(const CGpsRoute& route);
	CGpsRoute(const CGpsPointArray& array, size_t begin = 0, size_t count = static_cast<size_t>(-1));
	virtual ~CGpsRoute() {}

	CGpsRoute& operator= (const CGpsPointArray& array);
	CGpsRoute& operator= (const CGpsRoute& route);

	virtual E_ARRAY_TYPE getType() const { return E_ARRAY_ROUTE; }
	virtual void clear();

	bool IsModified() const;
	void ClearModified();

private:
	CGpsWaypointArray m_GpsWaypointOrg;
};

#endif // !defined(_GPSROUTE_H_INCLUDED_)

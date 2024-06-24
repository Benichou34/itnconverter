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
 * Purpose : Send coordinates or routes to a target GPS device, if available
 */

#ifndef _SENDTOGPS_H_INCLUDED_
#define _SENDTOGPS_H_INCLUDED_

#include "GpsPointArray.h"

class CSendToGps
{
public:
	typedef enum
	{
		E_TYPE_POINT,
		E_TYPE_ROUTE
	} E_TYPE;

	typedef enum
	{
		E_GPS_NAVIGON,
		E_GPS_PIONEER,
		E_GPS_MIOMORE,
		E_GPS_GARMIN
	} E_GPS;

	static bool IsAvailabled(E_GPS eGps, E_TYPE eType);
	static void SendTo(E_GPS eGps, const CGpsPoint& cGpsPoint);
	static void SendTo(E_GPS eGps, const CGpsPointArray& cGpsPointArray);
};
#endif /*_SENDTOGPS_H_INCLUDED_*/

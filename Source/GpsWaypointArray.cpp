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
#include "GpsWaypointArray.h"

 //////////////////////////////////////////////////////////////////////
 // Construction/Destruction
 //////////////////////////////////////////////////////////////////////

CGpsWaypointArray::CGpsWaypointArray() : CGpsPointArray()
{
}

CGpsWaypointArray::CGpsWaypointArray(const CGpsWaypointArray& wptArray) : CGpsPointArray(wptArray)
{
}

CGpsWaypointArray::CGpsWaypointArray(const CGpsPointArray& array, size_t begin, size_t count) : CGpsPointArray(array, begin, count)
{
}

CGpsWaypointArray& CGpsWaypointArray::operator= (const geo::CGeoLocations& gLocations)
{
	CGpsPointArray::operator=(gLocations);
	return *this;
}

CGpsWaypointArray& CGpsWaypointArray::operator= (const CGpsPointArray& array)
{
	CGpsPointArray::operator=(array);
	return *this;
}

CGpsWaypointArray& CGpsWaypointArray::operator= (const CGpsWaypointArray& wptArray)
{
	if (&wptArray == this)
		return *this;

	CGpsPointArray::operator=(wptArray);
	return *this;
}

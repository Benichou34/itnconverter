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

#include "GeoNamesTools.h"

using namespace geo;

E_GEO_STATUS_CODE CGeoNamesTools::GetStatusCode(unsigned int unStatus)
{
	switch (unStatus)
	{
	case 10: // Authorization Exception
		return E_GEO_REQUEST_DENIED;

	case 11: // record does not exist
	case 14: // invalid parameter
	case 21: // invalid input
		return E_GEO_INVALID_REQUEST;

	case 15: // no result found
		return E_GEO_ZERO_RESULTS;

	case 17: // postal code not found
		return E_GEO_NOT_FOUND;

	case 18: // daily limit of credits exceeded
	case 19: // hourly limit of credits exceeded
	case 20: // weekly limit of credits exceeded
		return E_GEO_OVER_QUERY_LIMIT;

	case 12: // other error
	case 13: // database timeout
	case 16: // duplicate exception
	case 22: // server overloaded exception
	case 23: // service not implemented
	default:
		return E_GEO_UNKNOWN_ERROR;
	}
}

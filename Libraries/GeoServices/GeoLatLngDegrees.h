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

#ifndef _GEO_LATLNG_DEGREES_H_INCLUDED_
#define _GEO_LATLNG_DEGREES_H_INCLUDED_

#include "GeoLatLng.h"

namespace geo
{
	class CGeoLatLngDegrees
	{
	public:
		typedef struct
		{
			long lDegrees;
			double dMinutes;
			bool bDirection; // false: N/E, true: S/W
		} S_DM;

		typedef struct
		{
			long lDegrees;
			long lMinutes;
			double dSeconds;
			bool bDirection; // false: N/E, true: S/W
		} S_DMS;

		CGeoLatLngDegrees();
		CGeoLatLngDegrees(const CGeoLatLngDegrees& gLatLngDegrees);
		CGeoLatLngDegrees(const CGeoLatLng& gLatLng);
		CGeoLatLngDegrees(double dLat, double dLng);
		CGeoLatLngDegrees(const S_DM& dmLat, const S_DM& dmLng);
		CGeoLatLngDegrees(const S_DMS& dmsLat, const S_DMS& dmsLng);
		virtual ~CGeoLatLngDegrees() = default;

		CGeoLatLngDegrees& operator= (const CGeoLatLngDegrees& gLatLngDegrees);
		CGeoLatLngDegrees& operator= (const CGeoLatLng& gLatLng);

		void assign(double dLat, double dLng);
		void assign(const S_DM& dmLat, const S_DM& dmLng);
		void assign(const S_DMS& dmsLat, const S_DMS& dmsLng);

		S_DM dmLat() const;
		S_DM dmLng() const;

		S_DMS dmsLat() const;
		S_DMS dmsLng() const;

		const CGeoLatLng& latLng() const;

	private:
		CGeoLatLng m_geoLatLng;
	};
} // namespace geo

#endif /*_GEO_LATLNG_H_INCLUDED_*/

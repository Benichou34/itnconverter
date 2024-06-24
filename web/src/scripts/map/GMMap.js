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

function GMMap(type)
{
	if(use_google === true)
	{
		var mapOptions =
		{ 
			getTileUrl: function(coord, zoom) {return "";},
			tileSize: new google.maps.Size(256, 256),
			maxZoom: -1,
			name: 'Google Maps'
		};

		return new GMapType(mapOptions);
	}
	else
	{
		var mapOptions =
		{ 
			tileSize: new google.maps.Size(256, 256), 
			minZoom: 1, 
			maxZoom: 20, 
			name: 'Google Maps', 
			credit: '&copy;2018 Google'
		};

		switch(type)
		{
		case _TYPE_ROUTE_MAP:
			mapOptions.getTileUrl = function(coord, zoom) {return "https://mt.google.com/vt/lyrs=m&x="+coord.x+"&y="+coord.y+"&z="+zoom;};
			break;

		case _TYPE_AERIAL_MAP:
			mapOptions.getTileUrl = function(coord, zoom) {return "https://mt.google.com/vt/lyrs=s&x="+coord.x+"&y="+coord.y+"&z="+zoom;};
			break;

		case _TYPE_HYBRID_MAP:
			mapOptions.getTileUrl = function(coord, zoom) {return "https://mt.google.com/vt/lyrs=y&x="+coord.x+"&y="+coord.y+"&z="+zoom;};
			break;

		case _TYPE_PHYSICAL_MAP:
			mapOptions.getTileUrl = function(coord, zoom) {return "https://mt.google.com/vt/lyrs=p&x="+coord.x+"&y="+coord.y+"&z="+zoom;};
			break;

		default:
			return null;
		}
		
		return createMapType(mapOptions);
	}
}

RegisterMap('GM', GMMap, "Google Maps", 5, {map: true, sat: true, hybrid: true, topo: true});

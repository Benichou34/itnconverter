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

function GDLMap(type)
{
	if(type != _TYPE_PHYSICAL_MAP) {
		return null;
	}

	var mapOptions =
	{ 
		getTileUrl: function(tile, zoom) {return HttpRelay("https://wmts3.geoportail.lu/mapproxy_4_v3/wmts/topogr_global/GLOBAL_WEBMERCATOR_4_V3/"+zoom+"/"+tile.x+"/"+tile.y+".png", "http://www.geoportail.lu");},
		tileSize: new google.maps.Size(256, 256), 
		minZoom: 3, 
		maxZoom: 16, 
		alt: 'Grand-Duché de Luxembourg', 
		name: 'Grand-Duché de Luxembourg', 
		credit: '&copy; Grand-Duché de Luxembourg <a href="http://www.geoportail.lu" target="_blank">GeoPortail</a>'
	};

	return createMapType(mapOptions);
}

RegisterMap('GDL', GDLMap, "Grand-Duché de Luxembourg", 24, {topo: true});

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

function GPMap(type)
{
	var mapLayer;

	switch(type)
	{
	case _TYPE_ROUTE_MAP:
		mapLayer = "GEOGRAPHICALGRIDSYSTEMS.MAPS";
		break;
	
	case _TYPE_PHYSICAL_MAP:
		mapLayer = "GEOGRAPHICALGRIDSYSTEMS.MAPS.SCAN25TOUR";
		break;

	default:
		return null;
	}

	var mapOptions =
	{ 
		getTileUrls: [function(coord, zoom) {return "https://wxs.ign.fr/here_your_api_key/geoportail/wmts?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=" + mapLayer + "&STYLE=normal&TILEMATRIXSET=PM&TILEMATRIX=" + zoom + "&TILEROW=" + coord.y + "&TILECOL=" + coord.x + "&FORMAT=image/jpeg";}],
		tileSize: new google.maps.Size(256, 256), 
		minZoom: 1, 
		maxZoom: 18, 
		alt: 'IGN geoPortail', 
		name: 'IGN geoPortail', 
		credit: '&copy; 2022 IGN <a href=" http://www.geoportail.gouv.fr" target="_blank">GeoPortail</a>'
	};

	return createMapType(mapOptions);
}

RegisterMap('GP', GPMap, "IGN geoPortail", 13, {map: true, sat: true, hybrid: true, topo: true});

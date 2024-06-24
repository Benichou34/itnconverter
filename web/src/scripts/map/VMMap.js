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

function VMMap(type)
{
	var mapOptions =
	{ 
		getTileUrls: [],
		tileSize: new google.maps.Size(256, 256),
		minZoom: 1, 
		maxZoom: 19, 
		alt: 'ViaMichelin', 
		name: 'ViaMichelin', 
		credit: '&copy;Michelin &copy;TomTom &copy;Natural Earth'
	};

	switch(type)
	{
	case _TYPE_ROUTE_MAP:
		mapOptions.getTileUrls.push(function(tile, zoom) {return "http://map.viamichelin.com/map/mapdirect?map=viamichelin&z=" + zoom + "&x=" + tile.x + "&y=" + tile.y + "&format=png&version=201503191157&layer=background";});
		break;

	case _TYPE_AERIAL_MAP:
		mapOptions.getTileUrls.push(getVESatUrl);
		break;

	case _TYPE_HYBRID_MAP:
		mapOptions.getTileUrls.push(getVESatUrl);
		mapOptions.getTileUrls.push(function(tile, zoom) {return "http://map.viamichelin.com/map/mapdirect?map=hybrid&z=" + zoom + "&x=" + tile.x + "&y=" + tile.y + "&format=png&version=201503191157&layer=network";});
		break;

	default:
		return null;
	}
	
	return createMapType(mapOptions);
}

RegisterMap('VM', VMMap, "ViaMichelin", 25, {map: true, sat: false, hybrid: true});

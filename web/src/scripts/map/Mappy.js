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

function MappyMap(type)
{
	var mapOptions =
	{ 
		getTileUrls: [],
		tileSize: new google.maps.Size(256, 256), 
		minZoom: 4, 
		maxZoom: 19, 
		alt: 'Mappy Maps', 
		name: 'Mappy Maps', 
		credit: '&copy;Mappy | 2020 TomTom'
	};

	switch(type)
	{
	case _TYPE_ROUTE_MAP:
		mapOptions.getTileUrls.push(function(coord, zoom) {return HttpRelay('http://map1.mappy.net/map/1.0/slab/standard/256/'+zoom+'/'+coord.x+'/'+coord.y, 'https://fr.mappy.com/');});
		break;

	case _TYPE_AERIAL_MAP:
		mapOptions.getTileUrls.push(function(coord, zoom) {return HttpRelay("http://map1.mappy.net/map/1.0/slab/photo/256/"+zoom+"/"+coord.x+"/"+coord.y, 'https://fr.mappy.com/');});
		break;

	case _TYPE_HYBRID_MAP:
		mapOptions.getTileUrls.push(function(coord, zoom) {return HttpRelay("http://map1.mappy.net/map/1.0/slab/photo/256/"+zoom+"/"+coord.x+"/"+coord.y, 'https://fr.mappy.com/');});
		mapOptions.getTileUrls.push(function(coord, zoom) {return HttpRelay("http://map2.mappy.net/map/1.0/slab/hybrid/256/"+zoom+"/"+coord.x+"/"+coord.y, 'https://fr.mappy.com/');});
		break;

	default:
		return null;
	}

	return createMapType(mapOptions);
}

RegisterMap('MPY', MappyMap, "Mappy Maps", 28, {map: true, sat: true, hybrid: true});

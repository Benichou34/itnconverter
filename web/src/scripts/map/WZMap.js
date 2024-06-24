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

function WazeMap(type)
{
	if(type != _TYPE_ROUTE_MAP)
		return null;

	var mapOptions =
	{
		getTileUrls: [function(coord, zoom) {return "https://livemap-tiles1.waze.com/tiles/"+zoom+"/"+coord.x+"/"+coord.y+".png";}],
		tileSize: new google.maps.Size(256, 256), 
		minZoom: 1, 
		maxZoom: 18, 
		alt: 'Waze USA Map', 
		name: 'Waze USA Map', 
		credit: '&copy; 2006-2017 Waze Mobile. Tous Droits Réservés. <a href="https://www.waze.com/fr/legal/notices" target="_blank">Notices</a>'
	};

	return createMapType(mapOptions);
}

function WazeWorldMap(type)
{
	if(type != _TYPE_ROUTE_MAP)
		return null;

	var mapOptions =
	{ 
		getTileUrls: [function(coord, zoom) {return "https://worldtiles1.waze.com/tiles/"+zoom+"/"+coord.x+"/"+coord.y+".png";}],
		tileSize: new google.maps.Size(256, 256), 
		minZoom: 4, 
		maxZoom: 18, 
		alt: 'Waze World Map', 
		name: 'Waze World Map', 
		credit: '&copy; 2006-2017 Waze Mobile. Tous Droits Réservés. <a href="https://www.waze.com/fr/legal/notices" target="_blank">Notices</a>'
	};

	return createMapType(mapOptions);
}

RegisterMap('WZ', WazeMap, "Waze USA Map", 19, {map: true});
RegisterMap('WZW', WazeWorldMap, "Waze World Map", 19, {map: true});

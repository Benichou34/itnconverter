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

// http://www.kartverket.no/kart/gratis-kartdata/wms-tjenester/
function NOMap(type)
{
	if(type != _TYPE_PHYSICAL_MAP)
		return null;

	var mapOptions =
	{
		getTileUrls: [function(coord, zoom) {return 'https://gatekeeper1.geonorge.no/BaatGatekeeper/gk/gk.cache_wmts?&gkt=AFBEE1CC7D863648740611D6FF4EC590A95C562443808246C96A69F76B53A13AB2E212F889460F86912581370EED5EF68C0E31193C8F44F68D018F0853CD0DA1&layer=norgeskart_bakgrunn&style=default&tilematrixset=EPSG:3857&Service=WMTS&Request=GetTile&Version=1.0.0&Format=image/png&TileMatrix=EPSG:3857:'+zoom+'&TileCol='+coord.x+'&TileRow='+coord.y;}],
		tileSize: new google.maps.Size(256, 256),
		minZoom: 3, 
		maxZoom: 20, 
		alt: 'Norway Topographic Map', 
		name: 'Norway Topographic Map', 
		credit: '<a href="https://www.norgeskart.no" target="_blank">&copy; Norgeskart</a>'
	};

	return createMapType(mapOptions);
}

RegisterMap('NO', NOMap, "Norway Topographic Map", 30, {topo: true});

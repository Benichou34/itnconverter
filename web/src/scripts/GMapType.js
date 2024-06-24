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
 /*
alt 	string 	Alt text to display when this MapType's button is hovered over in the MapTypeControl. Optional.
maxZoom 	number 	The maximum zoom level for the map when displaying this MapType. Required for base MapTypes, ignored for overlay MapTypes.
minZoom 	number 	The minimum zoom level for the map when displaying this MapType. Optional; defaults to 0.
name 	string 	Name to display in the MapTypeControl. Optional.
projection 	Projection 	The Projection used to render this MapType. Optional; defaults to Mercator.
radius 	number 	Radius of the planet for the map, in meters. Optional; defaults to Earth's equatorial radius of 6378137 meters.
tileSize 	Size 	The dimensions of each tile. Required.

getTileUrls
credit
*/

var refMap = {};

function RegisterMap(flag, object, name, icon, maps)
{
	refMap[flag] = 
	{
		flag: flag,
		object: object,
		name: name,
		icon: icon || 0,
		map: maps.map || false,
		sat: maps.sat || false,
		hybrid: maps.hybrid || false,
		topo: maps.topo || false
	};
}

function GetMaps()
{
	return JSON.stringify(refMap);
}

function getWMSBox(tile, zoom)
{
	var s = 256 / map.factor[zoom]; 
	var southWestPixel = new google.maps.Point(tile.x * s, (tile.y + 1) * s);
	var northEastPixel = new google.maps.Point((tile.x + 1) * s, tile.y * s);
 
	var southWest = map.getProjection().fromPointToLatLng(southWestPixel);
	var northEast = map.getProjection().fromPointToLatLng(northEastPixel);

	return '&BBOX='+ southWest.lng()+','+southWest.lat()+','+northEast.lng()+','+northEast.lat();
}

function getNormalizedCoord(coord, zoom)
{
	var y = coord.y;
	var x = coord.x;

	// tile range in one direction range is dependent on zoom level
	// 0 = 1 tile, 1 = 2 tiles, 2 = 4 tiles, 3 = 8 tiles, etc
	var tileRange = 1 << zoom;

	// don't repeat across y-axis (vertically)
	if (y < 0 || y >= tileRange) {
		return null;
	}

  // repeat across x-axis
	if (x < 0 || x >= tileRange) {
		x = (x % tileRange + tileRange) % tileRange;
	}

	return {x: x, y: y};
}

function createMapType(options)
{
	if(!options.getTileUrls && !options.getTileUrl) {
		return null;
	}
	
	options.tileSize = options.tileSize || new google.maps.Size(256, 256);
	options.minZoom = options.minZoom || 0;
	
	if(!options.getTileUrls) {
		options.getTileUrls = [options.getTileUrl];
	}
	
	if(options.getTileUrls.length == 1) {
		if(!options.getTileUrl) {
			options.getTileUrl = options.getTileUrls[0];
		}
		
		var mapType = new google.maps.ImageMapType(options);
		
		if(options.projection) {
			mapType.projection = options.projection;
		}

		if(options.credit) {
			mapType.credit = options.credit;
		}
		
		return mapType;
	} else {
		return new GMapType(options);
	}
}

/** @constructor */
function GMapType(options)
{
	this.getTileUrls = options.getTileUrls;
	this.tileSize = options.tileSize;
	this.maxZoom = options.maxZoom;
	this.minZoom = options.minZoom;

	if(options.projection) {
		this.projection = options.projection;
	}
	if(options.radius) {
		this.radius = options.radius;
	}
	if(options.alt) {
		this.alt = options.alt;
	}
	if(options.name) {
		this.name = options.name;
	}
	if(options.credit) {
		this.credit = options.credit;
	}
}

GMapType.prototype.getDivTile = function(url, ownerDocument)
{
	var div = ownerDocument.createElement('div');
	div.style.width = this.tileSize.width + 'px';
	div.style.height = this.tileSize.height + 'px';
	div.style.backgroundRepeat = "no-repeat";
	if(url) {
		div.style.backgroundImage = 'url(' + url + ')';
	} else {
		div.style.backgroundColor = '#E5E3DF';
	}
	return div;
};

GMapType.prototype.getTile = function(coord, zoom, ownerDocument)
{
	var tile = getNormalizedCoord(coord, zoom);
	if(!tile) {
		return this.getDivTile(null, ownerDocument);
	}

	if(this.getTileUrls.length > 0)
	{
		var div = this.getDivTile(this.getTileUrls[0](tile, zoom), ownerDocument);
		
		for(var i = 1; i < this.getTileUrls.length; i++) {
			div.appendChild(this.getDivTile(this.getTileUrls[i](tile, zoom), ownerDocument));
		}
		
		return div;
	}

	return this.getDivTile(null, ownerDocument);
};

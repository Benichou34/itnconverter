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

var _VE_G = 585;

function getVEDirection(x, y)
{
   return x + (2*y);
}

function getVEName(x, y, z)
{
   var rx, ry;
   var s = '';
   for(var i = 17; i > z; i--)
   {
      rx = x % 2;
      x = Math.floor(x / 2);
      ry = y % 2;
      y = Math.floor(y / 2);
      s = getVEDirection(rx, ry) + s;
   }

   return s;
}

function getVEServer(x, y, z)
{
   var rx, ry;
   var s = '';
   for(var i = 17; i > z; i--)
   {
      rx = x % 2;
      x = Math.floor(x / 2);
      ry = y % 2;
      y = Math.floor(y / 2);
      s = getVEDirection(rx, ry);
   }

   return s;
}

function getVERouteUrl(tile, zoom)
{
   var x = tile.x;
   var y = tile.y;
   var z = 17 - zoom;
   var s = getVEName(x,y,z);
   var v = getVEServer(x,y,z);
   var param = '?g=' + _VE_G + '&n=z&mkt=' + tabLang.hl + '-' + tabLang.hl;

   return 'http://r'+v+'.ortho.tiles.virtualearth.net/tiles/r'+ s + param + '&lbl=l1&stl=h&shading=hill';
}

function getVESatUrl(tile, zoom)
{
   var x = tile.x;
   var y = tile.y;
   var z = 17 - zoom;
   var s = getVEName(x,y,z);
   var v = getVEServer(x,y,z);
   var param = '?g=' + _VE_G + '&n=z&mkt=' + tabLang.hl + '-' + tabLang.hl;

  return 'http://a'+v+'.ortho.tiles.virtualearth.net/tiles/a'+ s + '.jpeg' + param;
}

function getVEHybridUrl(tile, zoom)
{
   var x = tile.x;
   var y = tile.y;
   var z = 17 - zoom;
   var s = getVEName(x,y,z);
   var v = getVEServer(x,y,z);
   var param = '?g=' + _VE_G + '&n=z&mkt=' + tabLang.hl + '-' + tabLang.hl;

   return 'http://h'+v+'.ortho.tiles.virtualearth.net/tiles/h'+ s + '.jpeg' + param;
}

function VEMap(type)
{
	var mapOptions =
	{ 
		getTileUrls: [],
		tileSize: new google.maps.Size(256, 256), 
		minZoom: 1, 
		maxZoom: 19, 
		alt: 'Microsoft Bing Maps', 
		name: 'Bing Maps', 
		credit: '&copy; 2016 HERE, &copy; 2016 Microsoft Corporation'
	};

	switch(type)
	{
	case _TYPE_ROUTE_MAP:
		mapOptions.getTileUrls.push(getVERouteUrl);
		break;

   case _TYPE_AERIAL_MAP:
		mapOptions.getTileUrls.push(getVESatUrl);
		break;

	case _TYPE_HYBRID_MAP:
		mapOptions.getTileUrls.push(getVEHybridUrl);
		break;

	default:
		return null;
	}
	
	return createMapType(mapOptions);
}

RegisterMap('VE', VEMap, "Microsoft Bing Maps", 6, {map: true, sat: true, hybrid: true});

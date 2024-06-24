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

function GetMarkerIcon()
{
	var icoImage = {
		url: "images/marker.png",
		size: new google.maps.Size(20, 34),
		anchor: new google.maps.Point(10, 34)
	};

	var icoShadow = {
		url: "images/shadow50.png",
		size: new google.maps.Size(37, 34),
		anchor: new google.maps.Point(10, 34)
	};

	var icoAnchor = new google.maps.Point(0, -27);
	
	return {image: icoImage, shadow: icoShadow, anchor: icoAnchor};
}

function GetPinIcon()
{
	var icoImage = {
		url: "images/road.png",
		size: new google.maps.Size(25, 25),
		anchor: new google.maps.Point(5, 5)
	};

	var icoShadow = {
		url: "images/roads.png",
		size: new google.maps.Size(25, 25),
		anchor: new google.maps.Point(5, 5)
	};

	var icoAnchor = new google.maps.Point(0, 0);
	
	return {image: icoImage, shadow: icoShadow, anchor: icoAnchor};
}

function GetArrowIcon()
{
	var icoImage = {
		url: "images/arrow.png",
		size: new google.maps.Size(39, 34),
		anchor: new google.maps.Point(10, 34)
	};

	var icoShadow = {
		url: "images/arrowshadow.png",
		size: new google.maps.Size(39, 34),
		anchor: new google.maps.Point(10, 34)
	};

	var icoAnchor = new google.maps.Point(0, -30);
	
	return {image: icoImage, shadow: icoShadow, anchor: icoAnchor};
}

function GetCircleIcon()
{
	var icoImage = {
		url: "images/circle-green.png",
		size: new google.maps.Size(20, 34),
		anchor: new google.maps.Point(10, 10)
	};

	var icoShadow = {
		url: "images/circle-green-shadow45.png",
		size: new google.maps.Size(37, 34),
		anchor: new google.maps.Point(10, 10)
	};

	var icoAnchor = new google.maps.Point(0, 0);
	
	return {image: icoImage, shadow: icoShadow, anchor: icoAnchor};
}

function GetStarIcon()
{
	var icoImage = {
		url: "images/favorite.png",
		size: new google.maps.Size(21, 21),
		anchor: new google.maps.Point(10, 10)
	};

	var icoShadow = {
		url: "images/favorite_s.png",
		size: new google.maps.Size(32, 21),
		anchor: new google.maps.Point(10, 10)
	};

	var icoAnchor = new google.maps.Point(0, 0);
	
	return {image: icoImage, shadow: icoShadow, anchor: icoAnchor};
}

function GetPlotIcon()
{
	var icoImage = {
		url: "images/dd-via.png",
		size: new google.maps.Size(11, 11),
		anchor: new google.maps.Point(5, 5)
	};

	var icoAnchor = new google.maps.Point(0, 0);
	
	return {image: icoImage, anchor: icoAnchor};
}

function CheckName(name)
{
	if(name && name === "") {
		name = null;
	}
	
	return name||tabLang.user;
}

function ExtractRoutePath(route)
{
	var routePoint = route.points();
	var routePath = new google.maps.MVCArray();
	for(var i = 0; i < routePoint.length(); i++)
	{
		var latlng = routePoint[i];
		routePath.push(new google.maps.LatLng(latlng.lat(), latlng.lng()));
	}

	return routePath;
}

var ICON_MARKER = GetMarkerIcon();
var ICON_PIN = GetPinIcon();
var ICON_ARROW = GetArrowIcon();
var ICON_CIRCLE = GetCircleIcon();
var ICON_STAR = GetStarIcon();
var ICON_PLOT = GetPlotIcon();

var PIN_TYPE_DEFAULT = 0;
var PIN_TYPE_START = 1;
var PIN_TYPE_PAUSE = 2;
var PIN_TYPE_END = 3;
var PIN_TYPE_MARKER = 4;
var PIN_TYPE_SEARCH = 5;
var PIN_TYPE_PROXIMITY = 6;
var PIN_TYPE_POI = 7;
var PIN_TYPE_FAVORITE = 8;
var PIN_TYPE_ROADBOOK = 9;

/* MarkerPushPin class, derived from GLocation */
function MarkerPushPin()
{
	GLocation.call(this, new google.maps.LatLng(0, 0), "", ICON_MARKER, true);

	this.addOverlay();
	this.hide();
}

MarkerPushPin.prototype = new GLocation();
MarkerPushPin.prototype.constructor = MarkerPushPin;

MarkerPushPin.prototype.onEndMove = function(latlng)
{
	this.setPosition(latlng);
	this.showInfo();
};

MarkerPushPin.prototype.addToRoute = function()
{
	GLocation.prototype.addToRoute.call(this);
	this.remove();
};
MarkerPushPin.prototype.addToFavorites = function()
{
	GLocation.prototype.addToFavorites.call(this);
	this.remove();
};
MarkerPushPin.prototype.setPosition = function(latlng)
{
	var geoLocation = geoCode(latlng);
	GLocation.prototype.setPosition.call(this, geoLocation);
	this.marker.setTitle(geoLocation.address);

	if(!this.marker.getVisible())
		this.marker.setVisible(true);
};
MarkerPushPin.prototype.remove = function()
{
	this.hide();
};

/* PolylinePushPin class, derived from GLocation */
function PolylinePushPin()
{
	GLocation.call(this, new google.maps.LatLng(0, 0), "", ICON_PLOT, true);
	
	this.addOverlay();
	this.hide();

	this.previewRoute = null;
	this.tab = null;
	this.item = null;
}

PolylinePushPin.prototype = new GLocation();
PolylinePushPin.prototype.constructor = PolylinePushPin;

PolylinePushPin.prototype.getInfoTitle = function()
{
	return '<table><tr><td><img class="icon" src="images/plot.jpg" width="21" height="21" /></td><td><div class="title">' + this.marker.getTitle() + '</div></td></tr></table>';
};
PolylinePushPin.prototype.getInfoExtra = function()
{
	return '<div class="advice">' + tabLang.MoveStep + '</div>';
};
PolylinePushPin.prototype.onMove = function(latlng)
{
	var _PolylinePushPin = this;
	
	GetRoutePreview(this.tab, this.item, latlng, false, function(ptPreviewRoute) {
		if(_PolylinePushPin) {
			_PolylinePushPin.delPreviewRoute();
			_PolylinePushPin.previewRoute = new google.maps.Polyline({
				map: map,
				path: ExtractRoutePath(ptPreviewRoute),
				strokeColor: ptPreviewRoute.color(),
				strokeOpacity: 0.5,
				strokeWidth: 5
			});
		}
	});
};
PolylinePushPin.prototype.onEndMove = function(latlng)
{
	this.setPosition(latlng);
	this.showInfo();
};

PolylinePushPin.prototype.addToRoute = function()
{
	InsertPoint(this.tab, this.item, this.marker.getPosition(), this.marker.getTitle());
	this.remove();
};
PolylinePushPin.prototype.addToFavorites = function()
{
	GLocation.prototype.addToFavorites.call(this);
	this.remove();
};
PolylinePushPin.prototype.setPolyline = function(tab, item, latlng)
{
	this.tab = tab;
	this.item = item;

	this.delPreviewRoute();
	this.setPosition(latlng);
	this.showInfo();
};
PolylinePushPin.prototype.setPosition = function(latlng)
{
	var geoLocation = geoCode(latlng);
	GLocation.prototype.setPosition.call(this, geoLocation);
	this.marker.setTitle(geoLocation.address);

	if(!this.marker.getVisible())
		this.marker.setVisible(true);
};
PolylinePushPin.prototype.remove = function()
{
	this.delPreviewRoute();
	this.hide();
};
PolylinePushPin.prototype.delPreviewRoute = function()
{
	if(this.previewRoute)
	{
		this.previewRoute.setMap(null);
		this.previewRoute = null;
	}
};

/* PushPin class, derived from GLocation */
function PushPin(tab, item, gpsPoint, iconPin, isDraggable)
{
	if(tab !== null && item !== null && gpsPoint)
	{
		this.tab = tab;
		this.item = item;

		GLocation.call(this, new google.maps.LatLng(gpsPoint.lat, gpsPoint.lng), CheckName(gpsPoint.address), iconPin, isDraggable);
		this.addOverlay();
	}
}

PushPin.prototype = new GLocation();
PushPin.prototype.constructor = PushPin;

PushPin.prototype.addToRoute = function()
{
	AddPoint(this.tab, this.item);
};
PushPin.prototype.addToFavorites = function()
{
	AddFavorite(this.tab, this.item);
};
PushPin.prototype.sendToGps = function(gps)
{
	SendToGps(this.tab, this.item, gps);
};
PushPin.prototype.onClick = function(latlng)
{
	SelectPoint(this.tab, this.item);
};
PushPin.prototype.onEndMove = function(latlng)
{
	MovePoint(this.tab, this.item, geoCode(latlng));
};
PushPin.prototype.getInfoExtra = function()
{
	var snippet = GetPoint(this.tab, this.item).snippet;
	return snippet?('<div class="snippet">' + snippet + '</div>'):("");
};
PushPin.prototype.getInfoDel = function()
{
	return '<br><a href="javascript:tmpLocation.setRemove();"><img class="icon" src="images/del.jpg" width="12" height="12" />' + tabLang.del + '</a>';
};
PushPin.prototype.setName = function(name)
{
	RenamePoint(this.tab, this.item, name);
};
PushPin.prototype.setRemove = function()
{
	DeletePoint(this.tab, this.item);
};
PushPin.prototype.Refresh = function(tab, item)
{
	this.tab = tab;
	this.item = item;

	var gpsPoint = GetPoint(this.tab, this.item);
	this.marker.setTitle(CheckName(gpsPoint.address));

	var gpsPos = new google.maps.LatLng(gpsPoint.lat, gpsPoint.lng);
	if(gpsPos != this.marker.getPosition()) {
		this.setPosition(gpsPos);
	}
};

/* FavoritePushPin class, derived from PushPin */
function FavoritePushPin(tab, item, gpsPoint)
{
	PushPin.call(this, tab, item, gpsPoint, ICON_STAR, false);
	this.changeName = false;
}

FavoritePushPin.prototype = new PushPin();
FavoritePushPin.prototype.constructor = FavoritePushPin;

FavoritePushPin.prototype.getInfoTitle = function()
{
	var address = CheckName(GetPoint(this.tab, this.item).address);
	var details = '<table><tr><td><img class="icon" src="images/favorite.png" width="21" height="21" /></td><td>';
	
	if(!this.changeName) {
		details += '<a href="javascript:tmpLocation.editName(true);"><div class="title">' +  address + '</div></a>';
	} else {
		details += '<FORM action="javascript:void(0)" onSubmit="tmpLocation.setName(this.txtName.value)"><INPUT type="text" id="txtName" class="EditName" name="txtName" value="' +  address + '"><INPUT type="submit" class="ButtonOk" value="" title="' + tabLang.valid + '"/><INPUT type="button" class="ButtonCancel" onClick="tmpLocation.editName(false)" title="' + tabLang.cancel + '"/></FORM>';
	}
	details += '</td></tr></table>';

	return details;
};
FavoritePushPin.prototype.getInfoAdd = function()
{
	return '<br><a href="javascript:tmpLocation.addToRoute()"><img class="icon" src="images/add.jpg" width="12" height="12" />' + tabLang.add + '</a>';
};
FavoritePushPin.prototype.editName = function(bEdit)
{
	this.changeName = bEdit;
	PushPin.prototype.showInfo.call(this);
	this.changeName = false;
};

/* ProximityPushPin class, derived from PushPin */
function ProximityPushPin(tab, item, gpsPoint)
{
	PushPin.call(this, tab, item, gpsPoint, ICON_CIRCLE, false);
}

ProximityPushPin.prototype = new PushPin();
ProximityPushPin.prototype.constructor = ProximityPushPin;

ProximityPushPin.prototype.getInfoTitle = function()
{
	return '<table><tr><td><a href="http://www.google.com/cse?cx=partner-pub-4881128809765041%3Aspp08h-sqhq&q=' + this.marker.getTitle() + '"><img class="icon" src="images/search.jpg" width="21" height="21" alt="Google Search" /></a></td><td><div class="title">' + this.marker.getTitle() + '</div></td></tr></table>';
};

/* SearchPushPin class, derived from PushPin */
function SearchPushPin(tab, item, gpsPoint)
{
	PushPin.call(this, tab, item, gpsPoint, ICON_ARROW, false);
}

SearchPushPin.prototype = new PushPin();
SearchPushPin.prototype.constructor = SearchPushPin;

SearchPushPin.prototype.getInfoTitle = function()
{
	return '<table><tr><td><a href="http://www.google.com/cse?cx=partner-pub-4881128809765041%3Aspp08h-sqhq&q=' + this.marker.getTitle() + '"><img class="icon" src="images/search.jpg" width="21" height="21" alt="Google Search" /></a></td><td><div class="title">' + this.marker.getTitle() + '</div></td></tr></table>';
};

function NewPushPin(tab, item)
{
	var gpsPoint = GetPoint(tab, item);
	
	switch(gpsPoint.type)
	{
	case PIN_TYPE_START:
	case PIN_TYPE_PAUSE:
	case PIN_TYPE_END:
		return new RoutePushPin(tab, item, gpsPoint);

	case PIN_TYPE_SEARCH:
		return new SearchPushPin(tab, item, gpsPoint);

	case PIN_TYPE_PROXIMITY:
		return new ProximityPushPin(tab, item, gpsPoint);

	case PIN_TYPE_FAVORITE:
		return new FavoritePushPin(tab, item, gpsPoint);

	default:
		return null;
	}
}
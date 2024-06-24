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

var tmpLocation = null;
var _global_zIndex = -2000000000;

var _DEF_ZOOM_IN = 15;
var _refZoomOut = 6;

function SetZoomLabel()
{
	var zoomLabel = document.getElementById("zoomLabel");

	if(zoomLabel)
	{
		if(map.getZoom() >= _DEF_ZOOM_IN) {
			zoomLabel.innerHTML = tabLang.ZoomOut;
		}
		else {
			zoomLabel.innerHTML = tabLang.ZoomIn;
		}
	}
}

function GLocation(latlng, name, icon, isDraggable)
{
	if(latlng)
	{
		var location = this;
		var markerOptions = {position: latlng, title: name, icon: icon.image, draggable: isDraggable, zIndex: _global_zIndex};
		if(icon.shadow) {
			markerOptions.shadow = icon.shadow;
		}
		if(icon.anchor) {
			markerOptions.anchorPoint = icon.anchor;
		} else {
			markerOptions.anchorPoint = new google.maps.Point(0, 0);
		}

		this.marker = new google.maps.Marker(markerOptions);

		_global_zIndex++;

		google.maps.event.addListener(this.marker, "click", function(event) {
			location.onClick(event.latLng);
		});

		google.maps.event.addListener(this.marker, "dragstart", function(event) {
			CloseInfoWindow();
		});

		google.maps.event.addListener(this.marker, "drag", function(event) {
			location.onMove(event.latLng);
		});

		google.maps.event.addListener(this.marker, "dragend", function(event) {
			location.onEndMove(event.latLng);
		});
	}
}

GLocation.prototype.onClick = function(latlng)
{
	this.showInfo();
};
GLocation.prototype.onMove = function(latlng)
{
};
GLocation.prototype.onEndMove = function(latlng)
{
};

GLocation.prototype.addOverlay = function()
{
	this.marker.setMap(map);
};
GLocation.prototype.removeOverlay = function()
{
	this.marker.setMap(null);
};

GLocation.prototype.setPosition = function(latlng)
{
	this.marker.setPosition(latlng);
};
GLocation.prototype.hide = function()
{
	if(this.marker)
	{
		CloseInfoWindow();
		this.marker.setVisible(false);
	}
};
GLocation.prototype.showInfo = function(maxWidth)
{
	tmpLocation = this;
	var details = '<div oncontextmenu="return false;" class="InfoWindow">' + this.getInfoTitle() + this.getInfoExtra() + '<div class="PushPinInfo">' + this.getInfoTarget() + this.getInfoAdd() + this.getInfoSendToGps() + this.getInfoStreetView() + this.getInfoDel() + this.getInfoZoom() + '</div>' + this.getInfoFooter() + '</div>';

	this.bringToFront();

	if(!maxWidth)
		maxWidth = 400;

	if(maxWidth)
		_global_infowindow.setOptions({maxWidth: maxWidth});

	_global_infowindow.setContent(details);
	_global_infowindow.open(this.marker.get('map'), this.marker);
};
GLocation.prototype.Zoom = function()
{
	var newZoom = _DEF_ZOOM_IN;

	if(newZoom > 20)
		newZoom = 20;

	if(map.getZoom() >= newZoom) { // Zoom Out
		newZoom = _refZoomOut;
	}
	else { // Zoom In
		_refZoomOut = map.getZoom();
	}

	map.setCenter(this.marker.getPosition());
	map.setZoom(newZoom);
};
GLocation.prototype.bringToFront = function()
{
	_global_zIndex += 2;
	this.marker.setZIndex(_global_zIndex);
	return _global_zIndex;
};
GLocation.prototype.remove = function()
{
	if(this.marker)
	{
		CloseInfoWindow();
		this.removeOverlay();
		this.marker = null;
	}
};
GLocation.prototype.addToRoute = function()
{
	AddPoint(this.marker.getPosition(), this.marker.getTitle());
};
GLocation.prototype.addToFavorites = function()
{
	AddFavorite(this.marker.getPosition(), this.marker.getTitle());
};
GLocation.prototype.sendToGps = function(gps)
{
	SendToGps(gps, this.marker.getPosition(), this.marker.getTitle());
};
GLocation.prototype.StreetView = function()
{
	new GStreetView(document.getElementById('map-canvas'), this.marker.getPosition(), true);
};
GLocation.prototype.getInfoTitle = function()
{
	return'<table><tr><td><img class="icon" src="images/pin.jpg" width="21" height="21" /></td><td><div class="title">' + this.marker.getTitle() + '</div></td></tr></table>';
};
GLocation.prototype.getInfoExtra = function()
{
	return "";
};
GLocation.prototype.getInfoTarget = function()
{
	try {
		return '<img class="icon" src="images/target.jpg" width="12" height="12" />' + GetStrCoords(this.marker.getPosition());
	}
	catch(ex){
		return '<img class="icon" src="images/target.jpg" width="12" height="12" />' + this.marker.getPosition().toUrlValue();
	}
};
GLocation.prototype.getInfoAdd = function()
{
	return '<br><a href="javascript:tmpLocation.addToRoute()"><img class="icon" src="images/add.jpg" width="12" height="12" />' + tabLang.add + '</a><br><a href="javascript:tmpLocation.addToFavorites()"><img class="icon" src="images/favorite.jpg" width="12" height="12" />' + tabLang.fav + '</a>';
};
GLocation.prototype.getInfoSendToGps = function()
{
	var details = "";

	if(tabLang.tthome) {
		details += '<br><a href="javascript:tmpLocation.sendToGps(0);"><img class="icon" src="images/tthome.jpg" width="12" height="12" />' + tabLang.tthome + '</a>';
	}

	if(tabLang.nvgfresh) {
		details += '<br><a href="javascript:tmpLocation.sendToGps(1);"><img class="icon" src="images/nvgfresh.jpg" width="12" height="12" />' + tabLang.nvgfresh + '</a>';
	}

	if(tabLang.navgate) {
		details += '<br><a href="javascript:tmpLocation.sendToGps(2);"><img class="icon" src="images/navgate.jpg" width="12" height="12" />' + tabLang.navgate + '</a>';
	}

	if(tabLang.miomore) {
		details += '<br><a href="javascript:tmpLocation.sendToGps(3);"><img class="icon" src="images/navman.jpg" width="12" height="12" />' + tabLang.miomore + '</a>';
	}

	if(tabLang.garmin) {
		details += '<br><a href="javascript:tmpLocation.sendToGps(4);"><img class="icon" src="images/garmin.jpg" width="12" height="12" />' + tabLang.garmin + '</a>';
	}

	return details;
};
GLocation.prototype.getInfoStreetView = function()
{
	if (use_google === true)
	{
		try
		{
			var location = GetStreetViewLocation(this.marker.getPosition());
			if(location && (location.lat() !== 0 || location.lng() !== 0)) {
				return '<br><a href="javascript:tmpLocation.StreetView();"><img class="icon" src="images/streetview.jpg" width="12" height="12" />' + tabLang.streetview + '</a>';
			}
		}
		catch(ex)
		{
			Trace('GetStreetViewLocation', ex);
		}
	}

	return "";
};
GLocation.prototype.getInfoDel = function()
{
	return '<br><a href="javascript:tmpLocation.remove();"><img class="icon" src="images/del.jpg" width="12" height="12" />' + tabLang.del + '</a>';
};
GLocation.prototype.getInfoZoom = function()
{
	var zoomText = tabLang.ZoomIn;
	if(map.getZoom() >= _DEF_ZOOM_IN) {
		zoomText = tabLang.ZoomOut;
	}

	return '<br><a href="javascript:tmpLocation.Zoom();"><img class="icon" src="images/zoom.jpg" width="12" height="12" /><div id="zoomLabel" style="display:inline;">' + zoomText + '</div></a>';
};
GLocation.prototype.getInfoFooter = function()
{
	return "";
};

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
var _displayNumber = true;

function GetRoutePushPinColor(pushpin)
{
	switch(pushpin)
	{
	case PIN_TYPE_START:
		return 'green';

	case PIN_TYPE_PAUSE:
		return 'yellow';

	case PIN_TYPE_END:
		return 'red';

	default:
		return '';
	}
}

/* RoutePushPin class, derived from PushPin */
function RoutePushPin(tab, item, gpsPoint)
{
	if(tab !== null && item !== null && gpsPoint)
	{
		this.previewRoute = null;
		this.route = null;
		this.changeName = false;
		this.color = GetRoutePushPinColor(gpsPoint.type);

		PushPin.call(this, tab, item, gpsPoint, ICON_PIN, true);

		if(_displayNumber) {
			this.number = new GText(this.marker.getPosition(), this.item + 1, 'pin', new google.maps.Size(5, 20), this.color, _global_zIndex);
		}
		else {
			this.number = null;
		}
		this.addRoute();
	}
}

RoutePushPin.prototype = new PushPin();
RoutePushPin.prototype.constructor = RoutePushPin;

RoutePushPin.prototype.onMove = function(latlng)
{
	if(this.number) {
		this.number.move(latlng);
	}

	this.moving = true;
	var _RoutePushPin = this;

	GetRoutePreview(this.tab, this.item, latlng, true, function(ptPreviewRoute) {
		_RoutePushPin.delPreviewRoute();
		if(_RoutePushPin.moving) {
			_RoutePushPin.previewRoute = new google.maps.Polyline({
				map: map,
				path: ExtractRoutePath(ptPreviewRoute),
				strokeColor: ptPreviewRoute.color(),
				strokeOpacity: 0.5,
				strokeWidth: 5
			});
		}
	});
};

RoutePushPin.prototype.onEndMove = function(latlng)
{
	PushPin.prototype.onEndMove.call(this, latlng);
	this.moving = false;
	this.delPreviewRoute();
};

RoutePushPin.prototype.removeOverlay = function()
{
	PushPin.prototype.removeOverlay.call(this);
	if(this.number)
	{
		this.number.remove();
		this.number = null;
	}
};
RoutePushPin.prototype.delPreviewRoute = function()
{
	if(this.previewRoute)
	{
		this.previewRoute.setMap(null);
		this.previewRoute = null;
	}
};
RoutePushPin.prototype.Refresh = function(tab, item)
{
	PushPin.prototype.Refresh.call(this, tab, item);
	var gpsPoint = GetPoint(this.tab, this.item);

	this.color = GetRoutePushPinColor(gpsPoint.type);

	if(this.number) {
		this.number.update(this.marker.getPosition(), this.item + 1, this.color);
	}

	if(!gpsPoint.route) {
		this.delRoute();
	}
};

RoutePushPin.prototype.getInfoTitle = function()
{
	var gpsPoint = GetPoint(this.tab, this.item);
	var address = CheckName(gpsPoint.address);
	var details = '<table><tr><td><div class="info" id="' + this.color + '">' + (this.item + 1) + '</div></td><td>';

	if(!this.changeName) {
		details += '<a href="javascript:tmpLocation.editName(true);"><div class="title">' +  address + '</div></a>';
	} else {
		details += '<FORM action="javascript:void(0)" onSubmit="tmpLocation.setName(this.txtName.value)"><INPUT type="text" id="txtName" class="EditName" name="txtName" value="' +  address + '"><INPUT type="submit" class="ButtonOk" value="" title="' + tabLang.valid + '"/><INPUT type="button" class="ButtonCancel" onClick="tmpLocation.editName(false)" title="' + tabLang.cancel + '"/><INPUT type="button" class="ButtonRefresh" onClick="document.getElementById(\'txtName\').value = CheckName(geoCode(new google.maps.LatLng(' + gpsPoint.lat + ', ' + gpsPoint.lng + ')).address)" title="' + tabLang.refresh + '"/></FORM>';
	}
	details += '</td></tr></table>';

	return details;
};
RoutePushPin.prototype.getInfoAdd = function()
{
	var route = GetPoint(this.tab, this.item).route;
	var details = "";

	if(this.route && route)
	{
		if(route.distance) {
			details += '<br><img class="icon" src="images/distance.jpg" width="12" height="12" />' + route.distance;
		}

		if(route.duration) {
			details += '<br><img class="icon" src="images/duration.jpg" width="12" height="12" />' + route.duration;
		}
	}
	else if(this.item > 0)
	{
		details += '<br><a href="javascript:tmpLocation.calculate()"><img class="icon" src="images/distance.jpg" width="12" height="12" />' + tabLang.calc + '</a>';
	}

	details += '<br><a href="javascript:tmpLocation.addToFavorites()"><img class="icon" src="images/favorite.jpg" width="12" height="12" />' + tabLang.fav + '</a>';
	return details;
};
RoutePushPin.prototype.editName = function(bEdit)
{
	this.changeName = bEdit;
	PushPin.prototype.showInfo.call(this);
	this.changeName = false;
};
RoutePushPin.prototype.bringToFront = function()
{
	var zId = PushPin.prototype.bringToFront.call(this);
	if(this.number) {
		this.number.zIndex(zId);
	}
};
RoutePushPin.prototype.remove = function()
{
	this.delRoute();
	PushPin.prototype.remove.call(this);
};
RoutePushPin.prototype.addRoute = function()
{
	this.delRoute();
	var ptRoute = GetPoint(this.tab, this.item).route;
	if(ptRoute)
	{
		this.route = new google.maps.Polyline({
			map: map,
			path: ExtractRoutePath(ptRoute),
			strokeColor: ptRoute.color(),
			strokeOpacity: 0.5,
			strokeWidth: 5
		});

		var lRoute = this;
		google.maps.event.addListener(this.route, "click", function(event)
		{
			if(Settings().AutoAddStep())
			{
				var geoLocation = geoCode(event.latLng);
				InsertPoint(lRoute.tab, lRoute.item, geoLocation, geoLocation.address);
			}
			else
			{
				gPolylineMarker.setPolyline(lRoute.tab, lRoute.item, event.latLng);
			}
		});

		google.maps.event.addListener(this.route, "mouseover", function(event) {
		});
	}
};
RoutePushPin.prototype.delRoute = function()
{
	if(this.route)
	{
		gPolylineMarker.remove();

		this.route.setMap(null);
		this.route = null;
	}
};
RoutePushPin.prototype.calculate = function()
{
	Calculate(this.tab, this.item);
};

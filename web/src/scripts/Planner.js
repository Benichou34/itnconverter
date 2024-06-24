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

var map = null;
var _global_infowindow = null;

function degreesToRadians(deg) {
	return deg * (Math.PI / 180);
}

function radiansToDegrees(rad) {
	return rad / (Math.PI / 180);
}

LoadScript("scripts/external/markermanager.js");
LoadScript("scripts/external/json2.js");
LoadScript("scripts/Language.js");
LoadScript("scripts/Glocation.js");
LoadScript("scripts/GButton.js");
LoadScript("scripts/Display.js");
LoadScript("scripts/GStreetView.js");
LoadScript("scripts/GNumber.js");
LoadScript("scripts/GPushPin.js");
LoadScript("scripts/GMapType.js");
LoadScript("scripts/Copyright.js");
LoadScript("scripts/RoutePushPin.js");
LoadScript("scripts/GZoomControl.js");
LoadScript("scripts/GMapProvider.js");

LoadScript("scripts/map/VEMap.js");
LoadScript("scripts/map/VMMap.js");
LoadScript("scripts/map/OSMap.js");
LoadScript("scripts/map/OTMap.js");
LoadScript("scripts/map/TTMap.js");
LoadScript("scripts/map/GPMap.js");
LoadScript("scripts/map/AUMap.js");
LoadScript("scripts/map/JPMap.js");
LoadScript("scripts/map/NOMap.js");
LoadScript("scripts/map/NRCANMap.js");
LoadScript("scripts/map/NZMap.js");
LoadScript("scripts/map/ArcGISMap.js");
LoadScript("scripts/map/USTopoMap.js");
LoadScript("scripts/map/WZMap.js");
LoadScript("scripts/map/GDLMap.js");
LoadScript("scripts/map/HereMap.js");
LoadScript("scripts/map/Mappy.js");
LoadScript("scripts/map/IDEEMap.js");
LoadScript("scripts/map/TFMap.js");

var gTmpMarker = null;
var gPolylineMarker = null;
var tabPin = [];

function ShowUpdateWindow()
{
	if(map)	{
		_global_infowindow.setContent('<p class="UpdateTitle">' + tabLang.updateTitle + ' (' + _VERSION + ').<br>' + tabLang.downloadNow + '</p><p class="UpdateURL"><a href="' + _SITE_URL + '">' + _SITE_URL + '</a></p>');
		_global_infowindow.setPosition(map.getCenter());
		_global_infowindow.open(map);
	}
}

function AutoViewMap()
{
	if(tabPin.length && tabPin[0].length > 0)
	{
		CloseInfoWindow();
		if(tabPin[0].length > 1)
		{
			var bounds = new google.maps.LatLngBounds();
			for(var i in tabPin[0])
			{
				if(tabPin[0][i]) {
					bounds.extend(tabPin[0][i].marker.getPosition());
				}
			}

			map.fitBounds(bounds);
			map.setCenter(bounds.getCenter());
		}
		else
		{
			map.setCenter(tabPin[0][0].marker.getPosition(), 15);
		}
	}
}

function CenterMap(lat, lng, zoom)
{
	if(zoom) {
		map.setZoom(zoom);
	}

	map.panTo(new google.maps.LatLng(lat, lng));
}

function MapClickEnabled(flag)
{
	if(flag === true)
	{
		google.maps.event.addListener(map, "click", function(event)
		{
			gTmpMarker.setPosition(event.latLng);
			gTmpMarker.showInfo();
		});
	}
	else
	{
		google.maps.event.clearListeners(map, "click");
	}
}

function InitMap(jsOptions)
{
	var jsParam = JSON.parse(jsOptions);

	try
	{
		var mapOptions = {
			center: new google.maps.LatLng(47.576526, 9.140625),
			zoom: 4,
			disableDefaultUI: true,
			mapTypeControl: false,
			scaleControl: true,
			keyboardShortcuts: true,
			streetViewControl: use_google,
			streetViewControlOptions: {
				position: google.maps.ControlPosition.TOP_LEFT
			},
			zoomControl: false,
			rotateControl: false,
			fullscreenControl: false,
			mapTypeId: google.maps.MapTypeId.ROADMAP
		};

		var mpTypes = [];
		var country = "";
		var mapIndex = -1;

		// Extraction des parametres
		if(jsParam)
		{
			if(jsParam.lang)
				SetMapLanguage(jsParam.lang);

			if(jsParam.center)
			{
				mapOptions.center = new google.maps.LatLng(jsParam.center.lat, jsParam.center.lng);
				mapOptions.zoom = 6;
			}

			if(jsParam.country) {
				country = jsParam.country;
			}

			if(jsParam.number === false) {
				_displayNumber = jsParam.number;
			}
		}

		map = new google.maps.Map(document.getElementById("map-canvas"), mapOptions);
		map.factor = [];

		for(var f = 0; f < 30; f++) {
			map.factor[f] = Math.pow(2, f);
		}

		if(jsParam && jsParam.maps)
		{
			var defaultFlag = 'OS';
			var defaultType = _TYPE_ROUTE_MAP;

			var paramFlag = defaultFlag;
			var paramType = defaultType;

			if(jsParam.map) {
				paramFlag = jsParam.map.flag;
				paramType = jsParam.map.type;
			}

			for(var i in jsParam.maps)
			{
				var jsMap = jsParam.maps[i];

				if(refMap[jsMap.flag])
				{
					var newMap = null;

					if(jsMap.map)
					{
						newMap = refMap[jsMap.flag].object(_TYPE_ROUTE_MAP);
						if(newMap) {
							newMap.flag = jsMap.flag;
							newMap.type = _TYPE_ROUTE_MAP;
							map.mapTypes.set(jsMap.flag + '_route', newMap);
							mpTypes.push(jsMap.flag + '_route');

							if((newMap.flag == paramFlag && newMap.type == paramType) || (mapIndex == -1 && newMap.flag == defaultFlag && newMap.type == defaultType)) {
								mapIndex = mpTypes.length - 1;
							}
						}
					}

					if(jsMap.sat)
					{
						newMap = refMap[jsMap.flag].object(_TYPE_AERIAL_MAP);
						if(newMap) {
							newMap.flag = jsMap.flag;
							newMap.type = _TYPE_AERIAL_MAP;
							map.mapTypes.set(jsMap.flag + '_aerial', newMap);
							mpTypes.push(jsMap.flag + '_aerial');

							if((newMap.flag == paramFlag && newMap.type == paramType) || (mapIndex == -1 && newMap.flag == defaultFlag && newMap.type == defaultType)) {
								mapIndex = mpTypes.length - 1;
							}
						}
					}

					if(jsMap.hybrid)
					{
						newMap = refMap[jsMap.flag].object(_TYPE_HYBRID_MAP);
						if(newMap) {
							newMap.flag = jsMap.flag;
							newMap.type = _TYPE_HYBRID_MAP;
							map.mapTypes.set(jsMap.flag + '_hybrid', newMap);
							mpTypes.push(jsMap.flag + '_hybrid');

							if((newMap.flag == paramFlag && newMap.type == paramType) || (mapIndex == -1 && newMap.flag == defaultFlag && newMap.type == defaultType)) {
								mapIndex = mpTypes.length - 1;
							}
						}
					}

					if(jsMap.topo)
					{
						newMap = refMap[jsMap.flag].object(_TYPE_PHYSICAL_MAP);
						if(newMap) {
							newMap.flag = jsMap.flag;
							newMap.type = _TYPE_PHYSICAL_MAP;
							map.mapTypes.set(jsMap.flag + '_terrain', newMap);
							mpTypes.push(jsMap.flag + '_terrain');

							if((newMap.flag == paramFlag && newMap.type == paramType) || (mapIndex == -1 && newMap.flag == defaultFlag && newMap.type == defaultType)) {
								mapIndex = mpTypes.length - 1;
							}
						}
					}
				}
			}
		}

		AddZoomControl(map);
		AddCopyrightControl(map);

		if(mpTypes.length > 0)
		{
			if (mapIndex == -1)
				mapIndex = 0;
			AddMapsControl(mpTypes, mapIndex);
		}
		else
		{
			map.setMapTypeId(google.maps.MapTypeId.ROADMAP);
		}

//		AddExtrasControl(country);
		AddTraficControl(country);

		_global_infowindow = new google.maps.InfoWindow();
		gTmpMarker = new MarkerPushPin();
		gPolylineMarker = new PolylinePushPin();

		google.maps.event.addListener(map, "zoom_changed", SetZoomLabel);

		Signal('I', "");
	}
	catch(ex)
	{
		ReportError(-1, -1, 'InitMap', ex);
	}
}

function initialize()
{
	Signal('L', '{"software":{"version":' + _VERSION + ',"url":"' + _SITE_URL + '"}}');
}

function NewPinList(tab)
{
	tabPin[tab] = [];
}

function CloseInfoWindow()
{
	_global_infowindow.close();
}

function ClearRoutes(tab)
{
	if(tabPin.length)
	{
		for(var i in tabPin[tab]) {
			tabPin[tab][i].delRoute();
		}
	}
}

function AddRoute(tab, item)
{
	try
	{
		tabPin[tab][item].addRoute();
	}
	catch(ex)
	{
		ReportError(tab, item, 'AddRoute', ex);
	}
}

function ViewPushPin(tab, item)
{
	try
	{
		map.panTo(tabPin[tab][item].marker.getPosition());
		tabPin[tab][item].showInfo();
	}
	catch(ex)
	{
		ReportError(tab, item, 'ViewPushPin', ex);
	}
}

function DelPushPin(tab, item)
{
	try
	{
		tabPin[tab][item].remove();
		tabPin[tab][item] = null;
		tabPin[tab].splice(item, 1);
	}
	catch(ex)
	{
		ReportError(tab, item, 'DelPushPin', ex);
	}
}

function InsertPushPin(tab, item)
{
	try
	{
		tabPin[tab].splice(item, 0, NewPushPin(tab, item));
	}
	catch(ex)
	{
		ReportError(tab, item, 'InsertPushPin', ex);
	}
}

function RefreshPushPins(tab, item, counter)
{
	try
	{
		for(var i=item;i<item+counter;i++) {
			tabPin[tab][i].Refresh(tab, i);
		}
	}
	catch(ex)
	{
		ReportError(tab, item, 'RefreshPushPins', ex);
	}
}

function ClearPushPins(tab)
{
	try
	{
		for(var i in tabPin[tab])
		{
			tabPin[tab][i].remove();
			tabPin[tab][i] = null;
		}

		tabPin[tab].length = 0;
	}
	catch(ex)
	{
		ReportError(tab, -1, 'ClearPushPins', ex);
	}
}

google.maps.event.addDomListener(window, 'load', initialize);

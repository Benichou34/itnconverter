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

function Signal(ins, msg)
{
	window.location = "signal:" + ins + msg;
}

function Trace(func, msg)
{
	window.external.Trace(func + ', ' + msg);
}

function ReportError(tab, item, func, err)
{
	window.external.MessageBox(tab + ',' + item + ',' + func + ',' + err.name + ',' + err.message);
}

function geoCodeName(point)
{
	try	{
		var location = window.external.GetLocation(point);
		if(location) {
			return (location.address() || tabLang.user);
		}
	}
	catch(ex) {
		Trace('geoCodeName', ex);
	}

	return tabLang.user;
}

function geoCodeRoute(point)
{
	var geoRoute = point;
	geoRoute.address = null;

	try	{
		var location = window.external.GetRoadLocation(point);
		if(location)
		{
			geoRoute = new google.maps.LatLng(location.lat(), location.lng());
			geoRoute.address = location.address() || tabLang.user;
		}
	}
	catch(ex) {
		Trace('geoCodeRoute', ex);
	}

	return geoRoute;
}

function geoCode(point)
{
	var location = window.external.ReverseGeocoding(point);
	var geoRoute = new google.maps.LatLng(location.lat(), location.lng());
	geoRoute.address = location.address() || tabLang.user;
	return geoRoute;
}

function HttpRelay(query, host, referrer)
{
	var relayQuery = 'http://benitools.info/relay.php?';

	if(host) {
		relayQuery += 'host=' + host + '&';
	}

	if(referrer) {
		relayQuery += 'ref=' + referrer + '&';
	}

	return relayQuery + 'url=' + query;
}

function HttpRequest(query, options)
{
	if(options)
		return window.external.HttpRequest(query, options);
	else
		return window.external.HttpRequest(query);
}

var Utf8 = {};
Utf8.Encode = function(input)
{
	return window.external.Utf8.Encode(input);
};
Utf8.Decode = function(input)
{
	return window.external.Utf8.Decode(input);
};

var Base64 = {};
Base64.Encode = function(input)
{
	return window.external.Base64.Encode(input);
};
Base64.Decode = function(input)
{
	return window.external.Base64.Decode(input);
};

function InsertPoint(tab, item, latlng, name)
{
	window.external.InsertPoint(tab, item, latlng, name);
}

function AddPoint(tab, item)
{
	window.external.AddPoint(tab, item);
}

function AddFavorite(tab, item)
{
	window.external.AddFavorite(tab, item);
}

function SendToGps(tab, item, gps)
{
	window.external.SendToGps(tab, item, gps);
}

function SelectPoint(tab, item)
{
	window.external.SelectPoint(tab, item);
}

function MovePoint(tab, item, latlng)
{
	window.external.MovePoint(tab, item, geoCode(latlng));
}

function RenamePoint(tab, item, name)
{
	window.external.RenamePoint(tab, item, name);
}

function DeletePoint(tab, item)
{
	window.external.DeletePoint(tab, item);
}

function GetPoint(tab, item)
{
	return window.external.GetPoint(tab, item);
}

function GetStrCoords(latlng)
{
	return window.external.GetStrCoords(latlng);
}

function GetStreetViewLocation(latlng)
{
	return window.external.GetStreetViewLocation(latlng);
}

function GetRoutePreview(tab, item, latlng, step, callback)
{
	window.external.GetRoutePreview(tab, item, latlng, step, callback);
}

function Calculate(tab, item)
{
	window.external.Calculate(tab, item);
}

function Settings()
{
	return window.external.Settings();
}

function SetCurrentMap(flag, type)
{
	window.external.SetCurrentMap(flag, type);
}

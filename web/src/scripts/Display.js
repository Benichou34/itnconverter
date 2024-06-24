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

var _TYPE_ROUTE_MAP = 0;
var _TYPE_AERIAL_MAP = 1;
var _TYPE_HYBRID_MAP = 2;
var _TYPE_PHYSICAL_MAP = 3;

LoadScript("scripts/GCheckbox.js");
LoadScript("scripts/GPoiCheckbox.js");
LoadScript("scripts/GTraficCheckbox.js");
LoadScript("scripts/SpeedCam.js");
LoadScript("scripts/vtraffic/poi.js");
LoadScript("scripts/vtraffic/trafic.js");
LoadScript("scripts/vtraffic/events.js");
LoadScript("scripts/vtraffic/gas.js");
LoadScript("scripts/vtraffic/parking.js");
LoadScript("scripts/vtraffic/meteo.js");

var scExtrasCountry = ["AT","AD","BE","BG","BA","BR","CA","CH","CY","CZ","DE","DK","ES","FI","FR","GB","GR","HU","HR","IT","IL","IE","IS","KW","LU","LI","LT","MT","MA","NL","NO","NZ","OM","PT","PL","RO","RU","SE","SK","SI","RS","TN","TR","UA","AE","US","ZA"];

function getLanguage()
{
	return (navigator.language || navigator.userLanguage).substr(0, 2);
}

function selectMap(mapId)
{
	// Hack for Google Maps
	var mapType = map.mapTypes.get(mapId);
	if(mapType && mapType.maxZoom === -1)
	{
		switch(mapType.type)
		{
		case _TYPE_ROUTE_MAP:
			map.setMapTypeId(google.maps.MapTypeId.ROADMAP);
			break;

		case _TYPE_AERIAL_MAP:
			map.setMapTypeId(google.maps.MapTypeId.SATELLITE);
			break;

		case _TYPE_HYBRID_MAP:
			map.setMapTypeId(google.maps.MapTypeId.HYBRID);
			break;

		case _TYPE_PHYSICAL_MAP:
			map.setMapTypeId(google.maps.MapTypeId.TERRAIN);
			break;

		default:
			map.setMapTypeId(google.maps.MapTypeId.ROADMAP);
			break;
		}
	}
	else {
		map.setMapTypeId(mapId);
	}

	try	{
		SetCurrentMap(mapType.flag, mapType.type);
	}
	catch(ex) {
		Trace('selectMap', ex);
	}
}

function GListControl(text, title, className, list)
{
	if(!text)
		return null;

	this.text = text;
	this.title = title;
	this.className = className;
	this.list = list;

	var tbCb = this.list;
	var bt = new GButton(this.text, this.title);
	var opt = document.createElement("div");
	opt.className = this.className;

	for(var i in tbCb)
	{
		tbCb[i].getContainer().style.top = ((19*i) + 1) + "px";
		opt.appendChild(tbCb[i].getContainer());
		opt.style.height = ((19*i) + 25) + "px";
	}

	bt.onClick(function() {
		var k = 0;

		for(var i in tbCb)
		{
			if(tbCb[i].isChecked()) {
				k++;
			}
		}

		if(!k)
		{
			bt.setText(text);
			bt.setPushed(false);
		}
		else
		{
			bt.setPushed(true);
			bt.setText(text + " ("+ k + ")");
		}
	});

	var tmo = null;

	bt.onMouseOver(function() {
		clearTimeout(tmo);
		opt.style.display = "inline";
	});

	bt.onMouseOut(function() {
		tmo = setTimeout(function(){
			opt.style.display = "none";
		}, 500);
	});

	var container = bt.getContainer();
	container.appendChild(opt);

	var controlDiv = document.createElement('div');
	controlDiv.style.padding = '5px';
	controlDiv.appendChild(container);
	return controlDiv;
}

var sListComboBox = [];

function GComboBoxControl(id, className, list, selected)
{
	this.id = id;
	this.className = className;

	sListComboBox[this.id] = list;

	var innerHTML = "<select id='" + this.id + "' onChange='onComboChange(this)'>";
	var background;
	var typeName;

	for(var i in list)
	{
		var mapType = map.mapTypes.get(list[i]);

		switch(mapType.type)
		{
		case _TYPE_ROUTE_MAP:
			background = "#FFCDA7";
			typeName = tabLang.RouteMap;
			break;

		case _TYPE_AERIAL_MAP:
			background = "#7FC9FF";
			typeName = tabLang.SatMap;
			break;

		case _TYPE_HYBRID_MAP:
			background = "#7FFFFF";
			typeName = tabLang.HybridMap;
			break;

		case _TYPE_PHYSICAL_MAP:
			background = "#A5FF7F";
			typeName = tabLang.TopoMap;
			break;

		default:
			background = "#FFFFFF";
			typeName = mapType.type;
			break;
		}

		innerHTML += "<option";
		if(i == selected) {
			innerHTML += " selected='selected'";
		}
		innerHTML += ">[" + typeName + "] " + mapType.name + "</option>";
//		innerHTML += "<option style=\"background: " + background + ";\">[" + typeName + "] " + mapType.name + "</option>";
	}
	innerHTML += "</select>";

	var container = document.createElement("div");
	container.className = this.className;
	container.innerHTML = innerHTML;

	var controlDiv = document.createElement('div');
	controlDiv.style.padding = '5px';
	controlDiv.appendChild(container);
	return controlDiv;
}

function onComboChange(select) {
	selectMap(sListComboBox[select.id][select.selectedIndex]);
}

function GTraficControl() {
	return GListControl.call(this, "VTrafic", "Afficher le trafic", "TraficOptions", [new TraficCheckBox()/*, new EventsCheckBox()*/]);
}

GTraficControl.prototype = new GListControl;
GTraficControl.prototype.constructor = GTraficControl;

function GExtrasControl(country) {
	var list = [new GTraficCheckBox(), new SpeedCamCheckBox()];

	if(country == 'FR')
	{
		list.push(new GasCheckBox());
		list.push(new ParkingCheckBox());
		list.push(new MeteoCheckBox());
	}

	return GListControl.call(this, tabLang.Extras, tabLang.ExtrasInfo, "ExtrasOptions", list);
}

GExtrasControl.prototype = new GListControl;
GExtrasControl.prototype.constructor = GExtrasControl;

function AddTraficControl(country)
{
	if(country == 'FR' || getLanguage() == 'fr') {
		var controlDiv = new GTraficControl();
		controlDiv.index = 3;
		map.controls[google.maps.ControlPosition.TOP_RIGHT].push(controlDiv);
	}
}

function AddExtrasControl(country)
{
	if(getLanguage() == 'fr') {
		country = 'FR';
	}

	for(var i in scExtrasCountry)
	{
		if(scExtrasCountry[i] == country)
		{
			var controlDiv = new GExtrasControl(country);
			controlDiv.index = 2;
			map.controls[google.maps.ControlPosition.TOP_RIGHT].push(controlDiv);
			break;
		}
	}
}

function AddMapsControl(mpTypes, mpIndex)
{
	var controlDiv = new GComboBoxControl("map_combo", "mapCombo", mpTypes, mpIndex);
	controlDiv.index = 1;
	map.controls[google.maps.ControlPosition.TOP_RIGHT].push(controlDiv);
	selectMap(mpTypes[mpIndex]);
}

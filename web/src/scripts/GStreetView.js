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

function GStreetViewLayer()
{
	this.overlay = null;
}
GStreetViewLayer.prototype.showOverlay = function()
{
	if(!this.overlay)
	{
		this.overlay = new google.maps.StreetViewCoverageLayer();
		this.overlay.setMap(map);
	}
};
GStreetViewLayer.prototype.hideOverlay = function()
{
	if(this.overlay)
	{
		this.overlay.setMap(null);
		this.overlay = null;
	}
};

function GStreetView(container, latlng, closeButton)
{
	var panoOptions = {};

	if(latlng) {
		panoOptions.position = latlng;
	}
	if(closeButton) {
		panoOptions.enableCloseButton = closeButton;
	}

	this.panorama = new google.maps.StreetViewPanorama(container, panoOptions);
	map.setStreetView(this.panorama);
}
GStreetView.prototype.remove = function()
{
	this.panorama = null;
	map.setStreetView(this.panorama);
};
GStreetView.prototype.setLatlng = function(latlng)
{
	if(this.panorama) {
		this.panorama.setPosition(latlng);
	}
};
GStreetView.prototype.show = function()
{
	if(this.panorama) {
		this.panorama.setVisible(true);
	}
};
GStreetView.prototype.hide = function()
{
	if(this.panorama) {
		this.panorama.setVisible(false);
	}
};
GStreetView.prototype.isHidden = function()
{
	if(this.panorama) {
		return this.panorama.getVisible();
	} else {
		return true;
	}
};

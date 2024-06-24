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

function ZoomControl(controlDiv, map)
{
	// Creating divs & styles for custom zoom control
	controlDiv.style.padding = '5px';

	// Set CSS for the control wrapper
	var controlWrapper = document.createElement('div');
	controlWrapper.style.cursor = 'pointer';
	controlWrapper.style.textAlign = 'center';
	controlWrapper.style.width = '32px'; 
	controlWrapper.style.height = '64px';
	controlDiv.appendChild(controlWrapper);

	// Set CSS for the zoomIn
	var zoomInButton = document.createElement('div');
	zoomInButton.className = 'ZoomIn';
	controlWrapper.appendChild(zoomInButton);

	// Set CSS for the zoomOut
	var zoomOutButton = document.createElement('div');
	zoomOutButton.className = 'ZoomOut';
	controlWrapper.appendChild(zoomOutButton);

	// Setup the click event listeners for zoom-in, zoom-out:
	google.maps.event.addDomListener(zoomOutButton, 'click', function()
	{
		map.setZoom(map.getZoom() - 1);
	});

	google.maps.event.addDomListener(zoomInButton, 'click', function()
	{
		map.setZoom(map.getZoom() + 1);
	});
}

function AddZoomControl(map)
{
	var zoomControlDiv = document.createElement('div');
	zoomControlDiv.index = 1;

	new ZoomControl(zoomControlDiv, map);

	map.controls[google.maps.ControlPosition.TOP_LEFT].push(zoomControlDiv);
}

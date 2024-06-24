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

function CopyrightControl(controlDiv, map)
{
	// Set CSS for the control wrapper
	controlDiv.className = 'gm-style-cc';
	controlDiv.style.height = '14px';
	controlDiv.style.lineHeight = '14px';
    
	// Set CSS for the Copyright text
	var copyrightDiv = document.createElement('div');
	copyrightDiv.className  = 'Copyright';
	controlDiv.appendChild(copyrightDiv);

	google.maps.event.addListener(map, 'maptypeid_changed', function() {
		var mapType = map.mapTypes.get(map.getMapTypeId());
		if (mapType && mapType.credit)
		{ 
			copyrightDiv.style.display = 'block';
			copyrightDiv.innerHTML = mapType.credit;
		}
		else
			copyrightDiv.style.display = 'none';            
	});
}

function AddCopyrightControl(map)
{
	var copyrightDiv = document.createElement('div');
	copyrightDiv.index = 1;

	new CopyrightControl(copyrightDiv, map);

	map.controls[google.maps.ControlPosition.BOTTOM_RIGHT].push(copyrightDiv);
}

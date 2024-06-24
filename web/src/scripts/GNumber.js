function GText(point, html, classname, pixelOffset, color, zindex)
{
	// Mandatory parameters
	this.point = point;
	this.html = html;
	this.classname = classname||"";
	this.color = color;
	this.div_ = null;
	this.map_ = map;

	// Optional parameters
	this.pixelOffset = pixelOffset||new google.maps.Size(0,0);
	this.zindex=zindex||0;

	this.setMap(map); // Explicitly call setMap on this overlay
}

GText.prototype = new google.maps.OverlayView();

GText.prototype.onAdd = function()
{
	var div = document.createElement('div');

	div.style.borderStyle = 'none';
	div.style.borderWidth = '0px';
	div.style.position = 'absolute';
	div.style.zIndex = this.zindex;
	div.innerHTML = '<div class="' + this.classname + '">' + this.html + '</div>';

	this.div_ = div;
	this.getPanes().floatShadow.appendChild(div);
};

GText.prototype.onRemove = function()
{
	this.div_.parentNode.removeChild(this.div_);
	this.div_ = null;
};

GText.prototype.remove = function()
{
	this.setMap(null);
};

GText.prototype.copy = function()
{
	return new GText(this.point, this.html, this.pixelOffset, this.overlap);
};

GText.prototype.draw = function()
{
	var p = this.getProjection().fromLatLngToDivPixel(this.point);
	var h = parseInt(this.div_.clientHeight, 10);
	this.div_.style.left = (p.x + this.pixelOffset.width) + "px";
	this.div_.style.top = (p.y +this.pixelOffset.height - h) + "px";
	this.div_.style.backgroundColor = this.color;
	this.div_.style.zIndex = this.zindex;
};

GText.prototype.move = function(point)
{
	if(point != this.point)
	{
		this.point = point;
		this.draw();
	}
};

GText.prototype.zIndex = function(zindex)
{
	this.zindex = zindex;
};

GText.prototype.update = function(point, html, color)
{
	this.html = html;
	this.div_.innerHTML = '<div class="' + this.classname + '">' + this.html + '</div>';
	this.color = color;
	this.move(point);
};


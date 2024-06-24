function GButton(text, title, width)
{
	this.dvContainer = document.createElement("div");
	this.dvContainer.className = "ControlButton";
	
	var dvText = document.createElement("div");
	dvText.className = "ControlButtonContent";
	this.dvContainer.appendChild(dvText);

	if(width) {
		this.dvContainer.style.width = width;
	}
	
	this.setText(text);
	this.setTitle(title);
	this.setPushed(false);
}
GButton.prototype.getContainer = function()
{
	return this.dvContainer;
};
GButton.prototype.setPushed = function(push)
{
	var div = this.dvContainer.firstChild;
	div.style.fontWeight = push?"bold":"";
	div.style.border = "1px solid white";
	var shadows = push?["Top", "Left"]:["Bottom", "Right"];
	for (var i=0;i<shadows.length;i++) {
		div.style["border" + shadows[i]] = "1px solid #b0b0b0";
	}
};
GButton.prototype.setText = function(text)
{
	this.dvContainer.firstChild.innerHTML = text;
};
GButton.prototype.setTitle = function(title)
{
	this.dvContainer.title = title;
};
GButton.prototype.onClick = function(callback)
{
	this.dvContainer.onclick = callback||null;
};
GButton.prototype.onMouseOut = function(callback)
{
	this.dvContainer.onmouseout = callback||null;
};
GButton.prototype.onMouseOver = function(callback)
{
	this.dvContainer.onmouseover = callback||null;
};

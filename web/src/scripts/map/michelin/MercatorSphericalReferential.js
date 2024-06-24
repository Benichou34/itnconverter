function MercatorSphericalReferential(name)
{
	this.referentialName=name;
	this.a=6378137.0;
}

MercatorSphericalReferential.prototype.getGroundPixelSize=function(psp,uJ,uz)
{
	var lat=uz*Math.PI/180;
	var dlon=psp/this.a;
	var d=dlon*this.a*Math.cos(lat);
	return d;
};

MercatorSphericalReferential.prototype.wgs84ToMapRef=function(uJ,uz)
{
	var easting=this.a*Math.PI*uJ/180.0;
	var phi=uz*Math.PI/180.0;
	var northing=this.a*Math.log(Math.tan(Math.PI/4.0+phi/2.0));
	return new google.maps.Point(easting,northing);
};

MercatorSphericalReferential.prototype.mapRefToWGS84=function(easting,northing)
{
	var uJ=(easting/this.a)*(180.0/Math.PI);
	var phi=2*(Math.atan(Math.exp(northing/this.a))-Math.PI/4);
	var uz=phi*180.0/Math.PI;
	return new google.maps.LatLng(uz,uJ);
};

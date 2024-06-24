function MercatorReferential(name)
{
   this.referentialName=name;
   this.a=6378137.0;
   this.e=Math.sqrt(2/298.257223563-1/Math.pow(298.257223563,2));
   this.fs2=0.00335655146764446;
   this.fs4=0.0000065717547193811;
   this.fs6=1.76445428453261E-08;
}

MercatorReferential.prototype.getGroundPixelSize=function(psp,uJ,uz)
{
   var lat=uz*Math.PI/180;
   var dlon=psp/this.a;
   var d=dlon*this.a*Math.cos(lat)/Math.sqrt(1-Math.pow(this.e*Math.sin(lat),2));
   return d;
};

MercatorReferential.prototype.wgs84ToMapRef=function(uJ,uz)
{
   var easting=this.a*Math.PI*uJ/180.0;
   var phi=uz*Math.PI/180.0;
   var esp=this.e*Math.sin(phi);
   var northing=this.a*Math.log(Math.tan(Math.PI/4.0+phi/2.0)*Math.pow((1-esp)/(1+esp),this.e/2));
   return new google.maps.Point(easting,northing);
};

MercatorReferential.prototype.mapRefToWGS84=function(easting,northing)
{
   var uJ=(easting/this.a)*(180.0/Math.PI);
   var phi0=2*(Math.atan(Math.exp(northing/this.a))-Math.PI/4);
   var phi=phi0+this.fs2*Math.sin(2*phi0)+this.fs4*Math.sin(4*phi0)+this.fs6*Math.sin(6*phi0);
   var uz=phi*180.0/Math.PI;
   return new google.maps.LatLng(uz,uJ);
};

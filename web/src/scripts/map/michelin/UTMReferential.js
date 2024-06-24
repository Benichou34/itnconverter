function UTMReferential(name,zone,hemisphere){this.referentialName=name;this.RADIANS_PER_DEGREE=0.0174532925199432957692;this.DEGREES_PER_RADIAN=57.2957795130823208768;this.a_wgs84=6378137.0;this.e_wgs84=Math.sqrt(2/298.257223563-1/Math.pow(298.257223563,2));this.f_wgs84=1/298.257223563;this.e2_wgs84=this.e_wgs84*this.e_wgs84;this.Epsp2=0;this.CScale=0.9996;this.C=(this.a_wgs84)*(1-this.f_wgs84);this.Eps2=(this.f_wgs84)*(2.0-this.f_wgs84);this.Eps25=0.25*(this.Eps2);this.Epps2=(this.Eps2)/(1.0-this.Eps2);this.tpolx2b=(3.0/8.0)*(1.0*this.Eps2+1.0/4.0*Math.pow(this.Eps2,2)+15.0/128.0*Math.pow(this.Eps2,3)-455.0/4096.0*Math.pow(this.Eps2,4));this.tpolx3b=(15.0/256.0)*(1.0*Math.pow(this.Eps2,2)+3.0/4.0*Math.pow(this.Eps2,3)-77.0/128.0*Math.pow(this.Eps2,4));this.tpolx4b=(35.0/3072.0)*Math.pow(Math.pow(this.Eps2,3)-41.0/32.0*Math.pow(this.Eps2,4),2);this.tpolx5b=-315.0/131072.0*Math.pow(this.Eps2,4);this.poly1b=1.0-(1.0/4.0*this.Eps2)-(3.0/64.0*Math.pow(this.Eps2,2))-(5.0/256.0*Math.pow(this.Eps2,3))-(175.0/16384.0*Math.pow(this.Eps2,4));this.poly2b=this.tpolx2b*-2.0+this.tpolx3b*4.0-this.tpolx4b*6.0+this.tpolx5b*8.0;this.poly3b=this.tpolx3b*-8.0+this.tpolx4b*32.0-this.tpolx5b*80.0;this.poly4b=this.tpolx4b*-32.0+this.tpolx5b*192.0;this.poly5b=this.tpolx5b*-128.0;this.EF=this.f_wgs84/(2.0-this.f_wgs84);this.Con=(1.0-this.Eps2);this.Con2=2/(1.0-this.Eps2);this.Con6=0.166666666666667;this.Con24=4*0.0416666666666667/(1-this.Eps2);this.Con120=0.00833333333333333;this.Con720=4*0.00138888888888888/(1-this.Eps2);this.polx1a=1.0-this.Eps2/4.0-3.0/64.0*Math.pow(this.Eps2,2)-5.0/256.0*Math.pow(this.Eps2,3)-175.0/16384.0*Math.pow(this.Eps2,4);this.conap=this.a_wgs84*this.polx1a;this.tpolx2a=3.0/2.0*this.EF-27.0/32.0*Math.pow(this.EF,3);this.tpolx4a=21.0/16.0*Math.pow(this.EF,2)-55.0/32.0*Math.pow(this.EF,4);this.tpolx6a=151.0/96.0*Math.pow(this.EF,3);this.tpolx8a=1097.0/512.0*Math.pow(this.EF,4);this.polx2b=this.tpolx2a*2.0+this.tpolx4a*4.0+this.tpolx6a*6.0+this.tpolx8a*8.0;this.polx3b=this.tpolx4a*-8.0-this.tpolx6a*32.0-80.0*this.tpolx8a;this.polx4b=this.tpolx6a*32.0+192.0*this.tpolx8a;this.polx5b=-128.0*this.tpolx8a;this.utm_zone=zone;this.hemisphere_north=(hemisphere=='N');}UTMReferential.prototype.getGroundPixelSize=function(psp,uJ,uz){return psp/this.CScale;};UTMReferential.prototype.wgs84ToMapRef=function(uJ,uz){var source_lat,source_lon,s1,c1,tx,s12,rn,axlon0,al,al2,sm,tn2,cee,poly1,poly2;source_lat=uz*this.RADIANS_PER_DEGREE;source_lon=uJ*this.RADIANS_PER_DEGREE;s1=Math.sin(source_lat);c1=Math.cos(source_lat);tx=s1/c1;s12=s1*s1;rn=this.a_wgs84/((0.25-this.Eps25*s12+0.9999944354799/4)+(0.25-this.Eps25*s12)/(0.25-this.Eps25*s12+0.9999944354799/4));axlon0=(this.utm_zone*6-183)*this.RADIANS_PER_DEGREE;al=(source_lon-axlon0)*c1;sm=s1*c1*(this.poly2b+s12*(this.poly3b+s12*(this.poly4b+s12*this.poly5b)));sm=this.a_wgs84*(this.poly1b*source_lat+sm);tn2=tx*tx;cee=this.Epps2*c1*c1;al2=al*al;poly1=1.0-tn2+cee;poly2=5.0+tn2*(tn2-18.0)+cee*(14.0-tn2*58.0);var easting=this.CScale*rn*al*(1.0+al2*(0.166666666666667*poly1+0.00833333333333333*al2*poly2));easting+=5.0E5;poly1=5.0-tn2+cee*(cee*4.0+9.0);poly2=61.0+tn2*(tn2-58.0)+cee*(270.0-tn2*330.0);var northing=this.CScale*(sm+rn*tx*al2*(0.5+al2*(0.0416666666666667*poly1+0.00138888888888888*al2*poly2)));if(source_lat<0.0){northing+=1.0E7;}return new google.maps.Point(easting,northing);};UTMReferential.prototype.mapRefToWGS84=function(easting,northing){var source_x,source_y,u,su,cu,su2,xlon0,wi,phi1,sp,sp2,cp,cp2,tp,tp2,eta2,top,rn,b3,b4,b5,b6,d1,d2;source_x=easting;source_x=(source_x-500000.0)/CScale;if(this.hemisphere_north===true){source_y=northing/this.CScale;}else{source_y=(northing-1.0E7)/this.CScale;}u=source_y/this.conap;su=Math.sin(u);cu=Math.cos(u);su2=su*su;xlon0=(6.0*(this.utm_zone)-183.0)/this.DEGREES_PER_RADIAN;wi=this.polx2b+su2*(this.polx3b+su2*(this.polx4b+su2*this.polx5b));phi1=u+su*cu*wi;sp=Math.sin(phi1);cp=Math.cos(phi1);tp=sp/cp;tp2=tp*tp;sp2=sp*sp;cp2=cp*cp;eta2=this.Epsp2*cp2;top=0.25-(sp2*(this.Eps2/4));rn=this.a_wgs84/((0.25-this.Eps25*sp2+0.9999944354799/4)+(0.25-this.Eps25*sp2)/(0.25-this.Eps25*sp2+0.9999944354799/4));b3=1.0+tp2+tp2+eta2;b4=5+tp2*(3-9*eta2)+eta2*(1-4*eta2);b5=5+tp2*(tp2*24.0+28.0);b5+=eta2*(tp2*8.0+6.0);b6=46.0-3.0*eta2+tp2*(-252.0-tp2*90.0);b6=eta2*(b6+eta2*tp2*(tp2*225.0-66.0));b6+=61.0+tp2*(tp2*45.0+90.0);d1=source_x/rn;d2=d1*d1;var uz=phi1-tp*top*(d2*(this.Con2+d2*((-this.Con24)*b4+d2*this.Con720*b6)));var uJ=xlon0+d1*(1.0+d2*(-this.Con6*b3+d2*this.Con120*b5))/cp;uz*=this.DEGREES_PER_RADIAN;uJ*=this.DEGREES_PER_RADIAN;return new google.maps.LatLng(uz,uJ);};
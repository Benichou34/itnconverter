google.maps.__gjsload__('overlay', function(_){var yw=_.oa("g"),zw=_.n(),Aw=function(a){a.Xf=a.Xf||new zw;return a.Xf},Bw=function(a){this.Z=new _.hh(function(){var b=a.Xf;if(a.getPanes()){if(a.getProjection()){if(!b.g&&a.onAdd)a.onAdd();b.g=!0;a.draw()}}else{if(b.g)if(a.onRemove)a.onRemove();else a.remove();b.g=!1}},0)},Cw=function(a,b){function c(){return _.jh(e.Z)}var d=Aw(a),e=d.Ee;e||(e=d.Ee=new Bw(a));_.B(d.Y||[],_.N.removeListener);var f=d.ha=d.ha||new _.En,g=b.__gm;f.bindTo("zoom",g);f.bindTo("offset",g);f.bindTo("center",g,"projectionCenterQ");
f.bindTo("projection",b);f.bindTo("projectionTopLeft",g);f=d.Uh=d.Uh||new yw(f);f.bindTo("zoom",g);f.bindTo("offset",g);f.bindTo("projection",b);f.bindTo("projectionTopLeft",g);a.bindTo("projection",f,"outProjection");a.bindTo("panes",g);d.Y=[_.N.addListener(a,"panes_changed",c),_.N.addListener(g,"zoom_changed",c),_.N.addListener(g,"offset_changed",c),_.N.addListener(b,"projection_changed",c),_.N.addListener(g,"projectioncenterq_changed",c)];c();b instanceof _.Ye&&(_.Ki(b,"Ox"),_.ao("Ox","-p",a))},
Fw=function(a){if(a){var b=a.getMap(),c=a.__gmop;if(c){if(c.map==b)return;a.__gmop=null;c.eg()}if(b&&b instanceof _.Ye){var d=b.__gm;d.overlayLayer?a.__gmop=new Dw(b,a,d.overlayLayer):d.g.then(function(e){e=e.wa;var f=new Ew(b,e);e.ua(f);d.overlayLayer=f;Fw(a)})}}},Dw=function(a,b,c){this.map=a;this.ta=b;this.Ll=c;this.qe=!1;_.Ki(this.map,"Ox");_.ao("Ox","-p",this.ta);c.h.push(this);c.g&&Gw(this,c.g);c.i.Nf()},Gw=function(a,b){a.ta.get("projection")!=b&&(a.ta.bindTo("panes",a.map.__gm),a.ta.set("projection",
b))},Ew=function(a,b){this.j=a;this.i=b;this.g=null;this.h=[]};_.A(yw,_.O);yw.prototype.changed=function(a){"outProjection"!=a&&(a=!!(this.get("offset")&&this.get("projectionTopLeft")&&this.get("projection")&&_.sd(this.get("zoom"))),a==!this.get("outProjection")&&this.set("outProjection",a?this.g:null))};_.A(Bw,_.O);Dw.prototype.draw=function(){this.qe||(this.qe=!0,this.ta.onAdd&&this.ta.onAdd());this.ta.draw&&this.ta.draw()};Dw.prototype.eg=function(){_.bo("Ox","-p",this.ta);this.ta.unbindAll();this.ta.set("panes",null);this.ta.set("projection",null);_.kb(this.Ll.h,this);this.qe&&(this.qe=!1,this.ta.onRemove?this.ta.onRemove():this.ta.remove())};Ew.prototype.dispose=_.n();
Ew.prototype.Ta=function(a,b,c,d,e,f){var g=this.g=this.g||new _.wo(this.j,this.i,_.n());g.Ta(a,b,c,d,e,f);a=_.Ca(this.h);for(b=a.next();!b.done;b=a.next())b=b.value,Gw(b,g),b.draw()};_.qf("overlay",{Ig:function(a){if(a){var b=a.getMap();if(b&&b instanceof _.Ye||a.__gmop)Fw(a);else{b=a.getMap();var c=Aw(a),d=c.bl;c.bl=b;d&&(c=Aw(a),(d=c.ha)&&d.unbindAll(),(d=c.Uh)&&d.unbindAll(),a.unbindAll(),a.set("panes",null),a.set("projection",null),_.B(c.Y,_.N.removeListener),c.Y=null,c.Ee&&(c.Ee.Z.Ra(),c.Ee=null),_.bo("Ox","-p",a));b&&Cw(a,b)}}},preventMapHitsFrom:function(a){_.lp(a,{onClick:function(b){return _.Eo(b.event)},La:function(b){return _.Bo(b)},oc:function(b){return _.Co(b)},ab:function(b){return _.Co(b)},
Qa:function(b){return _.Do(b)}}).Jc(!0)},preventMapHitsAndGesturesFrom:function(a){a.addEventListener("click",_.pe);a.addEventListener("contextmenu",_.pe);a.addEventListener("dblclick",_.pe);a.addEventListener("mousedown",_.pe);a.addEventListener("mousemove",_.pe);a.addEventListener("MSPointerDown",_.pe);a.addEventListener("pointerdown",_.pe);a.addEventListener("touchstart",_.pe);a.addEventListener("wheel",_.pe)}});});

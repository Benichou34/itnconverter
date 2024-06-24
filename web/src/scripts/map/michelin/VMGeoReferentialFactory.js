function VMGeoReferentialFactory()
{
	this.REFERENTIAL_MILLER="MILLER";
	this.REFERENTIAL_EQUI43="EQUI43";
	this.REFERENTIAL_MDM35="MDM35";
	this.REFERENTIAL_MDS84="MDS84";
	this.REFERENTIAL_MDSPH="MDSPH";
	this.REFERENTIAL_MLA2E="MLA2E";
	this.REFERENTIAL_MLAEST="MLAEST";
	this.REFERENTIAL_MLAWES="MLAWES";
	this.REFERENTIAL_MLAEUR="MLAEUR";
	this.REFERENTIAL_MTMGR="MTMGR";
	this.REFERENTIAL_MTMGB="MTMGB";
	this.REFERENTIAL_MTMCY="MTMCY";
	this.REFERENTIAL_MTMPL="MTMPL";
	this.REFERENTIAL_MTMSC="MTMSC";
	this.REFERENTIAL_UTM26N="UTM26N";
	this.REFERENTIAL_UTM28N="UTM28N";

	this.referentials=[];
	this.referentials[this.REFERENTIAL_MILLER]=new MillerReferential(this.REFERENTIAL_MILLER);
	this.referentials[this.REFERENTIAL_EQUI43]=new Equi43Referential(this.REFERENTIAL_EQUI43);
	this.referentials[this.REFERENTIAL_MDS84]=new MercatorReferential(this.REFERENTIAL_MDS84);
	this.referentials[this.REFERENTIAL_MDSPH]=new MercatorSphericalReferential(this.REFERENTIAL_MDSPH);
	this.referentials[this.REFERENTIAL_MDM35]=new Mdm35Referential(this.REFERENTIAL_MDM35);
	this.referentials[this.REFERENTIAL_MLA2E]=new LambertReferential(this.REFERENTIAL_MLA2E,-168,-60,320,6378249.2,0.0068034877,2.337222,46.8,45.8989194444,47.6960138889);
	this.referentials[this.REFERENTIAL_MLAEST]=new LambertReferential(this.REFERENTIAL_MLAEST,-84,-97,117,6378388,0.00672267,18,45,36.25,52.933333);
	this.referentials[this.REFERENTIAL_MLAWES]=new LambertReferential(this.REFERENTIAL_MLAWES,-84,-97,-117,6378388,0.00672267,-3,45,36.25,52.933333);
	this.referentials[this.REFERENTIAL_MLAEUR]=new LambertReferential(this.REFERENTIAL_MLAEUR,-84,-97,-117,6378388,0.00672267,11,51,36.25,52.933333);
	this.referentials[this.REFERENTIAL_MTMGR]=new TransverseMercatorReferential(this.REFERENTIAL_MTMGR,-84,-97,-117,6378388,0.00672267,24.0,35.0,0.9996);
	this.referentials[this.REFERENTIAL_MTMGB]=new TransverseMercatorReferential(this.REFERENTIAL_MTMGB,-84,-97,-117,6378388,0.00672267,-2.0,49.0,0.9996);
	this.referentials[this.REFERENTIAL_MTMCY]=new TransverseMercatorReferential(this.REFERENTIAL_MTMCY,-84,-97,-117,6378388,0.00672267,33.5,35.0,0.9996);
	this.referentials[this.REFERENTIAL_MTMPL]=new TransverseMercatorReferential(this.REFERENTIAL_MTMPL,-84,-97,-117,6378388,0.00672267,18.0,49.0,0.9996);
	this.referentials[this.REFERENTIAL_MTMSC]=new TransverseMercatorReferential(this.REFERENTIAL_MTMSC,-84,-97,-117,6378388,0.00672267,18.0,60.0,0.9996);
	this.referentials[this.REFERENTIAL_UTM26N]=new UTMReferential(this.REFERENTIAL_UTM26N,26,'N');
	this.referentials[this.REFERENTIAL_UTM28N]=new UTMReferential(this.REFERENTIAL_UTM28N,28,'N');
}

VMGeoReferentialFactory.prototype.getReferential=function(name)
{
	if(this.referentials[name])
	{
		return this.referentials[name];
	}
	else
	{
		return this.referentials["MILLER"];
	}
};

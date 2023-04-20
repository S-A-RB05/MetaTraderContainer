// Currency Strength Strategy /////////////////////
///////////////////////////////////////////////////

function run()
{
	//assetList("AssetsCur.csv");
	set(LOGFILE+PLOTNOW);
	PlotWidth = 800;
	PlotHeight1 = 340;

	ccyReset();
	string Name;
	while(Name = (loop(Assets)))
	{
		if(assetType(Name) != FOREX) continue;
		asset(Name);
		vars Prices = series(priceClose());
		var Strength = ROC(Prices,1);
 		ccySet(Strength);
	}
	
	static char OldBest[8], OldWorst[8];
	string Best = ccyMax(), Worst = ccyMin();

	if(*OldBest && !strstr(Best,OldBest)) {
		asset(OldBest);
		exitLong();
		if(ccyStrength(Best) > 1.) {
			asset(Best);
			enterLong();
		}
	} 
	if(*OldWorst && !strstr(Worst,OldWorst)) {
		asset(OldWorst);
		exitShort();
		if(ccyStrength(Worst) < -1.) {
			asset(Worst);
			enterShort();
		}
	}
	
	strcpy(OldBest,Best);
	strcpy(OldWorst,Worst);
}



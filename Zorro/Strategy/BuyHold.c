// Buy & Hold Strategy ////////////////////////////

#define TICKER			"SPY"
//#define BOND			"AGG"	// comment in for bond mix
#define USE_CFD				// CFD based asset parameters
#define BONDPERCENT	40		// percentage of bonds
#define STARTDATE		2013

void assetSetup() // modify if needed
{
	Capital = 10000;	// total capital
#ifndef USE_CFD	
	Leverage = 2;		// margin account
	Margin = 7500;	// total invested margin (avoid margin call!)
	Interest = 1.5;	// annual broker interest, percent
#else
	Leverage = 20;	// CFD leverage
	Margin = 2500;	// total invested margin (avoid margin call!)
	Interest = 4;	// annual broker interest, percent
#endif
}

void run()
{
	set(PLOTNOW,LOGFILE);
	BarPeriod = 1440;
	LookBack = 0;
	StartDate = STARTDATE;
	EndDate = year(NOW);

	History = "?.t6"; // enforce STOOQ history
	Slippage = 0;
#ifdef BOND
	assetAdd(BOND);
	assetHistory(BOND,FROM_STOOQ);
	asset(BOND);
#endif
	assetAdd(TICKER);
	assetHistory(TICKER,FROM_STOOQ);
	asset(TICKER);
	assetSetup();
	if(is(FIRSTRUN)) {
#ifdef BOND
		Margin *= 1.-0.01*BONDPERCENT;
		enterLong();
		asset(BOND);
		assetSetup();
		Margin *= 0.01*BONDPERCENT;
#endif
		enterLong();		
	}
}
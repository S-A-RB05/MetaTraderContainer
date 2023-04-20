// Workshop 8: Simple Option system //////////////////////////////////////////
#include <contract.c>

#define PREMIUM	3.00
#define DAYS		6*7 // expire after 6 weeks 

void run() 
{
	StartDate = 20120101;
	EndDate = 20190101;
	BarPeriod = 1440;
	set(PLOTNOW,LOGFILE);

	History = "*.t8"; // options data for price history
	assetList("AssetsIB");
	asset("SPY");
	Multiplier = 100;

// load today's contract chain
	if(!contractUpdate(Asset,0,CALL|PUT)) return;

// Enter new positions
	if(!NumOpenShort && !is(LOOKBACK)) { 
		if(combo(
			contractFind(CALL,DAYS,PREMIUM,2),1, 
			contractFind(PUT,DAYS,PREMIUM,2),1,
			0,0,0,0)) 
		{
			MarginCost = comboMargin(-1,3);
			enterShort(comboLeg(1));
			enterShort(comboLeg(2));
		}
	}
}
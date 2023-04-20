// Machine learning ////////////////////////////
#include <profile.c>

function run()
{
	StartDate = 2010;
	EndDate = 2018;
	BarPeriod = 1440; // 1 day
	NumWFOCycles = 5;
	asset("EUR/USD");
	
	set(RULES,TESTNOW,PLOTNOW,LOGFILE);

	if(Train) Hedge = 2;	// for training, allow long + short	
	LifeTime = 1; 
	if(!Train) MaxLong = MaxShort = 1;
	
	if(adviseLong(PATTERN+FAST+2+RETURNS,0,
		priceH(2),priceL(2),priceC(2),
		priceH(1),priceL(1),priceC(1),
		priceH(1),priceL(1),priceC(1),
		priceH(0),priceL(0),priceC(0)) > 50)
		enterLong();
	
	if(adviseShort() > 50)
		enterShort();

	//plotTradeProfile(40);
	//plotWFOCycle(Equity,0);
	//plotWFOProfit();
}

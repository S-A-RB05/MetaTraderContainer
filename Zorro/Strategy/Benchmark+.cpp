// Simple RSI based system for speed comparison //////////
#include <zorro.h>

DLLFUNC void run()
{
	set(TICKS);  // tick based backtest
	BarPeriod = 60;
	StartDate = 2011;
	EndDate = 2020;	// 10 years
	assetList("AssetsFix");
	asset("EUR/USD");
	
// get the RSI series
	vars RSIs = series(RSI(seriesC(),20));
 
// set up trade limits 
	Stop = 100*PIP;
	TakeProfit = 100*PIP;
	LifeTime = 24; 				// close trades after 24 hours
	MaxLong = MaxShort = 1;		// no multiple positions
	Hedge = 1;						// close opposite positions 
 
// if rsi crosses over buy level, enter long
	if(crossOver(RSIs,75))
		enterLong();
// if rsi crosses below sell level, enter short
	if(crossUnder(RSIs,25))
		enterShort();

// measure the time
	static var Time = 0;
	if(is(INITRUN)) 
		Time = timer();
	if(is(EXITRUN)) 
		printf("\n>>> %i ticks, %.3f sec <<<",NumTicks,(timer()-Time)/1000);
}
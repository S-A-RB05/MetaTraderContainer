// Trend Trading ///////////////////
#include <profile.c>

function run()
{
	set(LOGFILE,PLOTNOW); // log all trades
	StartDate = 2012; // fixed simulation period 
	EndDate = 2015; // tailored for this strategy 
	Verbose = 2;
	LookBack = 300;	// needed for MMI
	asset("EUR/USD");

	vars Prices = series(priceC());
	vars Trends = series(LowPass(Prices,300));
	
	Stop = 30*ATR(100); // very distant stop
	
	vars MMI_Raws = series(MMI(Prices,300));
	vars MMI_Smooths = series(LowPass(MMI_Raws,300));
	
	if(falling(MMI_Smooths)) 
	{
		if(valley(Trends))
			enterLong();
		else if(peak(Trends))
			enterShort();
	}
	
	plot("MMI_Raw",MMI_Raws,NEW,GREY);
	plot("MMI_Smooth",MMI_Smooths,0,BLACK);
	//setf(PlotMode,PL_DARK);
	//plotTradeProfile(-50); 
}
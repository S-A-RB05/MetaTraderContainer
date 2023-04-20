// Parameter optimization Test ////////////////

#define METHOD	ASCENT
//#define METHOD	BRUTE (Zorro S required)
//#define METHOD	GENETIC (Zorro S required)

function run()
{
	set(PARAMETERS,TESTNOW);	// generate and use optimized parameters
	setf(TrainMode,METHOD);
	set(LOGFILE);					// export parameter charts
	BarPeriod = 240;	
	LookBack = 500;
	StartDate = 2011;
	EndDate = 2020; 	
	NumWFOCycles = 10; // activate WFO
	NumCores = -1;	// multicore training (Zorro S required)
	
// measure the time
	static var Time = 0;
	if(Train && is(FIRSTINITRUN)) 
		Time = timer();
	if(Test && Init && Time > 0)
		printf("\nTime for optimizing >>> %.3f sec <<<",(timer()-Time)/1000);

// portfolio loop
	while(asset(loop("EUR/USD","USD/JPY","GBP/USD"))) 
	{
// calculate the buy/sell signal
		vars Price = series(price());
		vars Filtered = series(BandPass(Price,optimize(30,20,60,5),0.5));
		vars Signal = series(FisherN(Filtered,500));
		var Threshold = optimize(1,0.6,1.4,0.1);

// buy and sell
		Stop = optimize(10,2,20,2) * ATR(100);
		Trail = optimize(4,2,20,2) * ATR(100);
		if(crossUnder(Signal,-Threshold))
			enterLong(); 
		else if(crossOver(Signal,Threshold))
			enterShort();
	}
} 
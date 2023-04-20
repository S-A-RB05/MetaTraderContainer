// Backtest Reality Check ///////////////////////
// Start in [Train] mode ////////////////////////
#include <profile.c>

#define CYCLES 500
#define PLOT_CLIP		2	// clip histogram
#define FILE_NAME		"Log\\RealityCheck.csv" // exported results


// moon phase trading system - don't try that with real money!
void strategy()
{
   BarPeriod = 1440;
   StartDate = 2016;

	assetList("AssetsIB"); 
   asset("SPY");
	
	set(PARAMETERS);
	int MoonPhase = fmod(wdate(0),29.5306);
   int WaxingDay = optimize(1,1,14,1);
   int WaningDay = optimize(15,15,29,1);
   
// trade by moon...	
	if(MoonPhase == WaxingDay)
		enterLong();
	else if(MoonPhase == WaningDay)
		enterShort();
}

function run()
{
#ifdef CYCLES		
	NumTotalCycles = CYCLES;
	if(TotalCycle > 1) 
		setf(Detrend,SHUFFLE);
	set(PRELOAD,TESTNOW);
#endif
	strategy();
}

function evaluate()
{
	var Result = objective(); // pessimistic return ratio
	if(NumTotalCycles < 2) {
		printf("\nObjective: %.3f",Result);
		return;
	}
	static var OriginalResult,PV;
	if(TotalCycle == 1) {
		OriginalResult = Result;
		PV = 0;
		file_write(FILE_NAME,"Cycle,Result\n",0);
	} else {
		if(Result < PLOT_CLIP*OriginalResult) // clip image
			plotHistogram("Random",Result,OriginalResult/50,1,RED);	
		if(Result >= OriginalResult)
			PV += 100./NumTotalCycles;		
	}
	file_append(FILE_NAME,strf("%i,%.3f\n",TotalCycle,Result),0);
	if(TotalCycle == NumTotalCycles) { // last cycle
		PlotLabels = 2;
		plotHistogram("Original",OriginalResult,OriginalResult/50,sqrt(NumTotalCycles),BLACK);	
		plotHistogram("",0,OriginalResult/50,1,-1);	// plot labels
		printf("\n-------------------------------------------");
		printf("\nP-Value %.1f%%",PV);
		if(PV <= 1)
			printf("\nResult is significant");
		else if(PV <= 5)
			printf("\nResult is possibly significant");
		else 
			printf("\nResult insignificant");
		printf("\n-------------------------------------------");
	} 
}
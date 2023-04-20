// Montecarlo Reality Check ///////////////////////
// 1. Put script name in #include line below
// 2. Set number of CYCLES (100-1000)
// 3. Set randomizing (BOOTSTRAP or SHUFFLE)
// 3. Start in [Train] mode for optimized or trained strategies,
//    in {Test] mode for unoptimized strategies
//////////////////////////////////////////////////
#include <profile.c>
#define	DO_MRC
#define	run strategy
#include	"TVC_Grid.c"	// <= your script
#undef	run
#define CYCLES		1000
#define RANDOMIZE	BOOTSTRAP	// or SHUFFLE 

function run()
{
	set(TESTNOW);			// for training
	NumTotalCycles = CYCLES;
	if(TotalCycle == 1)	// first cycle = original
		seed(1234);		// always same random sequence 
#ifdef RANDOMIZE
	else
		Detrend = RANDOMIZE;
	set(PRELOAD);
#endif
	strategy();
	set(LOGFILE|OFF); // don't export files
}

function evaluate()
{
	PlotLabels = 4;
	var PF = 10;
	if(LossTotal > 0) PF = WinTotal/LossTotal;
	static var OriginalProfit, Probability;
	if(TotalCycle == 1) {
		OriginalProfit = PF;
		Probability = 0;
	} else {
		if(PF > OriginalProfit) {
			Probability += 100./NumTotalCycles;
			if(PF < 2*OriginalProfit)  // clip image at double range
				plotHistogram("Above",PF,OriginalProfit/50,1,RED);
		} else
			plotHistogram("Below",PF,OriginalProfit/50,1,BLACK);
	}
	if(TotalCycle == NumTotalCycles) { // last cycle
		printf("\n-------------------------------------------");
		printf("\nP-Value %.1f%%",Probability);
		printf("\nResult is ");
		if(Probability <= 1)
			printf("highly significant");
		else if(Probability <= 5)
			printf("significant");
		else if(Probability <= 15)
			printf("maybe significant");
		else 
			printf("statistically insignificant");
		printf("\n-------------------------------------------");
	} 
}
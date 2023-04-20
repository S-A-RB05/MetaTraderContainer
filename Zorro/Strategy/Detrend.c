// Detrending and shuffling ///////////////////////
// Black - original prices
// Blue - detrended prices
// Red - inverted prices
// Green - shaped prices to simulate a sudden drop
// Grey - randomized prices

// randomizing methods
//#define METHOD	BOOTSTRAP 
//#define METHOD	RANDOMWALK
#define METHOD	SHUFFLE
//#define METHOD	SHUFFLE|PEAK|VALLEY

function run()
{
	set(PLOTNOW|LEAN|PRELOAD); // reload prices after any cycle
	setf(PlotMode,PL_LONG);
	StartDate = 2012; 
	BarPeriod = 1440;
	LookBack = 0;
	NumTotalCycles = 10;

	if(TotalCycle == 1) {
		Detrend = CURVE; 
		plot("Detrend",priceClose(),MAIN|LINE,BLUE);		
	} else if(TotalCycle == 2) {
		Detrend = INVERT; 
		plot("Invert",priceClose(),MAIN|LINE,RED);		
	} else if(TotalCycle == 3) {
		Detrend = SHAPE; 
		var SuddenDrop[10] = { 1.5, 1.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.5, 1.5, 0 };
		Shape = SuddenDrop;
		plot("Shape",priceClose(),MAIN|LINE,GREEN);		
	} else if(TotalCycle <= NumTotalCycles-1) {
		Detrend = METHOD; 
		plot(strf("Shuffle%d", TotalCycle),priceClose(),MAIN,GREY); //color(100*TotalCycle/NumTotalCycles,BLUE,RED,GREEN));
	} // last cycle (TotalCycle == NumTotalCycles) is original price curve
}

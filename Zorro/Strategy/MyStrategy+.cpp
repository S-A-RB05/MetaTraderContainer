///////////////////////////////////////////////////////
// Examples for C++-based Zorro strategies
///////////////////////////////////////////////////////
//#include "stdafx.h" // for precompiled headers
#include <zorro.h>

//#define HELLO
//#define DOWNLOAD
#define WORKSHOP4
//#define WORKSHOP6
//#define WORKSHOP7
//#define RTEST
//#define PYTHONTEST
//#define OPTIONS
//#define DEEPLEARN
//#define DLLTEST	// user-defined extension DLL

#ifdef HELLO //////////////////////////////////////////

DLLFUNC void main()
{
	Verbose = 3;
	printf("Hello World");
	sound("win.wav");
}
#endif

#ifdef DOWNLOAD //////////////////////////////////////////

DLLFUNC int main()
{
  StartDate = 20100101; // YYYYMMDD format
  EndDate = NOW;  // up to today
  assetHistory("AAPL",FROM_YAHOO|OVERRIDE); // download data
}
#endif

#ifdef WORKSHOP4 //////////////////////////////////////
#include <profile.c>

DLLFUNC void run()
{
	if(Init) 
		printf("\nStrategy DLL test");
	set(LOGFILE,PLOTNOW); // log all trades
	EndDate = 20171231; // fixed simulation period 
	LookBack = 300;
	asset("EUR/USD");

	vars Prices = series(priceC(0));
	vars Trend = series(LowPass(Prices,300));
	Stop = 30*ATR(100);
	
	vars MMI_Raw = series(MMI(Prices,300));
	vars MMI_Smooth = series(LowPass(MMI_Raw,300));
	
	if(falling(MMI_Smooth)) {
		if(valley(Trend))
			enterLong();
		else if(peak(Trend))
			enterShort();
	}
	plotTradeProfile(-50); 
}
#endif

#ifdef WORKSHOP6 //////////////////////////////////////
void tradeCounterTrend()
{
	TimeFrame = 4;
	vars Prices = series(price(0));
	vars Filtered = series(BandPass(Prices,optimize(30,20,40,5,0),0.5));
	vars Signal = series(FisherN(Filtered,500));
	var Threshold = optimize(1,0.5,1.5,0.1,0);
	
	Stop = optimize(10,5,30,5,0) * ATR(100);
	Trail = 4*ATR(100);
	
	if(crossUnder(Signal,-Threshold)) 
		enterLong(); 
	else if(crossOver(Signal,Threshold)) 
		enterShort();
}

void tradeTrend()
{
	TimeFrame = 1;
	vars Prices = series(price(0));
	vars Trends = series(LowPass(Prices,optimize(300,100,700,50,0)));

	Stop = optimize(10,2,30,0,0) * ATR(100);
	Trail = 0;

	vars MMI_Raw = series(MMI(Prices,300));
	vars MMI_Smooth = series(LowPass(MMI_Raw,300));
	
	if(falling(MMI_Smooth)) {
		if(valley(Trends))
			enterLong();
		else if(peak(Trends))
			enterShort();
	}
}

DLLFUNC void run()
{
	set(PARAMETERS,FACTORS);  // generate and use optimized parameters and factors
	set(TESTNOW,PLOTNOW,LOGFILE);
	StartDate = 2005;
	EndDate = 2017; 	// fixed simulation period
	BarPeriod = 60;	// 1 hour bars
	LookBack = 2000;	// needed for Fisher()
	NumCores = -2;		// use multiple cores (Zorro S only)
	NumWFOCycles = 10; // activate WFO
	Capital = 10000;		// reinvestment mode
	
	if(ReTrain) {
		UpdateDays = -1;	// update price data from the server 
		SelectWFO = -1;	// select the last cycle for re-optimization
		set(FACTORS|OFF);	// don't generate factors when re-training
	}
	
// portfolio loop
	while(asset(loop("EUR/USD","USD/JPY")))
	while(algo(loop("TRND","CNTR")))
	{
// don't reinvest
		//Margin = 0.5 * OptimalF * Capital;
// reinvest the square root of the component profit		
		Margin = 0.5 * OptimalF * Capital * sqrt(1 + ProfitClosed/Capital);

		if(strstr(Algo,"TRND")) 
			tradeTrend();
		else if(strstr(Algo,"CNTR")) 
			tradeCounterTrend();
	}
}
#endif

#ifdef WORKSHOP7 //////////////////////////////////////
DLLFUNC void run()
{
	StartDate = 2010;
	EndDate = 2018;
	BarPeriod = 1440; // 1 day
	NumWFOCycles = 5;
#ifdef _WIN64
	Script = "MyStrategy64";
	LogNumber = 2;
#endif	
	asset("EUR/USD");
	
	set(RULES,TESTNOW,PLOTNOW,LOGFILE);

	if(Train) Hedge = 2;	// for training, allow long + short	
	LifeTime = 1; 
	if(!Train) MaxLong = MaxShort = 1;
	
	var GoLong = adviseLong(PATTERN+FAST+2+RETURNS,0,
		priceH(2),priceL(2),priceC(2),
		priceH(1),priceL(1),priceC(1),
		priceH(1),priceL(1),priceC(1),
		priceH(0),priceL(0),priceC(0));
	if(GoLong > 50) enterLong();

	var GoShort = adviseShort(PATTERN+FAST+2+RETURNS,0,
		priceH(2),priceL(2),priceC(2),
		priceH(1),priceL(1),priceC(1),
		priceH(1),priceL(1),priceC(1),
		priceH(0),priceL(0),priceC(0));
	if(GoShort > 50) enterShort();
	
	printf(" > %.2f %.2f",GoLong,GoShort);
	plot("Long",GoLong,NEW,BLUE);
	plot("Short",GoShort,0,RED);
}
#endif

#ifdef RTEST //////////////////////////////////////////
#include <r.h>

DLLFUNC void main()
{
	if(!Rstart()) {
		print(TO_WINDOW,"Error - R won't start!");
		return;
	}
	
	var vecIn[5],vecOut[5];
	int i;
	for(i=0; i<5; i++) 
		vecIn[i] = i;
	
	Rset("Rin",vecIn,5);
	Rx("Rout <- Rin * 10");
	Rv("Rout",vecOut,5);
		
	if(!Rrun()) {
		print(TO_WINDOW,"Error - R session aborted!");
		return;
	} 
	
	print(TO_WINDOW,"\nReturned: ");
		for(i=0; i<5; i++) 
			print(TO_WINDOW,"%.0f ",vecOut[i]);

// test a function
	Rx("RSum = function(MyVector) { return(sum(MyVector)) }");
	var RSum = Rd("RSum(Rout)"); 
	print(TO_WINDOW,"\nSum: %.0f",RSum);
}

#endif
#ifdef PYTHONTEST //////////////////////////////////////////
DLLFUNC int main()
{
	if(!pyStart("",1)) {
		printf("Error - Python won't start!");
		return 0;
	}
	
	var Vec[5] = { 0,1,2,3,4 };
	
	pySet("PyVec",Vec,5);
	pyX("for i in range(5): PyVec[i] *= 10\n");
	pyVec("PyVec",Vec,5);
	
	int i;
	printf("\nReturned: ");
	for(i=0; i<5; i++) printf("%.0f ",Vec[i]);

// test a function
	pyX("def PySum(V):\n Sum = 0\n for X in V: Sum += X\n return Sum\n\n");
	pyX("Result = PySum(PyVec)");
	printf("\nSum: %.0f",pyVar("Result"));
}
#endif

#ifdef OPTIONS ////////////////////////////////////////
#include <contract.c>

#define PREMIUM	3.00
#define DAYS	6*7 // expire after 6 weeks 

DLLFUNC void run() 
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
#endif // OPTIONS

#ifdef DEEPLEARN //////////////////////////////////////
#include <r.h>

var change(int n)
{
	return scale((priceC(0) - priceC(n)) / priceC(0), 100) / 100;
}

var range(int n)
{
	return scale((HH(n,0) - LL(n,0)) / priceC(0), 100) / 100;
}

DLLFUNC void run()
{
	NumCores = -1;
	Script = "DeepLearn";
	StartDate = 20150101;
	BarPeriod = 60;	// 1 hour
	LookBack = 100;
	WFOPeriod = 252 * 24; // 1 year
	DataSplit = 90;
	Verbose = 3+DIAG;
	set(RULES,TESTNOW,PLOTNOW);
	set(LOGFILE);

	assetList("AssetsFix");
	asset("EUR/USD");
	Spread = RollLong = RollShort = Commission = Slippage = 0;
	LifeTime = 3;
	if(Train) Hedge = 2;
	var Threshold = 0.5;

	var X = adviseLong(NEURAL|BALANCED, 0,
		change(1), change(2), change(3), change(4),
		range(1), range(2), range(3), range(4));
	if(X > Threshold) {
		enterLong();
	}
	if(adviseShort() > Threshold)
		enterShort();
}
#endif // DEEPLEARN

#ifdef DLLTEST //////////////////////////////////////
DLLFUNC var square(var Arg)
{
	return Arg * Arg;
}

/*Call from a lite-C script:
var square(var Arg);
API(square,Strategy\\MyStrategy)

void main()
{
	if(!square)
		printf("No square!");
	else
		printf("Square(3): %.3f",square(3));
}
*/
#endif

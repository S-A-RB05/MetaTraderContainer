// Deep Learning Test ///////////////////////////////////////////

//#define DO_SIGNALS  // generate sample set in Train mode
//#define DEEPNET
//#define H2O 
//#define MXNET
#define KERAS

///////////////////////////////////////////////////////////////////////
#include <r.h>

var change(int n)
{
	return scale((priceClose(0) - priceClose(n))/priceClose(0),100)/100;
}

var range(int n)
{
	return scale((HH(n) - LL(n))/priceClose(0),100)/100;
}

///////////////////////////////////////////////////////////////////////

function run()
{
	NumCores = -1;
#ifdef DEEPNET	
	if(Init) print(TO_WINDOW,"\nR and Deepnet required"); 
#endif
#ifdef H2O
	if(Init) print(TO_WINDOW,"\nR and H2O required"); 
	Script = "DeepLearnH2O"; 
	NumCores = 0;	// H2O is single instance only
#endif
#ifdef MXNET
	if(Init) print(TO_WINDOW,"\nR and MxNet required"); 
	Script = "DeepLearnMX";
#endif	
#ifdef KERAS
	if(Init) print(TO_WINDOW,"\nR and Keras required"); 
	Script = "DeepLearnKeras";
#endif	
	StartDate = 20150101;
	BarPeriod = 60;	// 1 hour
	LookBack = 100;

	WFOPeriod = 252*24; // 1 year
	DataSplit = 90;

	assetList("AssetsFix");
	asset("EUR/USD");
	set(RULES);
	Spread = RollLong = RollShort = Commission = Slippage = 0;
	LifeTime = 3;
	if(Train) Hedge = 2;
	
	var Threshold = 0.5;
	var vLong,vShort;
///////////////////////////////////////////////////////////	
#ifdef DO_SIGNALS
	SelectWFO = -1; // use the last WFO cycle for calibrating the neural net
	if((vLong = adviseLong(SIGNALS+BALANCED,0,
#else
	set(LOGFILE|PLOTNOW);
	if((vLong = adviseLong(NEURAL+BALANCED,0,
#endif
		change(1),change(2),change(3),change(4),
		range(1),range(2),range(3),range(4))) > Threshold)	
		enterLong();
#ifndef DO_SIGNALS
	if((vShort = adviseShort()) > Threshold)
		enterShort();
#endif
	plot("Long",vLong,NEW|LINE,BLACK);
	plot("Short",vShort,LINE,GREY);
}

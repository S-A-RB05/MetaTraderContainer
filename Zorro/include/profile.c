// Season & Distribution Analysis //////////////////////////
#ifndef profile_c
#define profile_c

#include <default.c>

#define PDIFF	(1<<0) 
#define PMINUS	(1<<1) 
#define PVAL	(1<<2) 
#define PDEV	0.25	// StdDev with 1/4 amplitude
#define COLOR_AVG		0xee0000
#define COLOR_DEV		0x888888
#define COLOR_PROFIT	0x8888ff

void plotSeason(
	int n,		// plotted bar number
	int label,	// bar label
	int season, // season number
	var value,	// profit to plot
	int type)		// cumulative or difference
{
	if(is(INITRUN) || !is(TESTMODE)) return; // [Test] only

	static int lastseason = 0;
	static var value0 = 0;
	if(!(type&PVAL) && season != lastseason) {
		value0 = value;
		lastseason = season;
	} 

	plotBar("StdDev",n,NIL,(value-value0)*PDEV,DEV|BARS,COLOR_DEV);	
	plotBar("Value",n,label,value-value0,AVG|BARS,COLOR_AVG);	

	if(type&PDIFF) value0 = value;
}

void plotYear(var value,int type)
{
	int periods = 52*5; // avg trading days per year
	checkLookBack(periods);
	int n = (week(0)-1)*5 + dow(0)-1;	// the bar number to plot
	if(n > periods) return;
	plotSeason(n,month(0),year(0),value,type);
}

void plotMonth(var value,int type)
{
	int periods = 22; // avg trading days per month
	checkLookBack(periods);
	int n = (periods*tdm(0))/max(periods,tom(0));
	//int n = (periods*day())/max(periods,dom());
	if(n > periods) return;
	plotSeason(n,n,month(0),value,type);
}

void plotWeek(var value,int type)
{
	if(BarPeriod < 1440) {
		int periods = (4*24 + 22);
		checkLookBack(periods);
		int h = hour(0) + 24*(dow(0)-1);
		if(h > periods) return;
		plotSeason(h,hour(0),week(0),value,type);
	} else
		plotSeason(2*dow(0),dow(0),week(0),value,type);
}

void plotDay(var value,int type)
{
	int periods = 4*24;
	checkLookBack(periods);
	//int m30 = 2*hour(0) + minute(0)/30;
	int m15 = 4*hour(0) + minute(0)/15;
	if(m15 > periods) return;
	plotSeason(m15,hour(0),dow(0),value,type);
}

void plotWFOCycle(var value,int type)
{
	int days = WFOBar*BarPeriod/1440;
	int size = 70;
	int DaysPerBar = g->nTestFrame*BarPeriod/1440/size;
	plotSeason(days/max(1,DaysPerBar),days,WFOCycle,value,type);
}

// plot price difference profile
void plotPriceProfile(int bars,int type)
{
	if(!is(TESTMODE)) return; // [Test] only
	if(!bars) bars = 50;
	set(PLOTNOW,PEEK); // peek in the future 
	var vProfit;
	int n;
	for(n = 1; n < bars; n++) 
	{
		if((type&PDIFF) && type > 0)
			vProfit = (price(-n-1)-price(-n))/PIP;
		else
			vProfit = (price(-n-1)-price(-1))/PIP;
		if((type&PMINUS) || type < 0) 
			vProfit = -vProfit;
		plotBar("StdDev",n,NIL,vProfit*PDEV,DEV|BARS,COLOR_DEV);
		plotBar("Price",n,n,vProfit,AVG|BARS|LBL2,COLOR_AVG);
	}
}

// convert trade profit to pips
var toPIP(var x) { return x/TradeUnits/PIP; }

void plotTradeProfile(int bars)
{
	if(!is(TESTMODE)) return; 
	g->dwStatus |= PLOTSTATS;
	if(is(EXITRUN))
	{
		if(!bars) bars = 50;
	
		var vWinMax = 0, vLossMax = 0;
		for(all_trades) if(TradeIsClosed) { // calculate minimum & maximum profit in pips
			vWinMax = max(vWinMax,toPIP(TradeResult));
			vLossMax = max(vLossMax,-toPIP(TradeResult));
		}
		if(vWinMax == 0 && vLossMax == 0) return;
		
		var vStep;
		if(bars < 0) // bucket size in pips
			vStep = -bars;
		else
			vStep = 10*(int)max(1.,(vWinMax+vLossMax)/bars/10);
		
		int n0 = ceil(vLossMax/vStep);
		for(all_trades) if(TradeIsClosed) 
		{
			var vResult = toPIP(TradeResult);
			int n = floor(vResult/vStep);
			plotBar("Profit",2*(n+n0),n*vStep,abs(vResult),SUM|BARS|LBL2,COLOR_PROFIT);
			plotBar("Number",2*(n+n0)+1,NIL,1,SUM|BARS|AXIS2,COLOR_DEV);
		}
	}
}

// plot a dot from the trade result to a scatter plot
void _plotDot(var X,var Y)
{
	if(!TradeIsClosed) return;
	if(Y > 0)
		plotGraph("Win",X,Y,DOT|GRAPH,GREEN);
	else
		plotGraph("Loss",X,Y,DOT|GRAPH,RED);
}

void plotMAEGraph(int bars)
{
	if(Test && is(EXITRUN))
		for(all_trades) 
			_plotDot(TradeMAE/PIP,toPIP(TradeProfit));
}

void plotMFEGraph(int bars)
{
	if(Test && is(EXITRUN))
		for(all_trades) 
			_plotDot(TradeMFE/PIP,toPIP(TradeProfit));
}

//bars > 0  Number of bars to plot. The more bars, the finer the resolution. 
//bars < 0  Step width of the MAE distribution in thousandths of percentage points of trade value (i.e. 0.001%). 
//The smaller the step width, the finer the resolution. 

void plotMAEPercentGraph(int bars)
{
	if(Test && is(EXITRUN))
		for(all_trades) if(TradeIsClosed)  
			_plotDot(100.*TradeMAE/TradePriceOpen,
				100*TradeProfit/TradeLots/TradeUnits/TradePriceOpen);
}

void plotMFEPercentGraph(int bars)
{
	if(Test && is(EXITRUN))
		for(all_trades) if(TradeIsClosed)  
			_plotDot(100.*TradeMFE/TradePriceOpen,
				100*TradeProfit/TradeLots/TradeUnits/TradePriceOpen);
}

void plotTradeGraphXY(int bars,int Nx,int Ny)
{
	if(!is(TESTMODE)) return; 
	if(Nx < 0 || Nx > 8 || Ny < -1 || Ny > 7) return;
	g->dwStatus |= PLOTSTATS;
	if(!is(EXITRUN)) return;
	if(!bars) bars = 100;

	var vMax = 0,vMin = 999999;
	for(all_trades) if(TradeIsClosed) { // calculate x range#
		if(TradeVar[Nx] == 0.) continue;
		vMax = max(vMax,TradeVar[Nx]);
		vMin = min(vMin,TradeVar[Nx]);
	}

	var vStep;
	if(bars < 0) // bucket size
		vStep = -bars;
	else
		vStep = 10*(int)max(1.,(vMax-vMin)/bars/10);
	
	for(all_trades) if(TradeIsClosed)
	{
		if(TradeVar[Nx] == 0.) continue;
		var vResult = ifelse(Ny == -1,TradeProfit,TradeVar[Ny]);
		var vBar = TradeVar[Nx]/vStep;
		int n = floor(vBar);
		plotBar("Profit",n,n*vStep,0,AVG|BARS|LBL2,COLOR_PROFIT);
		if(TradeVar[Ny] > 0)
			plotGraph("Win",vBar,TradeVar[Ny],DOT,GREEN);
		else
			plotGraph("Loss",vBar,TradeVar[Ny],DOT,RED);
	}
}


void plotProfit(int period)
{
	static var Profit = 0;
	static var PeriodProfit = 0;
	
	if(is(INITRUN)) Profit = Equity;
	if(period == 1) {
		PeriodProfit = Equity - Profit;
		Profit = Equity;
	} else if(period == 2) 
		PeriodProfit = 0;
	plot("Win",max(0,PeriodProfit),NEW|BARS,ColorEquity);
	plot("Loss",min(0,PeriodProfit),BARS,ColorDD);
}

void plotDayProfit()
{
	int period = 0;
	if(day(0) != day(1))
		period = 1;
	else if(hour(0) >= 16)
		period = 2;
	plotProfit(period);
}

void plotWeekProfit()
{
	int period = 0;
	if(week(0) != week(1))
		period = 1;
	else if(dow(0) >= THURSDAY)
		period = 2;
	plotProfit(period);
}

void plotMonthProfit()
{
	int period = 0;
	if(month(0) != month(1))
		period = 1;
	else if(day(0) >= 20)
		period = 2;
	plotProfit(period);
}

void plotQuarterProfit()
{
	int period = 0;
	if(month(0) != month(1) && month(0)%3 == 1)
		period = 1;
	else if(month(0)%3 == 0)
		period = 2;
	plotProfit(period);
}

void plotWFOProfit()
{
	int period = 0;
	if(WFOBar == 0)
		period = 1;
	else if(WFOBar >= 3*g->nTestFrame/4)
		period = 2;
	plotProfit(period);
}

///////////////////////////////////////////////
void plotCorrel(int start, int lag)
{
	DATA *P1 = plotData("Correl1");
	DATA *P2 = P1;
	if(0 == start)
		P2 = plotData("Correl2");
	int i;
	for(i=start; i<lag; i++) {
		var Corr = Correlation(
			P1->Data+1,P2->Data+1+i,P1->end-1-lag);
			plotBar("Correlation",i,i,Corr,BARS|LBL2,RED);
	}
}

void plotCorrelogram(var Val1,var Val2,int lag)
{
	plot("Correl1",Val1,0,0);
	plot("Correl2",Val2,0,0);
	if(is(EXITRUN))
		plotCorrel(0,lag);
}

void plotCorrelogram(var Val,int lag)
{
	plot("Correl1",Val,0,0);
	if(is(EXITRUN))
		plotCorrel(1,lag);
}

void plotHeatmap(const char* name,var* Data,int Rows,int Cols)
{
	if(!is(STRAIGHT)) {
		if(is(TRADEMODE) || (is(TRAINMODE) && mode(PARAMETERS|RULES))) return;
		if(!is(EXITRUN)) return;
	}
	int i,j;
	if(!name) name = "Heatmap";
#ifdef OLDMETHOD
	if(isf(PlotMode,PL_FILE)) {
// set the x/y scale according to number of rows
		PlotScale = 300/Rows;	
		if(Rows >= 20) PlotScale = 450./Rows;
		if(Rows >= 40) PlotScale = 600./Rows;
		if(Cols*PlotScale > PlotWidth)
			PlotScale = PlotWidth/Cols;
		PlotWidth = PlotScale*Cols;
		PlotHeight1 = PlotScale*(Rows+1);
	} else {
		PlotWidth = 600;
		PlotScale = PlotHeight1/Rows;	
	}
// plot x axis
	for(i=0; i<=PlotWidth/PlotScale; i++)
		plotBar(name,i+1,i+1,0,0,0xFFFFFF);
// plot the squares
	for(i=0; i<Cols; i++)
		for(j=0; j<Rows; j++) {
			//print(TO_ANY,"\nHeat: %i %i %.2f",i,j,Data[i*Rows+j]);
			plotGraph("Heat",i+1,-j-1,SQUARE|STATS,color(Data[i*Rows+j]*100.,BLUE,RED,0,0));
		}
#else
	dataNew(1,Rows,Cols);
	for(i=0; i<Cols; i++)
		for(j=0; j<Rows; j++) 
			dataSet(1,j,i,Data[i*Rows+j]);
	dataChart(1,name,HEATMAP,NULL);
#endif
}

void plotHeatmap(const char* name,mat M)
{ plotHeatmap(name,M->dat,M->rows,M->cols); }

void plotHistogram0(string Name,var Val,var Step,var Weight,int Color)
{
	var Bucket = floor(Val/Step);
	static var BucketMax = 0, BucketMin = 999999;
	if(Color == -1) { // end the histogram
		int i;
		for (i = BucketMin; i < BucketMax; i++)
			plotBar("#H", i, Step * i, 1, BARS + LBL2, 0xFF000000); // print labels
		BucketMax = 0; BucketMin = 999999;
	} else {
		BucketMax = max(BucketMax,Bucket);
		BucketMin = min(BucketMin,Bucket);
		plotBar(Name,Bucket,Step*Bucket,Weight,SUM+BARS+LBL2,Color);
	}
}

void plotBuyHold(string BenchMark,int Color)
{
	setf(PlotMode,PL_BENCHMARK);
	static var PriceBuy = 0;
	if(is(LOOKBACK)) {
		PriceBuy = priceC(0);
		return;
	}
	if(BenchMark)
		asset(BenchMark);
	var Hold = Leverage*(priceC(0)-PriceBuy);
	if(Capital > 0)
		Hold *= Capital/PriceBuy;
	else
		Hold *= Lots;
	plot("Benchmark",Hold,LINE|AXIS2,Color);
	if(BenchMark)
		asset(AssetPrev);
}


///////////////////////////////////////////////
//#define PTEST
#ifdef PTEST
function run()
{
	vars Price = series(price());
	vars Trend = series(LowPass(Price,1000));
	
	Stop = 100*PIP;
	TakeProfit = 100*PIP;
	if(valley(Trend)) {
		//plotPriceProfile(50,0);
		enterLong();
	} else if(peak(Trend)) {
		//plotPriceProfile(50,PMINUS);
		enterShort();
	}

	PlotWidth = 1000;
	PlotHeight1 = 320;
	PlotScale = 4;
	
	//plotDay(Equity,1); 
	//plotWeek(Equity,1); 
	//plotMonth(Equity,1); 
	//plotYear(Equity,1); 
	plotTradeProfile(50);
}
#endif
#endif
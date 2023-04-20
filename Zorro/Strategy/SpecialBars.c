// Special bars example //////////////////////////////

#define MULTI_ASSET

var BarRange; 

// Range Bars
int Range(vars Open,vars High,vars Low,vars Close)
{
  if(Open[0] != Close[1]) {
    High[0] = max(Open[0],Close[1]);
    Low[0] = min(Open[0],Close[1]);
    Open[0] = Close[1];
  }
  if(High[0]-Low[0] >= BarRange)
    return 1;
  return 4;
}

// Renko Bars, variant 1 
int Renko1(vars Open,vars High,vars Low,vars Close)
{
  Open[0] = roundto(Close[1],BarRange);
  if(Close[0]-Open[0] >= BarRange) {
    Close[0] = Open[0]+BarRange;
    High[0] = Close[0];
    Low[0] = Open[0];
    return 1;
  }
  if(Open[0]-Close[0] >= BarRange) {
    Close[0] = Open[0]-BarRange;
    High[0] = Open[0];
    Low[0] = Close[0];
    return 1;
  }
  return 4;
}

// Renko Bars, variant 2 
int Renko2(vars Open, vars High, vars Low, vars Close)
{
  var OpenDiff = abs(Close[0]-Open[1]);
  var CloseDiff = abs(Close[0]-Close[1]);
  if(OpenDiff < CloseDiff) // we have a valley or peak
     Open[0] = Open[1];
  else  // we are moving with the trend
     Open[0] = roundto(Close[1],BarRange);
  if(Close[0]-Open[0] >= BarRange) {  // going up
    Close[0] = Open[0]+BarRange;
    High[0] = Close[0];
    Low[0] = Open[0];
    return 1;
  }
  if(Open[0]-Close[0] >= BarRange) { // going down
    Close[0] = Open[0]-BarRange;
    High[0] = Open[0];
    Low[0] = Close[0];
    return 1;
  }
  return 4;
}

// Mean Renko Bars
int Renko3(vars Open, vars High, vars Low, vars Close)
{
  Open[0] = 0.5*(Close[1]+Open[1]);
  if(Close[0] <= Open[0] - BarRange) {
    Close[0] = Open[0] - BarRange;
    return 1;
  } else if(Close[0] >= Open[0] + BarRange) {
    Close[0] = Open[0] + BarRange;
    return 1;
  }
  return 4;
}

// Haiken Ashi Bars
int HA(vars Open,vars High,vars Low,vars Close)
{
  Close[0] = (Open[0]+High[0]+Low[0]+Close[0])/4;
  Open[0] = (Open[1]+Close[1])/2;
  High[0] = max(High[0],max(Open[0],Close[0]));
  Low[0] = min(Low[0],min(Open[0],Close[0]));
  return 8;
}

// Point-and-Figure Bars
int PAF(vars Open,vars High,vars Low,vars Close)
{
  static int direction = 0;
  if(direction == 1 && High[0]-Close[0] >= BarRange) {
    Open[0] = roundto(Low[0],BarRange);
    Close[0] = roundto(High[0],BarRange);
    Low[0] = Open[0];
    High[0] = Close[0];
    direction = 0;
    return 1;
  }
  if(direction == 0 && Close[0]-Low[0] >= BarRange) {
    Open[0] = roundto(High[0],BarRange);
    Close[0] = roundto(Low[0],BarRange);
    High[0] = Open[0];
    Low[0] = Close[0];
    direction = 1;
    return 1;
  }
  return 4;
}

// prototype to be set to one of the above functions
int _bar(vars Open,vars High,vars Low,vars Close); 

// strategy to be traded /////////////////////////////////////////

// simnple trend following strategy 
void strategy(vars Prices) 
{
	vars Trends = series(LowPass(Prices,20));
	plot("Trend",Trends,LINE,BLUE);
	Stop = 200*PIP;
	TakeProfit = 200*PIP;
	MaxLong = MaxShort = 1;
	if(crossOver(Prices,Trends))
		enterLong();
	if(crossUnder(Prices,Trends))
		enterShort();
}

#ifndef MULTI_ASSET // single asset user bars ////////////////////

int bar(vars Open,vars High,vars Low,vars Close) 
{
	return _bar(Open,High,Low,Close);
} 

void run()
{
	_bar = Renko1;
	BarRange = 0.003;	// set all bar parameters before first asset() call
	BarPeriod = 15;	// determines only the number of allocated bars, not the bar period
	StartDate = 20180601;
	EndDate = 20180901;
	set(PLOTNOW);

	asset("EUR/USD");
	strategy(series(priceClose()));
}

#else // multi asset user bars ///////////////////////////////////
// helper functions - bar() cannot be used for multi asset
vars Open,High,Low,Close; // global price series

// set fixed start conditions, since most range-based bars 
// produce random price curves dependent on initial price
var setup(var Price)
{ 
	if(is(FIRSTRUN) || is(INITRUN))
		return roundto(priceClose(),10000*PIP);
	else return Price; 
}

// disable all series auto-shifting until a new bar starts
int newbar()
{
	set(NOSHIFT|OFF);
	Open = series(setup(priceOpen()));
	High = series(setup(priceHigh()));
	Low = series(setup(priceLow()));
	Close = series(setup(priceClose())); // _bar needs shifted price series
	if(_bar(Open,High,Low,Close) == 1 || is(INITRUN) || is(FIRSTRUN)) {
		return 1;
	} else { // bar still open
		set(NOSHIFT); 				// prevent series auto-shifting 
		shift(Open,0,-LookBack); // shift price series back
		shift(High,0,-LookBack);
		shift(Low,0,-LookBack);
		shift(Close,0,-LookBack);
		return 0;
	}
}
//////////////////////////////////////////////////////////////////

void run()
{
	_bar = Renko1;	
	BarRange = 0.003;	// set all bar parameters before first asset() call
	BarPeriod = 1; // determines the time resolution of the bar
	StartDate = 20180601;
	EndDate = 20180901;
	ignore(57);
	LookBack = 80;
	set(PLOTNOW,LOGFILE);
	ColorUp = ColorDn = 0;	// plot no standard candles

	while(asset(loop("EUR/USD","GBP/USD","EUR/CHF"))) 
	{
		int NewBar = newbar();
		if(NewBar) // start of new renko bar
			printf("#\nNew %s Renko bar, OHLC %f %f %f %f",
				Asset,Open[0],High[0],Low[0],Close[0]);
		plot("Renko",Close,0,BLACK);
		strategy(Close); // use global price series instead of price functions
	}
}
#endif
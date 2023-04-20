// Indicator Source Code ////////////////////////
// (c) oP group 2014,2022
/////////////////////////////////////////////////
// This file contains the source code of indicators.
// It is for INFORMATION ONLY. YOU CANNOT MODIFY IT -
// well you could, but it had no effect...
// For modifications or variants of the indicators, 
// copy the function in your script, and edit it there.
// It can normally be easily adapted to lite-C.
// Some indicators contain functions or macros for 
// debugging and testing purposes; remove them for 
// compiling the code under lite-C.

#ifndef indicators_c
#define indicators_c

// Helper functions and definitions /////////////

// percentage of a variable within a band
var percent(var a,var l,var h) { 
	if(l == h) return 50.;
	return 100.*(a-l)/(h-l); 
}

// clip away the center range of a percent value
var percentClip(var a,var border)
{
// at the lower end, return the difference to 0
	if(a < border) return a-border; 
// at the upper end, return the difference to 100
	if(a > 100.-border) return a-(100.-border);
	return 0.; // center area
} 

// convert a smoothing factor to a period
//inline int smoothT(var a) { return (int)(2./a-1.); }
inline var smoothF(int period) { return 2./(period+1); }

//inline var deg(var x) { return (180./PI)*x; }

// Indicators /////////////////////////////////////////

// Highest High
var HH(int Period,int Offset) 
{ 
	if(_ispointer(Period)) // common conversion trap
		return _error011("HH");
	var vHH = 0.;
	for(int i = Offset; i < Period+Offset; i++) {
		var Price = priceHigh(i);
		if(vHH < Price)
			vHH = Price, g->vMaxIdx = i,
			g->nHHBar = g->nBar - (i*TIMEFRAME);
	}
	return vHH;
}

//Lowest Low
var LL(int Period,int Offset) 
{ 
	if(_ispointer(Period))
		return _error011("LL");
	var vLL = 9999999;
	for(int i = Offset; i < Period+Offset; i++) {
		var Price = priceLow(i);
		if(Price > 0. && vLL > Price)
			vLL = Price, g->vMinIdx = i,
			g->nLLBar = g->nBar - (i*TIMEFRAME);
	}
	if(vLL > 9999998) vLL = 0.; // prices were all zero
	return vLL;
}

int MatchIndex(var* Data,var Val,int Period) 
{ 
	var Closest = 9999999;
	int Idx = 0;
	for(int i = 0; i < Period; i++) {
		var Dist = abs(Data[i]-Val); 
		if(Dist < Closest)
			Closest = Dist, Idx = i;
	}
	return Idx;
}

var SumSq(vars Data,int Length)
{
	var Ex = 0;
	for(int i=0; i<Length; i++)
		Ex += Data[i]*Data[i];
	return Ex;
}

var Pivot(int TimePeriod,int Offset)
{
	return (HH(TimePeriod,Offset)+LL(TimePeriod,Offset)+priceClose(Offset))/3.;
}

var RET(int TimePeriod)
{
	if(priceClose(TimePeriod) <= 0.) return 0.;
	return (priceClose(0)-priceClose(TimePeriod))/priceClose(TimePeriod);
}

// Simple ATR based on SMA
var ATRS(int Period)
{
	Period = Max(1,Period);
	var vATRS =  0;
	for(int i = 0; i < Period; i++)
		vATRS += priceHigh(i)-priceLow(i);
	return vATRS/Period;
}

// Granvilles's On Balance Volume
var OBV(var* Data,var Volume)
{
	var* OBVs = series(Volume,2);
	if(Data[0] > Data[1])
		return OBVs[0] = OBVs[1]+Volume;
	else if(Data[0] < Data[1])
		return OBVs[0] = OBVs[1]-Volume;
	else
		return OBVs[0] = OBVs[1];
}

var SMMA(vars Data,int TimePeriod)
{
	int Period = Min(g->nBar,g->nUnstablePeriod);
	var Ex = SMA(Data+Period,TimePeriod);
	if(TimePeriod > 1)
		for(int i=Period-1; i>=0; i--)
			Ex = (Ex*(TimePeriod-1) + Data[i])/TimePeriod;
	return Ex;
}

// Zorro's Market Meanness Index
var MMI(var* Data,int TimePeriod)
{
// clip time period to history length
	TimePeriod = Min(TimePeriod,1000);
	checkLookBack(TimePeriod);
	TimePeriod = Min(TimePeriod,g->nBar-1);
	if(TimePeriod < 2) return 75;
// calculate MMI statistics
	var m = Median(Data,TimePeriod); 
	int i, nh=0, nl=0;
	for(i=1; i<TimePeriod; i++) {
		if(Data[i] > m && Data[i] > Data[i-1])
			nl++;
		else if(Data[i] < m && Data[i] < Data[i-1])
			nh++;
	}
	return 100.*(nl+nh)/(TimePeriod-1);
}

// Bill Williams Fractal Indicator
var FractalLow(var* Data,int Period)
{
	if(Period < 3) return *Data;
	int Center = (Period-1)/2;
	if(MinIndex(Data,Period) == Center)
		return Data[Center];
	else return 0.;
}

var FractalHigh(var* Data,int Period)
{
	if(Period < 3) return *Data;
	int Center = (Period-1)/2;
	if(MaxIndex(Data,Period) == Center)
		return Data[Center];
	else return 0.;
}

// Donchian Channel
var DChannel(int Period)
{
	g->vRealUpperBand = HH(Period,0);
	g->vRealLowerBand = LL(Period,0);
	return g->vRealUpperBand;
}

// Donchian Oscillator
var DCOsc(var* Data,int Period)
{
	DChannel(Period);
	return percent(Data[0],g->vRealLowerBand,g->vRealUpperBand);
}

// Bollinger Bands Oscillator
var BBOsc(var* Data,int Period,var NbDev,int MAType)
{
	BBands(Data,Period,NbDev,NbDev,MAType);
	return percent(Data[0],g->vRealLowerBand,g->vRealUpperBand);
}

// Trend Strength Index, from Engineering Returns 
var TSI(var* Data,int Period)
{
	var* ratio = series(abs(Data[0]-Data[Period])/ATR(Period),0);
	var* TSI = series(SMA(series(SMA(ratio,Period),0),10*Period),0);
	SETSERIES(Data,Period);
	return *TSI;
}

// Accumulation/distribution indicator
var ADO() 
{
	return (2*priceClose(0)-priceHigh(0)-priceLow(0))
		/Max(0.0001,priceHigh(0)-priceLow(0));
}

int addWeekend(int day) 
{
	int d = dow(0)-day;
	while(d <= 0) d += 7;
	if(d == SATURDAY) day += 1;
	if(d == SUNDAY) day += 2;
	return day;
}

// High, Low, Open, Close of a trading day in a certain time zone
var dayHigh(int zone,int day)
{
	if(IS(INITRUN)) return 0;
	day = addWeekend(day);
	int evening = timeOffset(zone,day,g->nEndMarket/100,g->nEndMarket%100);
	int morning = timeOffset(zone,day,g->nStartMarket/100,g->nStartMarket%100);
	if(evening >= morning) return priceHigh(Max(0,evening)); 
	var vHH = 0;
	for(int i = evening; i <= morning; i++)
		vHH = Max(vHH,priceHigh(i));
	return vHH;
}

var dayLow(int zone,int day)
{
	if(IS(INITRUN)) return 0;
	day = addWeekend(day);
	int evening = timeOffset(zone,day,g->nEndMarket/100,g->nEndMarket%100);
	int morning = timeOffset(zone,day,g->nStartMarket/100,g->nStartMarket%100);
	if(evening >= morning) return priceLow(Max(0,evening));
	var vLL = 999999;
	for(int i = evening; i <= morning; i++)
		vLL = Min(vLL,priceLow(i));
	return vLL;
}

var dayClose(int zone,int day)
{
	if(IS(INITRUN)) return 0;
	day = addWeekend(day);
	return priceClose(timeOffset(zone,day,g->nEndMarket/100,g->nEndMarket%100)); 
}

var dayOpen(int zone,int day)
{
	if(IS(INITRUN)) return 0;
	day = addWeekend(day);
	return priceOpen(timeOffset(zone,day,g->nStartMarket/100,g->nStartMarket%100)); 
}

var dayPivot(int zone,int day)
{
	if(IS(INITRUN)) return 0;
	return (dayHigh(zone,day)+dayLow(zone,day)+dayClose(zone,day))/3;
}

//"Internal Bar Strength"
var IBS()
{
	var Low = priceLow(0), High = priceHigh(0);
	if(High == Low) return 0.5;
	return (priceClose(0) - Low)/(High - Low);
}

// Filters /////////////////////////////////////////////

var FIR3(var* Data)
{
	return (Data[0]+2.*Data[1]+Data[2])/4.;
}

var FIR4(var* Data)
{
	return (Data[0]+2.*Data[1]+2.*Data[2]+Data[3])/6.;
}

var FIR6(var* Data)
{
	return (Data[0]+2.*Data[1]+3.*Data[2]+3.*Data[3]+2.*Data[4]+Data[5])/12.;
}

// normalize a value to the -1..+1 range
var Normalize(var* Data,int Period)
{
	Period = Max(2,Period);
	var vMin = MinVal(Data,Period);
	var vMax = MaxVal(Data,Period);
	if(vMax>vMin) 
		return 2.*(*Data-vMin)/(vMax-vMin) - 1.;
	else return 0.;
}

// Fisher Transform
var Fisher(var* Data)
{
	var v = Clamp(Data[0],-0.998,0.998);
	return 0.5*log((1.+v)/(1.-v));
}

// Normalized Fisher transform
var FisherN(var* Data,int Period)
{
	var *Value = series(*Data,2), *FN = series(*Data,2);
	Value[0] = 0.33*Normalize(Data,Period) + 0.67*Value[1];
	return FN[0] = Fisher(Value) + 0.5*FN[1];
}

// Inverse Fisher Transform
var FisherInv(var* Data)
{
	var Exp = exp(2.*Data[0]);
	return (Exp-1.)/(Exp+1.);
}

// 3-pole Butterworth filter
var Butterworth(var *Data,int Cutoff)
{
	if(!Cutoff) Cutoff = 8;
	var a = exp(-PI / Cutoff);
	var b = 2*a*cos(1.738*PI / Cutoff);
	var c = a*a;
	var c1 = b + c;
	var c2 = -(c + b*c);
	var c3 = c*c;
	var c0 = 1 - c1 - c2 - c3;

	var* Filt = series(*Data,4);
	SETSERIES(Data,0);
	return Filt[0] = c0*Data[0] + c1*Filt[1] + c2*Filt[2] + c3*Filt[3];
}

// Automatic gain control
var AGC1(var *Data, int Period);
var AGC0(var *Data, var alpha)
{
	if(alpha > 1.) return AGC1(Data,(int)alpha);	// for calling with optimize()
	vars G = series(Data[0],2);
	G[0] = Max(abs(Data[0]),alpha*G[1]);
	SETSERIES(Data,0);
	if(G[0] == 0.) return 0.;
	return Data[0]/G[0];
}

var AGC1(var *Data, int Period)
{
	var a = 0.991;
	if(Period > 0) a = pow(0.841,2./Period);
	return AGC0(Data,a);
}

// Highpass filter by John F. Ehlers, converted by DdlV
// http://www.mesasoftware.com/technicalpapers.htm
var HighPass2(var* Data,int CutoffPeriod) 
{
// alpha1 = (Cosine(.707*360 / 48) + Sine (.707*360 / 48) - 1) / Cosine(.707*360 / 48);
// HP = (1 - alpha1 / 2)*(1 - alpha1 / 2)*(Close - 2*Close[1] + Close[2]) + 2*(1 - alpha1)*HP[1] - (1 - alpha1)*(1 - alpha1)*HP[2];
	if(!CutoffPeriod) CutoffPeriod = 48;
	var a	= (0.707*2*PI)/CutoffPeriod;
	var alpha1 = 1.+(sin(a)-1.)/cos(a);
	var b	= 1.-alpha1/2.;
	var c	= 1.-alpha1;
	
	var* HP = series(0,3);
	SETSERIES(Data,0);
	return HP[0] = b*b*(Data[0]-2*Data[1]+Data[2])
		+2*c*HP[1]
		-c*c*HP[2];
}

// Ehlers' Decycler, S&C 9/2015
var Decycle(var* Data,int Period)
{
	return Data[0]-HighPass2(Data,Period);
}

// Ehlers' smoothing filter ("SuperSmoother"), 2-pole Butterworth * SMA
var Smooth(var *Data,int Cutoff)
{
	if(!Cutoff) Cutoff = 10;
	var f = (1.414*PI) / Cutoff;
	var a = exp(-f);
	var c2 = 2*a*cos(f);
	var c3 = -a*a;
	var c1 = 1 - c2 - c3;

	var* Filt = series(*Data,4);
	SETSERIES(Data,0);
	return Filt[0] = c1*(Data[0]+Data[1])*0.5 + c2*Filt[1] + c3*Filt[2];
}

var Roof(var* Data,int CutOffLow,int CutOffHigh)
{
	vars HP = series(HighPass2(Data,CutOffHigh),2);
	var R = Smooth(HP,CutOffLow);
	SETSERIES(Data,0);
	return R;
}

// Ehler's universal oscillator, from S&C 1/2015
var UO(var *Data,int Cutoff)
{
	vars WhiteNoise = series((Data[0]-Data[2])/2.,2);
	vars SWN = series(Smooth(WhiteNoise,Cutoff),2);
	var A = AGC0(SWN,0.991);
	SETSERIES(Data,0);
	return A;
}

// Predictive Stochastic by John F. Ehlers, converted by DdlV
// http://www.mesasoftware.com/technicalpapers.htm
var StochEhlers(var* Data,int TimePeriod,int CutOffLow,int CutOffHigh) 
{
	if(!TimePeriod) TimePeriod = 20;
	if(!CutOffLow) CutOffLow = 10;
	if(!CutOffHigh) CutOffHigh = CutOffLow;
	checkLookBack(TimePeriod);

	var* HP = series(HighPass2(Data,CutOffHigh),3);
	var* RS = series(Butterworth(HP,CutOffLow),0);
	
	var Highest = MaxVal(RS,TimePeriod);
	var Lowest = MinVal(RS,TimePeriod);
	var* SE = series(0,3);
	if(Highest == Lowest)
		SE[0] = 0;
	else
		SE[0] = (RS[0]-Lowest)/(Highest-Lowest);

	var B = Butterworth(SE,CutOffLow);
	SETSERIES(Data,TimePeriod);
	return B;
}

// Smoothed Momentum
var SMom(var *Data,int Period,int Cutoff)
{
	checkLookBack(Period);
	var *Diff = series(Data[0]-Data[Period],2);
	SETSERIES(Data,Period);
	return Butterworth(Diff,Cutoff);
}

// Zero-lag Moving Average by John Ehlers
var ZMA(var* Data,int Period)
{
	Period = Max(Period,1);
	var a = smoothF(Period);
	g->vEMA = EMA1(Data,a); 
	var *vZMA = series(*Data,2);
	g->vError = 1000000;
	var Gain,GainLimit=5,BestGain=0;
	for(Gain=-GainLimit; Gain<GainLimit; Gain += 0.1)
	{
		vZMA[0] = a*(g->vEMA + Gain*(Data[0]-vZMA[1])) + (1-a)*vZMA[1];
		var Error = Data[0] - vZMA[0];
		if(abs(Error) < g->vError) {
			g->vError = abs(Error);
			BestGain = Gain;
		}
	}
	SETSERIES(Data,Period);
	return vZMA[0] = a*(g->vEMA + BestGain*(Data[0]-vZMA[1])) + (1-a)*vZMA[1];
}

// Center Of Gravity Oscillator
var CGOsc(var* Data,int Period)
{
	var Num=0., Denom=0.;
	for(int i = 0; i < Period; i++) {
		Num += (i+1)*Data[i];
		Denom += Data[i];
	}
	SETSERIES(Data,Period);
	if(Denom > 0.)
		return -Num/Denom + 0.5*(Period+1);
	else
		return 0.;	
}

// Relative Vigor Index
var RVI(int Period)
{
	var *CO = series(FIR4(series(priceClose(0)-priceOpen(0),0)),0);
	var *HL = series(FIR4(series(priceHigh(0)-priceLow(0),0)),0);
	var Denom = SMA(HL,Period);
	if(Denom == 0.) return 0.;
	return SMA(CO,Period) / Denom;
}

// Hull Moving Average
var HMA(var* Data,int n)
{
	int Period = sqrt((var)n)+0.5;
	var H = WMA(series(2*WMA(Data,0.5*n+0.5)-WMA(Data,n),0),Period);
	SETSERIES(Data,Period);
	return H;
}

// Alligator
var Alligator(var* Data)
{
	g->vBlue = SMA(Data+5,13);
	g->vRed = SMA(Data+2,8);
	g->vGreen = SMA(Data,5);
	SETSERIES(Data,13);
	return g->vRed;
}

// Awesome Oscillator
var AO(var* Data) 
{ 
	var R = SMA(Data,5)-SMA(Data,34); 
	SETSERIES(Data,34);
	return R;
}

// Haiken Ashi
var HAClose() { return (priceOpen(0)+priceHigh(0)+priceLow(0)+priceClose(0))/4; }
var HAOpen() { return (priceOpen(1)+priceClose(1))/2; }
var HAHigh() { return Max(priceHigh(0),Max(HAOpen(),HAClose())); }
var HALow() { return Min(priceLow(0),Min(HAOpen(),HAClose())); }

// Accelerator Oscillator: AO - SMA(AO,5)
var AC(var* Data) 
{ 
	return (4*AO(Data)-AO(Data+1)-AO(Data+2)-AO(Data+3)-AO(Data+4))/5.;
}


// Laguerre filter
var Laguerre(vars Data, var alpha)
{
   vars L = series(Data[0],8);
	if(alpha > 1.) alpha = smoothF(alpha);
	var alpha1 = 1.-alpha;
	L[0] = alpha*Data[0] + alpha1*L[1];
	L[2] = -alpha1*L[0] + L[1] + alpha1*L[2+1];
	L[4] = -alpha1*L[2] + L[2+1] + alpha1*L[4+1];
	L[6] = -alpha1*L[4] + L[4+1] + alpha1*L[6+1];
   
	return (L[0]+2.*L[2]+2.*L[4]+L[6])/6.;
}

// series substitution with unstable period
typedef var (*F_SV)(vars,var);
typedef var (*F_SI)(vars,int);
var truncate(F_SV F,vars Data,var Param)
{
	if(!MODE2(STRUNC)) return (*F)(Data,Param);
	int Len = g->nUnstablePeriod+16; // 16 for internal series usage
	var *TBuffer = (double*)mem_tempalloc(Len*sizeof(double),MEMTEMP_TSERIES);
	if(TBuffer && Data[0] != 0.) 
		for(int i=0; i<Len; i++) TBuffer[i] = Data[0];
	var Ret = Data[0];
	for(int i=Len-1; i>=0; i--) {
		g->pSeriesBuffer = TBuffer;
		Ret = (*F)(Data+i,Param);
		shift(TBuffer,0,Len);
	}
	return Ret;
}

//var Laguerre(vars Data, var alpha) { return truncate(Laguerre0,Data,alpha); }


var Coral(vars Data)
{
	return T3(Data,60,0.4);
}

// ALMA, Arnaud Legoux Moving Average 
// © acidburn @ Zorro Forum
var ALMA0(var *Data, int Period, int sigma, var offset)
{
	checkLookBack(Period);
	if(sigma == 0) sigma = 6;
	if(offset == 0.) offset = 0.85;
	Period = Max(Period,2);
	var m = floor(offset * (Period - 1));
	var s = Period / sigma;
	var alma = 0., wSum = 0.;
	int i;

	for (i = 0; i < Period; i++) {
		var w = exp(-((i - m) * (i - m)) / (2 * s * s));
		alma += Data[Period - 1 - i] * w;
		wSum += w;
	}
	SETSERIES(Data,Period);	
	return alma / wSum;
}

var ALMA1(var *Data, int TimePeriod) { 
	return ALMA0(Data,Max(TimePeriod,6),0,0.); 
}

var SIROC(var* Data, int TimePeriod, int EMAPeriod) 
{
	if(!EMAPeriod) EMAPeriod = 13;
	checkLookBack(TimePeriod+EMAPeriod);
	var PrevEMA = EMA0(Data+TimePeriod,EMAPeriod);
	if(PrevEMA == 0.) return 0.;
	return 100.*(EMA0(Data,EMAPeriod)-PrevEMA)/PrevEMA; 
}

var DPO(var* Data, int TimePeriod) 
{
	int HalfPeriod = TimePeriod/2 + 1;
	checkLookBack(TimePeriod+HalfPeriod);
	return Data[0] - SMA(Data+HalfPeriod,TimePeriod); 
}

var Keltner(var* Data, int TimePeriod, var Factor)
{
	g->vRealMiddleBand = SMA(Data,TimePeriod);
	var Band = Factor * ATRS(TimePeriod);
	g->vRealUpperBand = g->vRealMiddleBand + Band;
	g->vRealLowerBand = g->vRealMiddleBand - Band;
	return g->vRealMiddleBand;
}

var SAR(var AFstep,var AFmin,var AFmax)
{
#ifndef NIX //_DEBUG
#define _newHigh	v[0]
#define _prevHigh	v[1]
#define _newLow	v[2]
#define _prevLow	v[3]
#define _isLong	v[4]
#define _af			v[5]
#define _ep			v[6]
#define _sar		v[7]
	var* v = series(0,-8); // static series
#else
	static var _newHigh,_prevHigh,_newLow,_prevLow,
		_isLong,_af,_ep,_sar;
#endif

	_prevLow = _newLow;
	_prevHigh = _newHigh;
	_newLow  = priceLow(0);
	_newHigh = priceHigh(0);
	var result = 0.;

	if(IS(FIRSTRUN)) {
		_isLong = priceClose(0) - priceOpen(0);
		_prevLow = _newLow;
		_prevHigh = _newHigh;
		if(_isLong > 0.) {
			_ep  = _newHigh;
			_sar = _newLow;
		} else {
			_ep  = _newLow;
			_sar = _newHigh;
		}
		_af = AFmin;
		return _sar;
	}
	
  if(_isLong > 0.)
  {  
// Switch to short if the low penetrates the SAR value.
		if(_newLow <= _sar)
		{
// Switch and override the SAR with the _ep
// Make sure the SAR is within yesterday's and today's range.
			_isLong = -1.;
			_sar = Max(_ep,Max(_prevHigh,_newHigh));
			result = _sar;
			
// Adjust _af and _ep, Calculate the new SAR
			_af = AFmin;
			_ep = _newLow;
			_sar += _af * (_ep - _sar);
			
// Make sure the new SAR is within yesterday's and today's range.
			_sar = Max(_sar,Max(_prevHigh,_newHigh));
		}
		else // No switch
		{
// Output the SAR (was calculated in the previous iteration)
			result = _sar;
			
// Adjust _af and _ep
			if(_newHigh > _ep) {
				_ep = _newHigh;
				_af = Min(_af+AFstep,AFmax);
			}
			
// Calculate the new SAR
			_sar += _af * (_ep - _sar);
	
// Make sure the new SAR is within yesterday's and today's range.
			_sar = Min(_sar,Min(_prevLow,_newLow));
		}
	}
	else
	{
		if(_newHigh >= _sar)
		{
			_isLong = 1.;
			_sar = Min(_ep,Min(_prevLow,_newLow));
			result = _sar;
			_af = AFmin;
			_ep = _newHigh;
			_sar += _af * (_ep - _sar);
			_sar = Min(_sar,Min(_prevLow,_newLow));
		}
		else
		{
			result = _sar;
			if(_newLow < _ep) {
				_ep = _newLow;
				_af = Min(_af+AFstep,AFmax);
			}
			_sar += _af * (_ep - _sar);
			_sar = Max(_sar,Max(_prevHigh,_newHigh));
		}
	}
	g->vSign = _isLong;
	return result;
}

var ZigZag(var* Data,var Depth,int Length,long Color)
{
#ifndef _DEBUG
	vars v = series(0,-9);
	#define _Dir		v[0]	
	#define _LastBar	v[1]	
	#define _HighBar	v[2]	
	#define _LowBar	v[3]	
	#define _LastPrice v[4]	
	#define _HighPrice v[5]
	#define _LowPrice	v[6]
	#define _Length	v[7]
	#define _Slope		v[8]
#else
	static var _Dir,_LastBar,_HighBar,_LowBar,
		_LastPrice,_HighPrice,_LowPrice,_Slope,_Length;
#endif
	SETSERIES(Data,0);
	var Price = Data[0];
	char PlotName[30];
	if(Color) sprintf(PlotName,"ZigZag_%x",Color);

	if(IS(INITRUN) || IS(FIRSTRUN)) {
		_HighPrice = Price;
		_LowPrice = Price;
		_LastPrice = Price;
		_LastBar = _HighBar = _LowBar = g->nBar;
		_Dir = 0;
		if(Color) {
			plotGraph(PlotName,0,_LastPrice,LINE,Color);
			plotGraph(PlotName,0,_LastPrice,LINE+END,Color);
		}
		g->vLength = 1;
		return g->vSign = g->vPeak = g->vSlope = 0;
	}

// check for up-swing > threshold and down-swing > threshold
	if(_Dir >= 0) 
	{
		if(Price > _HighPrice) { // new high pivot
			/*_LowPrice =*/ _HighPrice = Price;
			/*_LowBar =*/ _HighBar = g->nBar;
		}
// store first low pivot after the high pivot
		else if(Price < _LowPrice) { 
			_LowPrice = Price;
			_LowBar = g->nBar;
		}
		if(_HighPrice - _LastPrice >= Depth
			&& _HighPrice - Price >= Depth
			&& g->nBar - _HighBar >= Length)
		{				
// Up-swing ended, now move down
			if(Color) {
				plotGraph(PlotName,g->nBar-_LastBar,_LastPrice,LINE,Color);
				plotGraph(PlotName,g->nBar-_HighBar,_HighPrice,LINE+END,Color);
			}
// store the slope of the up-swing
			_Length = _HighBar-_LastBar;
			_Slope = (_HighPrice-_LastPrice)/_Length;
// The high becomes a new pivot, and _Dir changes to a down-swing
			_LowBar = _LastBar = _HighBar;
			_LowPrice = _LastPrice = _HighPrice;
			_Dir = -1;
		}
	}

	if(_Dir <= 0) // can happen in the same bar
	{
		if(Price < _LowPrice) {
			_LowPrice = /*_HighPrice =*/ Price;
			_LowBar = /*_HighBar =*/ g->nBar;
		}
		else if(Price > _HighPrice) {
			_HighPrice = Price;
			_HighBar = g->nBar;
		}
		if(_LastPrice - _LowPrice > Depth
			&& Price - _LowPrice > Depth
			&& g->nBar - _LowBar >= Length)
		{
			if(Color) {
				plotGraph(PlotName,g->nBar-_LastBar,_LastPrice,LINE,Color);
				plotGraph(PlotName,g->nBar-_LowBar,_LowPrice,LINE+END,Color);
			}				
			_Length = _LowBar-_LastBar;
			_Slope = (_LowPrice-_LastPrice)/_Length;
			_HighBar = _LastBar = _LowBar;
			_HighPrice = _LastPrice = _LowPrice;
			_Dir = 1;

// fix high price in case the current price was already a high pivot
			if(Price > _HighPrice) {
				_HighPrice = Price;
				_HighBar = g->nBar;
			}
		} 
	}

	g->vLength = _Length;
	g->vPeak = g->nBar-_LastBar;
	g->vSign = _Dir;
	return g->vSlope = _Slope;	
}

var Ichimoku(int PeriodTenkan,int PeriodKijun,int PeriodSenkou,int Offset)
{
	if(!PeriodTenkan) PeriodTenkan = 9;
	if(!PeriodKijun) PeriodKijun = 26;
	if(!PeriodSenkou) PeriodSenkou = 52;
	
	checkLookBack(Max(PeriodSenkou,PeriodKijun)+Offset);
	if(g->nTimeFrame == 1) {
		g->vTenkan = (HH(PeriodTenkan,Offset)+LL(PeriodTenkan,Offset))/2;
		g->vKijun = (HH(PeriodKijun,Offset)+LL(PeriodKijun,Offset))/2;
		g->vSenkouA = (HH(PeriodTenkan,Offset+PeriodKijun)+LL(PeriodTenkan,Offset+PeriodKijun)
			+ HH(PeriodKijun,Offset+PeriodKijun)+LL(PeriodKijun,Offset+PeriodKijun))/4;
		g->vSenkouB = (HH(PeriodSenkou,Offset+PeriodKijun)+LL(PeriodSenkou,Offset+PeriodKijun))/2;
	} else {
		var *High = series(priceHigh(0),0), *Low = series(priceLow(0),0);
		g->vTenkan = (MaxVal(High+Offset,PeriodTenkan)+MinVal(Low+Offset,PeriodTenkan))/2;
		g->vKijun = (MaxVal(High+Offset,PeriodKijun)+MinVal(Low+Offset,PeriodKijun))/2;
		g->vSenkouA = (MaxVal(High+Offset+PeriodKijun,PeriodTenkan)+MinVal(Low+Offset+PeriodKijun,PeriodTenkan)
			+ MaxVal(High+Offset+PeriodKijun,PeriodKijun)+MinVal(Low+Offset+PeriodKijun,PeriodKijun))/4;
		g->vSenkouB = (MaxVal(High+Offset+PeriodKijun,PeriodSenkou)+MinVal(Low+Offset+PeriodKijun,PeriodSenkou))/2;
	}
	return g->vTenkan;
}

var Chikou(int Shift)
{
	checkLookBack(Shift);
	return priceClose(Shift);
}

var ChandelierLong(int TimePeriod,var Factor)
{
	if(!TimePeriod) TimePeriod = 22;
	if(0. == Factor) Factor = 3.;
	return HH(TimePeriod,0) - ATR(TimePeriod)*Factor;
}

var ChandelierShort(int TimePeriod,var Factor)
{
	if(!TimePeriod) TimePeriod = 22;
	if(0. == Factor) Factor = 3.;
	return LL(TimePeriod,0) + ATR(TimePeriod)*Factor;
}


///////////////////////////////////////////////////////////////////
// helper functions 

int at(int hhmm)
{
	if(!g->bars) return 0;
	if(g->vBarPeriod >= 1440.) return 1;
	return hhmm > ltod(g->nBarZone,1) && hhmm <= ltod(g->nBarZone,0);
}

int barssinceN(int Condition,int Occurrences)
{
	if(!g->bars) return -1;
	vars Conditions = series(Condition);
	int i,j = Occurrences;
	for(i=0; i<g->nLookBack; i++) {
		if(Conditions[i] != 0.) j--;
		if(j <= 0) return i;
	}
	return -1;
}

int barssince(int Condition)
{
	return barssinceN(Condition,1);
}

var valuewhen(int Condition,var* Data,int Occurrences)
{
	int Idx = barssinceN(Condition,Occurrences);
	if(Idx >= 0) return Data[Idx];
	return 0.;
}

var cum(var Inc)
{
	vars Ex = series(0,2);
	return Ex[0] = Ex[1]+Inc;
}

int changed(var Val)
{
	vars Ex = series(Val, 2);
	if (Ex[0] > Ex[1]) return 1;
	if (Ex[0] < Ex[1]) return -1;
	return 0;
}

var fix0(var Val)
{
	if(Val != 0 && !_invalid(Val))
		return Val;
	return g->vFix;
}

// return the number of bars in rising/falling series
int NumRiseFall(var* Data,int Period)
{
	checkLookBack(Period);
	int i,j;
	for(i=0; i<Period-1; i++)
		if(Data[i]>=Data[i+1]) break;
	for(j=0; j<Period-1; j++)
		if(Data[j]<=Data[j+1]) break;
	return j-i;
}

// return the number of rising bars
int NumUp(var* Data,int Period,var vThreshold)
{
	checkLookBack(Period);
	int i,j=0;
	for(i=0; i<Period-1; i++)
		if(Data[i] > Data[i+1]+vThreshold) j++;
	return j;
}

// return the number of falling bars
int NumDn(var* Data,int Period,var vThreshold)
{
	checkLookBack(Period);
	int i,j=0;
	for(i=0; i<Period-1; i++)
		if(Data[i] < Data[i+1]-vThreshold) j++;
	return j;
}

// return Number of white and black candles
int NumWhiteBlack(var vBody,int Offset,int Period)
{
	checkLookBack(Period+Offset);
	int i,j;
	for(i=0; i<Period; i++)
		if(priceClose(i+Offset)+vBody > priceOpen(i+Offset)) break;
	for(j=0; j<Period; j++)
		if(priceClose(j+Offset)-vBody < priceOpen(j+Offset)) break;
	return j-i;
}

int NumInRange(var* Low,var* High,var vMin,var vMax,int Period)
{
	int i,n;
	for(i=0,n=0; i<Period; i++)
		if(High[i] <= vMax && Low[i] >= vMin) n++;
	return n;
}

var vUp, vDn;
var ProfitFactor(var* Data,int Length)
{
	vUp = 0.,vDn = 0.;
	for(int i=1; i<Length; i++) {
		if(Data[i-1] > Data[i])
			vUp += Data[i-1]-Data[i];
		else
			vDn += Data[i]-Data[i-1];
	}
	if(vDn == 0.) 
		return vUp == 0.? 1. : 10.;
	return Clamp(vUp/vDn,0.,9.999);
}

var SumUp(var* Data,int Length)
{
	ProfitFactor(Data,Length);
	return vUp;
}

var SumDn(var* Data,int Length)
{
	ProfitFactor(Data,Length);
	return -vDn;
}


// Spearman trend indicator, S&C 2/2011
var Spearman(var* Data,int Period)
{
	Period = Clamp(Period,2,256);
	checkLookBack(Period);
	int* Idx = sortIdx(Data,Period);
	var Sum = 0;
	int i,count;
	for(i=0,count=Period-1; i < Period; i++,count--)
		Sum += (count-Idx[i])*(count-Idx[i]);
	return 1. - 6.*Sum/(Period*(Period*Period-1.));
}

// Fractal Dimension
var FractalDimension(var* Data,int Period)
{
	checkLookBack(2*Period);
	Period &= ~1;
	int Period2 = Max(1,Period/2);
	var N1 = (MaxVal(Data,Period2)-MinVal(Data,Period2))/Period2;
	var N2 = (MaxVal(Data+Period2,Period2)-MinVal(Data+Period2,Period2))/Period2;
	var N3 = (MaxVal(Data,Period)-MinVal(Data,Period))/Period;
	if(N1+N2 <= 0. || N3 <= 0.) return 1.;
	return (log(N1+N2)-log(N3))/log(2.);
}

// Hurst exponent
var Hurst(var* Data,int Period)
{
	Period = Max(20,Period);
	vars Hursts = series(2. - FractalDimension(Data,Period),2);
	var R = Clamp(Smooth(Hursts,Period/10),0.,1.);
	return R;
}

// Average of >0 values
var SMAP(vars Data,int Period)
{
	checkLookBack(Period);
	int i,N = 0;
	var Sum = 0;
	for(i=0; i<Period; i++)
		if(Data[i] > 0) {
			Sum += Data[i];
			N++;
		}
	if(!N) return 0;
	return Sum/N;
}

// statistical moment (1 = mean, 2 = variance, 3 = skewness, 4 = kurtosis)
var Moment(var* Data,int Period,int n)
{
	Period = Max(2,Period);
	checkLookBack(Period);
	g->vMean = 0.;
	for(int i=0; i<Period; i++)
		g->vMean += Data[i];
	g->vMean /= Period;
	if(n <= 1) return g->vMean;
	var variance=0.,sum = 0.;
	int i,j;
	for(i=0; i<Period; i++) {
		var f = Data[i]-g->vMean;
		variance += f*f;
		for(j=2; j<=n; j++)
			f *= Data[i]-g->vMean;
		sum += f;
	}
	variance /= (Period-1);
	if(n==2) return variance;
	sum /= variance*(Period-1);
	var stdev = sqrt(variance);
	for(j=3; j<=n; j++)
		sum /= stdev;
	return sum;
}

// downside moment (1 = mean, 2 = variance, 3 = skewness, 4 = kurtosis)
var SemiMoment(var* Data,int Period,int n)
{
	Period = Max(2,Period);
	checkLookBack(Period);
	g->vMean = 0.;
	for(int i=0; i<Period; i++)
		g->vMean += Data[i];
	g->vMean /= Period;
	if(n <= 1) return g->vMean;
	var variance=0.,sum = 0.;
	int i,j,Dn=0;
	for(i=0; i<Period; i++) {
		var f = Data[i]-g->vMean;
		if(f > 0) continue; // count downside only
		Dn++;
		variance += f*f;
		for(j=2; j<=n; j++)
			f *= Data[i]-g->vMean;
		sum += f;
	}
	variance /= Dn;
	if(n==2) return variance;
	sum /= variance*Dn;
	var stdev = sqrt(variance);
	for(j=3; j<=n; j++)
		sum /= stdev;
	return sum;
}

var Sharpe(var* Data,int Period)
{
	var V = Moment(Data,Period,2);
	if(V <= 0.) return 0.;
	return g->vMean/sqrt(V);
}

var Sortino(var* Data,int Period)
{
	var V = SemiMoment(Data,Period,2);
	if(V <= 0.) return 0.;
	return g->vMean/sqrt(V);
}

var VolatilityMM(var* Data,int TimePeriod,int EMAPeriod)
{
	var Range = MaxVal(Data,TimePeriod)-MinVal(Data,TimePeriod);
	if(!EMAPeriod) return Range;
	var R = EMA0(series(Range,0),EMAPeriod);
	SETSERIES(Data,TimePeriod);
	return R;
}

var VolatilityC(int TimePeriod,int EMAPeriod)
{
	if(!TimePeriod) TimePeriod = 10;
	if(!EMAPeriod) EMAPeriod = 10;
	checkLookBack(TimePeriod+g->nUnstablePeriod);
	vars Range = series(priceHigh(0)-priceLow(0),0);
	var EMA_0 = EMA0(Range,EMAPeriod);
	var EMA_N = EMA0(Range+TimePeriod,EMAPeriod);
	return 100.*(EMA_0-EMA_N)/Max(1.,EMA_N);
}

var Volatility(var* Data,int TimePeriod)
{
	checkLookBack(TimePeriod);
	var Return = Data[1] == 0.? 1. : Data[0]/Data[1];
	vars LogReturn = series(log(Return),0);
	var Variance = Moment(LogReturn,TimePeriod,2);
	return sqrt(Variance*252*DAYBARS);
}

// Gain based on shannon probability
// Algorithm from http://www.johncon.com/john/correspondence/020213233852.26478.html
var ShannonGain(var* Data,int Period)
{
	Period = Max(Period,2);
	checkLookBack(Period+1);
	var sumx = 0., sumx2 = 0.;
	int j;
	for (j=0; j<Period; j++) {
		var G = 0.0000001;
		if(Data[j+1] != 0. && Data[j+1] != Data[j]) 
			G = log(Data[j]/Data[j+1]); // gain
		sumx += G;
		sumx2 += G*G;
	}
	var avgx = sumx/Period; // average gain
	var rmsx = sqrt(sumx2/Period); // root-mean-square of gains
	var P = ((avgx/rmsx)+1)/2.0; // shannon probability
	//return pow(1+rmsx,P)*pow(1-rmsx,1-P); // gain
	return P*log(1+rmsx) + (1-P)*log(1-rmsx); // logarithmic gain
}

var log_2(var X) { return log(X)/0.69315; } //log(2)
 
var ShannonEntropy256(char *S,int Length)
{
	static var Hist[256];
	memset(Hist,0,256*sizeof(var));
	var Step = 1./Length;
	for(int i=0; i<Length; i++) 
		Hist[S[i]] += Step;
	var H = 0;
	for(int i=0;i<256;i++){
		if(Hist[i] > 0.)
			H -= Hist[i]*log_2(Hist[i]);
	}
	return H;
}

var ShannonEntropy(var *Data,int Length,int PatternSize)
{
	int i,j;
	int Size = Length-PatternSize-1;
	if(!Data || Size <= 0) return 0.; 
	char* S = (char*)mem_tempalloc(Size,MEMTEMP_ENT);
	if(!S) return 0.;
	for(i=0; i<Size; i++) {
		int C = 0;
		for(j=0; j<PatternSize; j++) {
			if(Data[i+j] > Data[i+j+1])
				C += 1<<j;
		}
		S[i] = C;
	}
	var H = ShannonEntropy256(S,Size);
	return H;
} 

var CI(int TimePeriod)
{
	checkLookBack(TimePeriod);
	var Sum = 0;
	for(int i=0; i<TimePeriod; i++)
		Sum += priceHigh(i)-priceLow(i);
	Sum /= HH(TimePeriod,0)-LL(TimePeriod,0);
	return 100.*log10(Sum)/log10((double)TimePeriod);
}

var move0(vars Data,int Length,int Horizon,var Percent)
{
	if(Length <= Horizon) return 0;
	checkLookBack(Length);
	var* Moves = series(0,0);
	for(int i=0; i<Length-Horizon; i++)
		Moves[i] = 100.*(Data[i]-Data[i+Horizon])/Data[i+Horizon];
	var Result = Percentile(Moves,Length-Horizon,Percent);
	return Result;
}

var predictMove(vars Data,int Length,int Horizon,var Percent)
{
	if(Length <= Horizon) return 0;
	checkLookBack(Length);
	var* Moves = (var*)mem_tempalloc(Length-Horizon,MEMTEMP_MOVE);
	for(int i=Horizon; i<Length; i++)
		Moves[i-Horizon] = abs(Data[i-Horizon]-Data[i]);
	var Result = Percentile(Moves,Length-Horizon,Percent);
	return Result;
}

inline int seasonMode(int i,int Mode)
{
	switch(Mode) {
		case 1: return hour(i);
		case 2: return dow(i);
		case 3: return tdm(i);
		case 4: return month(i);
	}
	return 0;
}

// price move within a given time horizon
var season0(vars Data,int Length,int Horizon,int Mode)
{
	checkLookBack(Length);
	if(Length <= Horizon) return 0;
	int i = 0, M = seasonMode(0,Mode);
	var Val0 = 0., Val1 = 0.; // initial and final value
	if(Mode == 3) Length -= 31; // tdm goes back to the first day of the month
	while(i < Length) {
		for(; i < Length && seasonMode(i,Mode) == M; i++); // go back to previous day
		for(; i < Length && seasonMode(i,Mode) != M; i++); // go back to same day previous week/month
		for(; i < Length && seasonMode(i,Mode) == M; i++) { // collect data of day (Val1) and day+Horizon (Val0)
			if(i >= Horizon)
				Val1 += Data[i], Val0 += Data[i-Horizon];
		}
	}
	if(Val1 == 0.) return 0.;
	return 100.*(Val0-Val1)/Val1; // percent change from day to day+Horizon
}

var predictSeason(vars Data,int Length,int Horizon,int Mode)
{
	if(Length <= Horizon) return 0;
	var Val = 0.; // initial and final value
	int i, Samples, Count = 0;
	switch(Mode) {
		case 1: Samples = Length/DAYBARS; break;
		case 2: Samples = Length/(6*DAYBARS); break;
		case 3: Samples = Length/(24*DAYBARS); break;
		default: Samples = Length/(255*DAYBARS); break;
	}
//	if(g->nBar < Length) return 0;
	var D = wdate(0);
	for(i=Horizon; i<Length-1; i++) {
		if(D >= wdate(i+1)) { 
			if(Horizon)
				Val += Data[i-Horizon] - Data[i]; // sample the difference only
			else
				Val += Data[i];
			if(++Count >= Samples) 
				break; // prevent different number of samples dependent on date
			switch(Mode) {
				case 1: D -= 1.; break; // same hour previous day
				case 2: D -= 7.; break; // same day previous week
				case 3: { // same day previous month
					int YYYYMMDD = ymd(D); 
					if((YYYYMMDD % 10000) > 100)
						YYYYMMDD -= 100;
					else // January -> back to December
						YYYYMMDD -= 10000 - 1100;
					D = dmy(YYYYMMDD);
					break; }
				default: { // Mode == 4)
					int YYYYMMDD = ymd(D); 
					YYYYMMDD -= 10000;
					D = dmy(YYYYMMDD); // same day previous year
					break; }
			}
		}
	}
	if(!Count) return 0.;
	return Val/Count;
}

// ConnorsRSI
var ConnorsRSI(vars Data,int RSIPeriod,int StreakPeriod,int RankPeriod)
{
	if(!RSIPeriod) RSIPeriod = 3;
	if(!StreakPeriod) StreakPeriod = 2;
	if(!RankPeriod) RankPeriod = 100;

	vars Streaks = series(NumRiseFall(Data,30),0); // consider streaks up to 30
	vars Diffs = series((Data[0]-Data[1])/Max(0.00001,Data[0]),RankPeriod+1);
	return (RSI(Data,RSIPeriod)
		+RSI(Streaks,StreakPeriod)
		+PercentRank(Diffs+1,RankPeriod,Diffs[0]))/3.;
}

// SMA based RSI
var RSIS(vars Data,int Period)
{
	var AvgUp = SumUp(Data,Period+1)/Period, 
		AvgDn = -SumDn(Data,Period+1)/Period;
	if(AvgUp+AvgDn == 0.) return 50;
	else return 100 * AvgUp/(AvgUp+AvgDn);
}


// Mode: most frequent value in a series
var Mode(vars Data,int Period,int Steps)
{
	if(!Data || Steps <= 1) return 0.; 
	var* Rank = (var*)mem_tempalloc(Steps*sizeof(var),MEMTEMP_TEMP);
	memset(Rank,0,Steps*sizeof(var));
	if(!Rank) return 0.;
	MinMax(Data,Period);
	var Step = (g->vMax-g->vMin)/(Steps-1);
	if(Step == 0.) return 0.; // unfilled series
	for(int i=0; i<Period; i++) {
		int N = (int)((Data[i]-g->vMin)/Step);
		Rank[N] += 1;
	}
	int N = MaxIndex(Rank,Steps);
	//g->vTest = N;
	var Result = g->vMin+N*Step;
	return Result;
}

// Larry Williams Market Sentiment Index 
var SentimentLW(int Period)
{
	if(!Period) Period = 9;
	checkLookBack(Period);
	var TrueRangeHigh = max(priceClose(1),priceHigh(0)); 
	var TrueRangeLow = min(priceClose(1),priceLow(0)); 
	return 100. * SMA(series(priceClose(0)-TrueRangeLow),Period)
		/Max(SMA(series(TrueRangeHigh-TrueRangeLow),Period),0.01); 
}

// Normalized Sentiment Index 
var SentimentG(int Period)
{
	checkLookBack(Period);
	return 0.5*normalize(price(0),Period)+50.;
}

// volume weighted average value
var VWAV(vars Data,vars Weights,int Period)
{
	if(!Data || !Period) return 0.; 
	var* V = (var*)mem_tempalloc(Period*sizeof(var),MEMTEMP_VWAV);
   memcpy(V,Weights,Period*sizeof(var));
	renorm(V,Period,1.);
	var Val = 0.;
	for(int i=0; i<Period; i++)
		Val += Data[i]*V[i];
	return Val;
}

// Currency strength /////////////////////////////////////////////

#define MAXCURRENCIES	32
var CStren[MAXCURRENCIES],CStrenSum[MAXCURRENCIES];
int CNum[MAXCURRENCIES];
char CNames[MAXCURRENCIES*4];
int NumCurrencies = 0;

void ccyReset()
{
	for(int n=0; n<MAXCURRENCIES; n++) {
		CStren[n] = 0.;
		CStrenSum[n] = 0.;
		CNum[n] = 0;
	}
}

// assign an index to a currency name
int ccyIdx(char* name)
{
	char Ccy[4];
	memcpy(Ccy,name,4);
	Ccy[3] = 0; 
	for(int n=0; n<MAXCURRENCIES; n++) {
		if(!CNames[n*4]) { // not yet stored
			strcpy(CNames+n*4,Ccy);
			NumCurrencies++;
			return n;
		}
		if(strstr(CNames+n*4,Ccy))
			return n;
	}
	return 0;
}

// add strength to currency, and subtract it from counter currency
void ccySet(var Strength)
{
	if(!isForex(g->asset)) return; 
	int n = ccyIdx(g->asset->sName);
	CStrenSum[n] += Strength; 
	CNum[n]++;
	CStren[n] = CStrenSum[n]/CNum[n];
	g->asset->vStrength = CStren[n];
	
	n = ccyIdx(g->asset->sName+4); // counter currency
	CStrenSum[n] -= Strength; 
	CNum[n]++;
	CStren[n] = CStrenSum[n]/CNum[n];
	g->asset->vStrength -= CStren[n];
}

// return strength or strength difference
var ccyStrength(char* Currency)
{
	if(!Currency || strlen(Currency) < 3) return 0;
	if(Currency[3] == '/') // 
		return CStren[ccyIdx(Currency)] - CStren[ccyIdx(Currency+4)];  
	else return CStren[ccyIdx(Currency)];
}

char* ccyMax()
{
	var MaxStren = 0.;
	static char Ccy[8] = "";
	FOR_ASSETS(a)
		if(a->vStrength > MaxStren) {
			MaxStren = a->vStrength;
			strcpy_s(Ccy,a->sName);
		}
	}
	return Ccy;
}

char* ccyMin()
{
	var MaxStren = 0.;
	static char Ccy[8] = "";
	FOR_ASSETS(a)
		if(a->vStrength < MaxStren) {
			MaxStren = a->vStrength;
			strcpy_s(Ccy,a->sName);
		}
	}
	return Ccy;
}

// Test functions ////////////////////////////////////////////////

// Sine generator for filter testing
var genSine(var Period1, var Period2) 
{
	if(Period1 <= 1.) Period1 = 1.;
	if(Period2 == 0.) Period2 = Period1;
	var k = (Period2-Period1)/(g->numBars-g->nLookBack);
	int Phase = g->nBar-g->nLookBack;
	if(k == 0. || Phase < 0) // constant frequency
		return 0.5 + 0.5*sin((2.*PI*Phase)/Period1);
	else  // hyperbolic chirp
		return 0.5 + 0.5*sin(2*PI*log(1+k*Phase/Period1)/k);
}

// Square wave generator for filter testing
var genSquare(var Period1, var Period2) 
{
	return (genSine(Period1,Period2) < 0.5)? 0. : 1.;
}

// Noise generator for filter testing
var genNoise() 
{
	return random1(1.);
}

// More indicators ///////////////////////////////

// Efficiency Ratio, https://stockcharts.com/school/doku.php?id=chart_school:technical_indicators:kaufman_s_adaptive_moving_average
var ER(vars Data,int Period)
{
	if(!Period) Period = 10;
	checkLookBack(Period);
	var Change = abs(Data[0]-Data[Period]);
	var Path = 0.;
	for(int i=1; i<=Period; i++)
		Path += abs(Data[i-1]-Data[i]);
	if(Path == 0.) return 0.;
	return Change/Path;
}

var KAMA2(vars Data,int ERPeriod,int FastPeriod,int SlowPeriod)
{
	var _ER = ER(Data,ERPeriod);
	var SC = _ER * (smoothF(FastPeriod)-smoothF(SlowPeriod)) + smoothF(SlowPeriod);
	SC *= SC;
	vars _KAMA2 = series(Data[0]);
	return _KAMA2[0] = _KAMA2[1] + SC*(Data[0]-_KAMA2[1]);
}

// Least squares moving average, https://tradingsim.com/blog/least-square-moving-average/
var LSMA(vars Data,int Period,int Offset)
{
	polyfit(0,Data,Period,1,0);
	return polynom(0,Offset);
}

// Quadratic LSMA, https://www.tradingview.com/script/JXdhlosm-Quadratic-Least-Squares-Moving-Average-Smoothing-Forecast/
var QLSMA(vars Data,int Period,int Offset)
{
	polyfit(0,Data,Period,2,0);
	return polynom(0,Offset);
}

/////////////////////////////////////////////

int crossOver2(vars a,vars b,int TimePeriod) 
{ 
	int i;
	if(TimePeriod < 2) 
		return ifelse1(crossOver0(a,b),1,0);
	for(i=0; i<TimePeriod-1; i++)
		if(crossOver0(a+i,b+i)) return i+1;
	return 0;
}
int crossUnder2(vars a,vars b,int TimePeriod) 
{ 
	int i;
	if(TimePeriod < 2) 
		return ifelse1(crossUnder0(a,b),1,0);
	for(i=0; i<TimePeriod-1; i++)
		if(crossUnder0(a+i,b+i)) return i+1;
	return 0;
}
int crossOver3(vars a,var b,int TimePeriod) 
{ 
	int i;
	if(TimePeriod < 2) 
		return ifelse1(crossOver1(a,b),1,0);
	for(i=0; i<TimePeriod-1; i++)
		if(crossOver1(a+i,b)) return i+1;
	return 0;
}
int crossUnder3(vars a,var b,int TimePeriod) 
{ 
	int i;
	if(TimePeriod < 2) 
		return ifelse1(crossUnder1(a,b),1,0);
	for(i=0; i<TimePeriod-1; i++)
		if(crossUnder1(a+i,b)) return i+1;
	return 0;
}

int valley1(vars Data,int TimePeriod)
{
	int i;
	if(TimePeriod < 3) 
		return ifelse1(valley0(Data),1,0);
	for(i=0; i<TimePeriod-2; i++)
		if(valley0(Data+i)) return i+1;
	return 0;
}
int peak1(vars Data,int TimePeriod)
{
	int i;
	if(TimePeriod < 3) 
		return ifelse1(peak0(Data),1,0);
	for(i=0; i<TimePeriod-2; i++)
		if(peak0(Data+i)) return i+1;
	return 0;
}

var slope(int Type,vars Data,int TimePeriod)
{
	g->vMaxIdx = g->vMinIdx = 0.;
	g->vMin = g->vMax = 0.;
	int i;
	if(Type == VALLEY) {
		var Min1 = NIL, Min2 = NIL;
		for(i=0; i<TimePeriod; i++) {
			int Idx = valley1(Data+i,TimePeriod-i);
			if(!Idx) break;
			i += Idx;
			if(Data[i] < Min1 && Data[i] < Min2) { // lowest valley
				g->vMaxIdx = g->vMinIdx; 
				g->vMinIdx = i;
				Min1 = Data[i];
			} else if(Data[i] < Min2) { // second lowest
				g->vMaxIdx = i;
				Min2 = Data[i];
			}
		}
	} else if(Type == PEAK) {
		var Max1 = 0, Max2 = 0;
		for(i=0; i<TimePeriod; i++) {
			int Idx = peak1(Data+i,TimePeriod-i);
			if(!Idx) break;
			i += Idx;
			if(Data[i] > Max1 && Data[i] > Max2) { // highest peak
				g->vMinIdx = g->vMaxIdx; 
				g->vMaxIdx = i;
				Max1 = Data[i];
			} else if(Data[i] > Max2) { // second highest
				g->vMinIdx = i;
				Max2 = Data[i];
			}
		}
	}
	if(g->vMaxIdx == 0. || g->vMinIdx == 0.)
		return 0.; // less than 2 peaks/valleys
	g->vMin = Data[(int)g->vMinIdx];
	g->vMax = Data[(int)g->vMaxIdx];
	return abs(g->vMin-g->vMax)/(g->vMinIdx-g->vMaxIdx);
}

var line(int Offset)
{
	if(g->vMaxIdx == g->vMinIdx)
		return 0.; // no line
	return Offset*(g->vMin-g->vMax)/(g->vMinIdx-g->vMaxIdx);
}

var Support(vars Data,int TimePeriod)
{
	g->vSlope = slope(VALLEY,Data,TimePeriod);
	if(g->vMaxIdx && g->vMinIdx)
		return 0.5*(g->vMin+g->vMax);
	return 0.;
}

var Resistance(vars Data,int TimePeriod)
{
	g->vSlope = slope(PEAK,Data,TimePeriod);
	if(g->vMaxIdx && g->vMinIdx)
		return 0.5*(g->vMin+g->vMax);
	return 0.;
}

int Divergence(vars Highs,vars Lows,vars Data,int TimePeriod)
{
	var SlopeH = slope(PEAK,Highs,TimePeriod);
	var SlopeL = slope(VALLEY,Lows,TimePeriod);
	var SlopeOH = slope(PEAK,Data,TimePeriod);
	var SlopeOL = slope(VALLEY,Data,TimePeriod);
	int Ex = 0;
	if(SlopeOH > 0. && SlopeH <= 0.) {
		g->vSlope = SlopeOH;
		Ex += 4;	// bullish hidden
	}
	if(SlopeOL < 0. && SlopeL >= 0.) {
		g->vSlope = SlopeOL;
		Ex += 8; // bearish hidden
	}
	if(SlopeL < 0. && SlopeOL >= 0.)	{
		g->vSlope = SlopeL;
		Ex += 1; // bullish regular
	}
	if(SlopeH > 0. && SlopeOH <= 0.) {
		g->vSlope = SlopeH;
		Ex += 2;	// bearish regular
	}
	return Ex;
}

///////////////////////////////////////////////

// outside pattern from https://www.investopedia.com/terms/o/outsidereversal.asp
int CDLOutside()
{
	if(priceClose(1) < priceOpen(1) 
	&& priceClose(0) > priceOpen(0)
	&& priceHigh(0) > priceHigh(1)
	&& priceLow(0) < priceLow(1))
		return 100;	// bullish
	if(priceClose(1) > priceOpen(1) 
	&& priceClose(0) < priceOpen(0)
	&& priceHigh(0) > priceHigh(1)
	&& priceLow(0) < priceLow(1))
		return -100;	// bearish
	return 0;
}

// engulfing pattern from https://www.investopedia.com/terms/b/bullishengulfingpattern.asp
int CDLEngulfing0()
{
	if(priceClose(1) < priceOpen(1) 
	&& priceOpen(0) <= priceClose(1)
	&& priceClose(0) >= priceOpen(1))
		return 100;	// bullish
	if(priceClose(1) > priceOpen(1) 
	&& priceOpen(0) >= priceClose(1)
	&& priceClose(0) <= priceOpen(1))
		return -100;	// bearish
	return 0;
}

#endif

// correlation indicators from Ehlers' S&C 2020 articles

FUNCTION_V correlY; // dummy function
var trendFunc(var Phase) { return -Phase; }
var cosFunc(var Phase) { return cos(2*PI*Phase); }
var sinFunc(var Phase) { return -sin(2*PI*Phase); }

// spearman correlation with a function
var correl(vars Data,int Length,void* Func)
{
   correlY = (FUNCTION_V)Func; 
   var Sx = 0, Sy = 0, Sxx = 0, Sxy = 0, Syy = 0;
   int count;
   for(count = 0; count < Length; count++) {
      var X = Data[count];
      var Y = correlY((var)count/Length);
      Sx += X;
      Sy += Y;
      Sxx += X*X;
      Sxy += X*Y;
      Syy += Y*Y;
   }
   if(Length*Sxx-Sx*Sx > 0 && Length*Syy-Sy*Sy > 0)
      return (Length*Sxy-Sx*Sy)/sqrt((Length*Sxx-Sx*Sx)*(Length*Syy-Sy*Sy));
   else return 0;
}

var CTI(vars Data,int Length) { return correl(Data,Length,trendFunc); }
var CCYI(vars Data,int Length) { return correl(Data,Length,cosFunc); }
var CCYIR(vars Data,int Length) { return correl(Data,Length,sinFunc); }

// market state
var CCYIState(vars Data,int Length,var Threshold)
{
   vars Angles = series(0,2);
   var Real = correl(Data,Length,cosFunc);
   var Imag = correl(Data,Length,sinFunc);
//Compute the angle as an arctangent function and resolve ambiguity
   if(Imag != 0) Angles[0] = 90 + 180/PI*atan(Real/Imag);
   if(Imag > 0) Angles[0] -= 180;
//Do not allow the rate change of angle to go negative
   if(Angles[1]-Angles[0] < 270 && Angles[0] < Angles[1])
      Angles[0] = Angles[1];
//Compute market state
   if(abs(Angles[0]-Angles[1]) < Threshold)
		return Angles[0] < 0? -1.: 1.;
   else return 0;
}

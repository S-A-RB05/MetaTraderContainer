// Market Regime test ///////////////////

var ACI(vars Data,int TimePeriod,int Lag)
{
	Lag = clamp(Lag,1,TimePeriod-1);
	var Corr = 0;
	int i;
	for(i=1; i<=Lag; i++) {
		Corr += Correlation(
			Data,Data+i,TimePeriod-Lag);
	}
	//return LowPass(series(Corr/Lag),TimePeriod/2);
	return Corr/Lag;
}

function run()
{
	BarPeriod = 60;
	MaxBars = 1500;
	LookBack = 500;
	asset(""); // dummy asset
	ColorUp = ColorDn = 0; // don't plot a price curve
	PlotHeight1 = 300;
	PlotHeight2 = 200;
	set(PLOTNOW);
	
	if(Init) seed(1000);
	int TrendStart = LookBack + 0.4*(MaxBars-LookBack);
	vars Signals = series(1 + 0.5*genNoise());
	if(Bar > TrendStart) 
		Signals[0] += 0.003*(Bar-TrendStart);
		//0.1*sin(0.015*PI*(Bar-TrendStart));
	int Color = RED;
	int TimePeriod = 1*LookBack/2;
	if(Bar < TrendStart)
		plot("Sidewards",Signals,MAIN,BLACK);
	else
		plot("Trending",Signals,MAIN,BLUE);
	//plot("Hurst",Hurst(Signals,TimePeriod),NEW|LINE,Color);
	plot("MMI",MMI(Signals,TimePeriod),NEW|LINE,Color);
	//plot("FractDim",FractalDimension(Signals,TimePeriod),NEW|LINE,Color);
	//plot("ACI",ACI(Signals,TimePeriod,50),NEW|LINE,Color);
}


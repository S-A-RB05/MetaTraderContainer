// Rules and parameter training in 2 cycles ////////////////////////////

function run()
{
	set(TESTNOW,LOGFILE);
	Verbose = 3;
	Slippage = 0;
	StartDate = 2010;
	EndDate = 2018;
	BarPeriod = 1440; // 1 day
	NumWFOCycles = 5;
	asset("EUR/USD");

	if(Train) Hedge = 2;	// allow simultaneous long + short positions for training 
	LifeTime = 1; 
	MaxLong = MaxShort = 1;
	
	NumTrainCycles = 2;
	if(TrainCycle == 0) set(RULES,PARAMETERS); // Test or Trade mode
	else if(TrainCycle == 1) set(RULES);
	else if(TrainCycle == 2) set(PARAMETERS);
	
	Stop = optimize(15,1,15,1)*ATR(20);
	int GoLong = adviseLong(PATTERN+FAST+2+RETURNS,0,
		priceHigh(2),priceLow(2),priceClose(2),
		priceHigh(1),priceLow(1),priceClose(1),
		priceHigh(1),priceLow(1),priceClose(1),
		priceHigh(0),priceLow(0),priceClose(0));
	int GoShort = adviseShort();
	if(GoLong > 50)
		enterLong();
	if(GoShort > 50)
		enterShort();
}

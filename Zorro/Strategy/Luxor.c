// Luxor system by Jaeckle/Tomasini /////////////
// Keep a 'indicator soup' system profitable 
// by creative selection of start and end date 

function run()
{
	set(PLOTNOW);
	StartDate = 2004;
	EndDate = 2010;
	BarPeriod = 30;
	asset("GBP/USD");

// no trade costs...	
//	Commission = Spread = Slippage = RollLong = RollShort = 0; 
	
	vars Fast = series(SMA(seriesC(),3)),
		Slow = series(SMA(seriesC(),30));
	
	static var BuyLimit,SellLimit,BuyStop,SellStop;
	
	if(crossOver(Fast,Slow)) {
		BuyStop = priceH() + 1*PIP;
		BuyLimit = priceH() + 5*PIP;
	}
	if(crossUnder(Fast,Slow)) {
		SellStop = priceL() - 1*PIP;
		SellLimit = priceL() - 5*PIP;
	}
		
	if(!NumOpenLong && Fast[0] > Slow[0] && priceC() < BuyLimit)
		enterLong(1,BuyStop);
	if(!NumOpenShort && Fast[0] < Slow[0] && priceC() > SellLimit)
		enterShort(1,SellStop);
}
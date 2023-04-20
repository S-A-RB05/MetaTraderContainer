// Account status script ///////////////////////

#define NUM_BROKERS	3
#define BROKERS "FXCM-Real","Binance","IB-Gate"	// from account list
#define KEEP	// keep last status when disconnected
var Convert[NUM_BROKERS] = { 1.,30000.,1.2 };	// conversion factors 
//#define CONVERT 1 // get Convert[1] from Binance

int redgreen(var Val)
{
	if(Val > 0.) return GREEN;
	else if(Val < 0.) return RED;
	else return WHITE;
}

function run() 
{
	if(!require(-2.35)) return quit("");
	if(!Live) return quit("Start in [Trade] mode!");
	set(LOGFILE);
	BarPeriod = 1;
	LookBack = 0;
	TickTime = BarPeriod*60*1000;
	BarMode = BR_FLAT;
	int DayBars = 1440/BarPeriod;
	int Color = redgreen(0);
	ignore(53);
	ignore(72);

	if(Init) {
		panel(NUM_BROKERS+2,7,BLACK,75);
		panelSet(0,0,"Account",Color,1,1);
		int i=1;
		panelSet(0,i++,"Total Profit",Color,4,1);
		panelSet(0,i++,"Last Profit",Color,4,1);
		panelSet(0,i++,"Liquidity",Color,4,1);
		panelSet(0,i++,"Balance",Color,4,1);
		panelSet(0,i++,"OpenVal",Color,4,1);
		panelSet(0,i++,"Margin",Color,4,1);
	}

#ifdef CONVERT
	assetAdd("BTC/EUR","Binance:BTC/EUR");
	asset("BTC/EUR");
	var Price = priceClose();
	if(Price > 0.)
		Convert[CONVERT] = Price;
	printf("\nBTC/EUR: %.2f",Price);
#else
	asset(""); // dummy asset
#endif

	var Profit,Val,Total = 0,TotalDay = 0,TotalVal = 0;
	var Bal,Opn,Mgn;
	while(loop(BROKERS)) {
		brokerAccount(Loop1,0,&Bal,&Opn,&Mgn);
		printf("\n%s %s Open %s Margin %s",Loop1,
			sftoa(Bal,6),sftoa(Opn,6),sftoa(Mgn,6));
		if(Mgn == 0.) Mgn = Opn; // unleveraged account
		if(Bal+Opn == 0.) { // disconnected or empty
			set(NOSHIFT);
			panelSet(Itor1+1,0,Loop1,RED,1,1); 
		} else {
			set(NOSHIFT|OFF);
			panelSet(Itor1+1,0,Loop1,GREEN,1,1);
		}
#ifdef KEEP
		vars Lasts = series(0,-3);
		if(Bal == 0) Bal = Lasts[0]; else Lasts[0] = Bal;
		if(Opn == 0) Opn = Lasts[1]; else Lasts[1] = Opn;
		if(Mgn == 0) Mgn = Lasts[2]; else Lasts[2] = Mgn;
#endif
		TotalVal += Val = (Bal+Opn) * Convert[Itor1];
		vars InitVals = series(0,-1);
		if(InitVals[0] == 0.) InitVals[0] = Val;
		vars DayVals = series(Val,DayBars);
		Total += Profit = ifelse(Val!=0.,Val-InitVals[0],0.);
		int i=1;
		panelSet(Itor1+1,i++,sftoa(Profit,-2),redgreen(Profit),4,1);
		TotalDay += Profit = ifelse(Val!=0.,Val-DayVals[DayBars-1],0);
		panelSet(Itor1+1,i++,sftoa(Profit,-2),redgreen(Profit),4,1);
		panelSet(Itor1+1,i++,sftoa(Val,6),Color,4,1);
		panelSet(Itor1+1,i++,sftoa(Bal,6),Color,4,1);
		panelSet(Itor1+1,i++,sftoa(Opn,6),Color,4,1);
		panelSet(Itor1+1,i++,sftoa(Mgn,6),Color,4,1);
	}
	
	if(Init)
		panelSet(NUM_BROKERS+1,0,"Total",Color,1,1);
	int i=1;
	panelSet(NUM_BROKERS+1,i++,sftoa(Total,-2),redgreen(Total),4,1);
	panelSet(NUM_BROKERS+1,i++,sftoa(TotalDay,-2),redgreen(TotalDay),4,1);
	panelSet(NUM_BROKERS+1,i++,sftoa(TotalVal,6),Color,4,1);
}
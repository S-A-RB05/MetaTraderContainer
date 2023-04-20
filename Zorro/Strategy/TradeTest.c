// Trade Test for Broker API Verification ///////////////////

// Optional parameters for special tests ////////////////////
#define PERCENTSTOP	// stop in percent of price
//#define BY_MARGIN		// Margin instead of lots
#define MAXLOTS	200		// slider endpoint
//#define MAXLIMIT	0.5	// OrderLimit in multiples of spread
#define BARPERIOD	1
//#define BAROFFSET	10
#define USE_LOOKBACK	1000	// comment out for no lookback
#define VERBOSE	2	//+DIAG
//#define ERRORMODE	1		// simulate close errors
//#define DAYOFFSET	(1+6./24)
//#define LOG_ACCOUNT
//#define LOG_TRADES
#define LOG_VOL		// log volume, spread, margin cost
#define ORDERSTATUS	// print trade status at any tick
//#define LOG_ALL		// log all asset parameters
//#define LOG_BOOK	// read the order book
//#define RESUME			// resume open trades in next session
//#define TYPE_TRADES	// last trade price instead of current ask/bid
//#define NOWEEKEND
//#define DIAGNOSTICS	2
//#define ORDERTEXT	"MOC/" // special order types for IB
//#define EXCHANGE	"BTRX"
//#define RECORD			// store asset prices
//#define ROUND_LIMIT	// round order limit to 1 pip (needed for IB)
//#define ASSETLIST	"AssetsDarwinex2"
//#define ASSET "WTI-FUT-WTIV0-WTI-IPE"
//#define ASSET "ES-FUT-20200320-ES-GLOBEX"
//#define ASSET "VIX3M-IND-CBOE-USD"
//#define ASSET "AAPL"
//#define MAXREQUESTS	10./60	// 10 requests per minute
//#define BROKERSTOP		// send stop order to broker
//#define BROKERPOS		// read positions from account

///////////////////////////////////////////////////////////
int AutoTrade = 0, OrderMode = 0;
var Limit = 0;

int tradeAdapt(var Step)
{
	if(!TradeIsOpen) { // entry limit
		if(TradeIsLong) {
			if(OrderLimit > TradePriceOpen) 
				return 0; 	// try no more
			OrderLimit += Step;	// adapt limit
		} else { // short
			if(OrderLimit < TradePriceOpen-Spread) 
				return 0; 	
			OrderLimit -= Step;
		}
	} else { // exit limit
		if(TradeIsLong)
			OrderLimit -= Step;
		else
			OrderLimit += Step;
	}
	OrderLimit = roundto(OrderLimit,PIP/2);
	return 1;
}

// TMF with adaptive entry/exit algorithm
int tmf()
{
#ifdef ORDERSTATUS
	printf("\n%i: Lots %i Target %i Open %.5f Close %.5f",
		TradeID, TradeLots, TradeLotsTarget,(var)TradePriceOpen,(var)TradePriceClose);
#endif
//	printf("\nTMF: %f/%f\\%f/%f",priceOpen(),priceHigh(),priceLow(),priceClose());
// adapt limit of FOK/IOC trades
	if(TradeIsMissed && OrderMode >= 2) 
	{
		var Step = max(0.333*Spread,0.333*PIP);
		if(OrderMode == 2) // FOK order
			OrderDelay = 30;	// try again in 30 seconds
		else // GTC order, no delay needed
			OrderDelay = 0;
		if(!tradeAdapt(Step)) 
			return 1; // cancel trade
		else {
			printf("\n%s Limit %s",strtr(ThisTrade),sftoa(OrderLimit,5));
			return ifelse(TradeIsOpen,1+16,2+16); // repeat order, and trigger tmf at next event
		} 
	}
	return 0;
}


//void tock() { printf("\nBar %u",Bar); }

void setLimit(var Factor)
{
	var Factor = Limit/1000;
	OrderLimit = OrderDelay = OrderDuration = 0;
	if(OrderMode == 1 || OrderMode == 3) {
		OrderLimit = priceClose() + Factor*Spread; 
		printf(" Limit %s",sftoa(OrderLimit,5));
	} else if(OrderMode == 2) {
		OrderLimit = priceClose() + ifelse(Factor<0,-Spread,0.);
		printf(" Adaptive %s",sftoa(OrderLimit,5));
	}
#ifdef ROUND_LIMIT
	OrderLimit = roundto(OrderLimit,PIP);
	if(OrderLimit > 0) printf("\nRounded Limit: %.4f",OrderLimit);
#endif
}

void doTrade(int What)
{
#ifdef BY_MARGIN
	Margin = slider(1);
#else
	Lots = slider(1);	// get current slider position
#endif
#ifdef PERCENTSTOP
	if(slider(2) > 0)
		Stop = Trail = roundto(0.01*priceClose()*slider(2),PIP);
	else
		Stop = 0;
#else
	Stop = Trail = PIP*slider(2);
#endif
#ifdef ORDERTEXT
	brokerCommand(SET_ORDERTEXT,ORDERTEXT);
#endif
#ifdef MAXLIMIT
	if(What == 1 || What == 4)
		setLimit(-MAXLIMIT);
	else
		setLimit(MAXLIMIT);
#endif
	if(OrderMode == 3) {
		setf(TradeMode,TR_GTC);
		printf(" GTC");
	} else
		resf(TradeMode,TR_GTC);

	if(What == 1)
		enterLong(tmf);
	else if(What == 2)
		enterShort(tmf);
	else if(What == 3) {
#ifdef BY_MARGIN
		exitLong();
#else
		exitLong(0,0,Lots);
#endif
	} else if(What == 4) {
#ifdef BY_MARGIN
		exitShort();
#else
		exitShort(0,0,Lots);
#endif
	}
}

void click(int row,int col)
{
	if(!is(RUNNING)) return; // only clickable when session is active
	sound("click.wav");	
	
	if(row == -3) { // Asset Box
		panelSet(4,0,AssetBox);
		asset(AssetBox);
		return;
	}
	
	string Text = panelGet(row,col);
	if(Text == "Auto Off") {
		panelSet(row,col,"Auto On",0,0,0); 
		AutoTrade = 1;
	}
	else if(Text == "Auto On") {
		panelSet(row,col,"Auto Off",0,0,0); 
		AutoTrade = 0;
	}
	else if(Text == "NFA Off") {
		panelSet(row,col,"NFA On",0,0,0); 
		set(NFA);
	}
	else if(Text == "NFA On") {
		panelSet(row,col,"NFA Off",0,0,0); 
		set(NFA|OFF);
	}
	else if(Text == "Hedge 0") {
		panelSet(row,col,"Hedge 2",0,0,0); 
		Hedge = 2;
	}
	else if(Text == "Hedge 2") {
		panelSet(row,col,"Hedge 4",0,0,0); 
		Hedge = 4;
	}
	else if(Text == "Hedge 4") {
		panelSet(row,col,"Hedge 5",0,0,0); 
		Hedge = 5;
	}
	else if(Text == "Hedge 5") {
		panelSet(row,col,"Hedge 0",0,0,0); 
		Hedge = 1;
	}
	else if(Text == "MKT Order") {
		panelSet(row,col,"LMT Order",0,0,0); 
		OrderMode = 1;
	}
	else if(Text == "LMT Order") {
		panelSet(row,col,"Adaptive",0,0,0); 
		OrderMode = 2;
	}
	else if(Text == "Adaptive") {
		panelSet(row,col,"GTC Order",0,0,0); 
		OrderMode = 3;
	}
	else if(Text == "GTC Order") {
		panelSet(row,col,"MKT Order",0,0,0); 
		OrderMode = 0;
	} else if(row == 4) { // new asset name
		char Name[NAMESIZE2];
		strcpy(Name,strmid(Text,0,NAMESIZE2-1));
		assetAdd(Name,Text); 
		asset(Name);
	} else {
		printf("\n%s %s",Text,Asset);
		if(Text == "Buy Long")
			call(1,doTrade,1,0);
		else if(Text == "Buy Short")
			call(1,doTrade,2,0);
		else if(Text == "Close Long")
			call(1,doTrade,3,0);
		else if(Text == "Close Short")
			call(1,doTrade,4,0);
		else if(Text == "Update Stop") {
			exitShort(0,priceClose()+Stop);	
			exitLong(0,priceClose()-Stop);	
		}
		OrderLimit = 0;
	}
}

function run()
{
	if(is(TESTMODE)) {
		quit("Click [Trade]!");
		return;
	}
	if(Init)
		AutoTrade = OrderMode = 0;
#ifdef ERRORMODE
	g->nErrorMode |= ERRORMODE;
#endif
	BarPeriod = BARPERIOD;
#ifdef BAROFFSET
	BarOffset = BAROFFSET;
#endif
	PlotPeriod = 5;
	NumYears = 1;
	TradesPerBar = 1;
#ifdef USE_LOOKBACK
	LookBack = USE_LOOKBACK;
#else
	LookBack = 0;
#endif
#ifdef VERBOSE
	Verbose = VERBOSE;
#endif
#ifndef RESUME
	SaveMode = 0;
#endif
#ifdef NOWEEKEND
	resf(BarMode,BR_WEEKEND);
	setf(BarMode,BR_FLAT);
#endif
#ifdef DAYOFFSET
	DayOffset = DAYOFFSET;
	BarZone = EST;
	StartMarket = 930;
	EndMarket = 1715;
	setf(BarMode,BR_MARKET|BR_FLAT);
	printf("\nLocal time %s",strdate(HMS,ldate(BarZone,0)));
#else
	BarMode = BR_FLAT; // assume market always open
#endif
	TickTime = 1000;
	set(LOGFILE);
	
#ifdef ASSETLIST
	assetList(ASSETLIST);
#endif
#ifdef EXCHANGE
	if(Init) brokerCommand(SET_BROKER,EXCHANGE);
#endif
#ifdef ASSET
	assetAdd(ASSET);
	asset(ASSET);
#else
	asset(Asset);
#endif
#ifdef BROKERSTOP
	if(Init) {
		brokerCommand(SET_ORDERTYPE,8); // activate stop orders
		StopFactor = 1; // let broker observe the stops
	}
#endif
#ifdef BROKERPOS
	if(Init) {
		SaveMode = 0;
		int N = brokerTrades(0);
		printf("\n%i account positions read",N);
	}
#endif


#ifdef BY_MARGIN
	Margin = slider(1,100,0,100*MAXLOTS,"Margin",0);
#else
	Lots = slider(1,MAXLOTS/4,0,MAXLOTS,"Lots",0);
#endif
#ifdef PERCENTSTOP	
	Stop = Trail = 0.01*priceClose()*slider(2,0,0,40,"Stop %",0);
#else
	Stop = Trail = PIP*slider(2,0,0,50,"Stop",0);
#endif
	Limit = slider(3,0,0,1000,".Limit","Order limit in spread units");

	if(is(INITRUN)) brokerCommand(SET_ORDERTEXT,"TradeTest");
#ifdef DIAGNOSTICS
	if(is(INITRUN)) brokerCommand(SET_DIAGNOSTICS,DIAGNOSTICS);
#endif
#ifdef TYPE_TRADES
	if(is(INITRUN)) brokerCommand(SET_PRICETYPE,2);
	if(is(INITRUN)) brokerCommand(SET_VOLTYPE,4);
#endif

	if(is(INITRUN)) {
		AutoTrade = 0;
		Hedge = 2;
		panel(9,1,GREY,80);
		int n = 0;
		panelSet(n++,0,"Auto Off",ColorPanel[2],1,4);
		if(mode(NFA))
			panelSet(n++,0,"NFA On",ColorPanel[2],1,4);
		else
			panelSet(n++,0,"NFA Off",ColorPanel[2],1,4);
		panelSet(n++,0,"Hedge 2",ColorPanel[2],1,4);
		panelSet(n++,0,"MKT Order",ColorPanel[2],1,4);
		panelSet(n++,0,Asset,ColorPanel[2],1,2);
		panelSet(n++,0,"Buy Long",ColorPanel[3],1,4);
		panelSet(n++,0,"Buy Short",ColorPanel[3],1,4);
		panelSet(n++,0,"Close Long",ColorPanel[3],1,4);
		panelSet(n++,0,"Close Short",ColorPanel[3],1,4);
		panelSet(n++,0,"Update Stop",ColorPanel[3],1,4);

#ifdef MAXREQUESTS
		MaxRequests = MAXREQUESTS;
#endif
		if(MaxRequests > 0)
			printf("\nMax Requests: %.1f / sec",MaxRequests);
	}
	
	if(AutoTrade) {	
		asset(panelGet(4,0));
		if(NumOpenTotal >= 3) {
			exitLong();
			exitShort();
		} else {
			if(price(0) > price(1)) 
				enterLong();
			else if(price(0) < price(1)) 
				enterShort();
		}
	}
	if(!is(LOOKBACK)) {
#ifdef LOG_ACCOUNT
_POS(10);
		printf("\nN %i H %i  B %s Eq %s M %s  Px %s",
			ifelse(is(NFA),1,0),Hedge,
			sftoa(Balance,2),sftoa(Equity,2),sftoa(MarginVal,2),
			sftoa(priceClose(0),5));
_POS(20);
		var Pos = brokerCommand(GET_POSITION,SymbolTrade);
		if(Pos != 0)
			printf(" T %.2f",Pos);
#endif
#ifdef LOG_TRADES		
		for(open_trades) {
			if(TradeIsPending)
				printf("\n%s still pending",strtr(ThisTrade));
			else
				printf("\n%s Lots: %i Target: %i",strtr(ThisTrade),TradeLots,TradeLotsTarget);
		}
#endif
		int x=1;
	}
#ifdef LOG_VOL
	if(!is(LOOKBACK) && is(SPONSORED))
		printf("\nVol %s  Spr %s  MCost %s",sftoa(marketVol(),2),sftoa(Spread,2),sftoa(MarginCost,2));
#endif
#ifdef LOG_ALL
	if(once(!is(LOOKBACK))) {
		printf("\n%s: PIP %s PipCost %s Mult %.2f",Asset,sftoa(PIP,2),sftoa(PIPCost,2),LotAmount);
		printf("\n%s: Levg %.0f MCost %s",Asset,Leverage,sftoa(MarginCost,2));
		printf("\n%s: Roll+ %f Roll- %f",Asset,RollLong,RollShort);
	}
#endif
#ifdef LOG_BOOK
	static T2 Quotes[MAX_QUOTES];
	brokerCommand(SET_SYMBOL,Symbol);
	int N = brokerCommand(GET_BOOK,Quotes);
	printf("\nOrderbook: %i quotes",N);
#endif
#ifdef RECORD
	History = "rec.t6";
	priceRecord();
#endif
}
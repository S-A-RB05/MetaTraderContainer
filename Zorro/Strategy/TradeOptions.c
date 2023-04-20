// Trade Options or Combos ///////////////////

#include <contract.c>

#define LIST	"AssetsIB"
#define TICKER	"SPY" // select specific list with symbol
#define WAIT_TIME	60	// seconds
#define PRINT_GREEKS
//#define RESUME			// resume trades


int Chain = 0;
int Type = 1; // 1 Call, 2 Put, 3 Strangle, 4 Butterfly, 5 Condor 
var Strike;
int Expiry;

void tradeComboLimit(int Sign,int Attempts)
{
	var Limit = 50, LimitStep = 50/max(1,Attempts); // 50% ask-bid spread
	int i,Attempts; 
	for(; Attempts >= 0; Attempts--,Limit -= LimitStep)
	{
		if(Attempts) {
			printf("#\nPrices:");
			for(i=1; i<=comboLegs(); i++) {
				contractPrice(comboContract(i));
				printf("# %.2f/%.2f",(var)(comboContract(i)->fAsk),(var)(comboContract(i)->fBid));
			}
			var AskBidSpread = abs(comboPremium(Sign)+comboPremium(-Sign));
			OrderLimit = comboPremium(Sign) - Sign*0.01*Limit*AskBidSpread;
			OrderLimit = round(abs(OrderLimit),0.01);
			printf("#\nPremium %.2f Spread %.2f Limit %.2f",
				comboPremium(Sign),AskBidSpread,OrderLimit);
			resf(TradeMode,TR_GTC);	
		} else {
			OrderLimit = 0.;
			printf("#\nPremium %.2f at market",comboPremium(Sign));
			setf(TradeMode,TR_GTC);	// last attempt: open at market
		}
		for(i=1; i<=comboLegs(); i++) 
			ThisTrade = enterLong(Sign*Lots*comboLeg(i));
		if(ThisTrade) if(TradeIsOpen) 
			break;
		if(OrderLimit == 0.)
			break;
	}
}

void trade(int Sign)
{
	Strike = slider(1);
	Expiry = slider(2);
	//var Limit = slider(3);
	var CenterPrice = priceClose();
	int Legs = 0;
	if(Type == 1)
		Legs = combo(
			contract(CALL,Expiry,CenterPrice+Strike),1,
			0,0,0,0,0,0);
	else if(Type == 2)
		Legs = combo(
			contract(PUT,Expiry,CenterPrice+Strike),1,
			0,0,0,0,0,0);
	else if(Type == 3)
		Legs = combo(
			contract(CALL,Expiry,CenterPrice-Strike),1, 
			contract(PUT,Expiry,CenterPrice+Strike),1,
			0,0,0,0);
	else if(Type == 4) // butterfly
		Legs = combo(
			contract(PUT,Expiry,CenterPrice-abs(Strike)),-1, 
			contract(PUT,Expiry,CenterPrice+abs(Strike)),-1,
			contract(PUT,Expiry,CenterPrice),2, 
			0,0);
	else if(Type == 5)
		Legs = combo(
			contract(CALL,Expiry,CenterPrice+abs(Strike)),1, 
			contract(PUT,Expiry,CenterPrice-abs(Strike)),1,
			contract(CALL,Expiry,(CenterPrice+2*abs(Strike))),-1, 
			contract(PUT,Expiry,(CenterPrice-2*abs(Strike))),-1);
	if(!Legs) return;
	printf("\nSend order...");
#ifdef ORDERTYPE
	brokerCommand(SET_ORDERTEXT,ORDERTYPE);
#endif
	brokerCommand(SET_DIAGNOSTICS,1);
	tradeComboLimit(Sign,0);
}


void click(int row,int col)
{
	if(!is(RUNNING)) return; // only clickable when session is active
	sound("click.wav");	
	string Text = panelGet(row,col);
	
	if(Text == "Call") {
		panelSet(row,col,"Put",0,0,0); 
		Type = 2;
	} else if(Text == "Put") {
		panelSet(row,col,"Strangle",0,0,0); 
		Type = 3;
	} else if(Text == "Strangle") {
		panelSet(row,col,"Butterfly",0,0,0); 
		Type = 4;
	} else if(Text == "Butterfly") {
		panelSet(row,col,"Condor",0,0,0); 
		Type = 5;
	} else if(Text == "Condor") {
		panelSet(row,col,"Call",0,0,0); 
		Type = 1;
	}
	else if(Chain) {
		if(Text == "Buy")
			call(1,trade,1,0);
		else if(Text == "Sell")
			call(1,trade,-1,0);
	}
}

void run()
{
	if(!Live) { quit("Click [Trade]!"); return; }
#ifdef LIST
	assetList(LIST,TICKER);	
#endif
	BarPeriod = 1;
	NumYears = 1;
	LookBack = 0;
	Verbose = 3;
	set(LOGFILE);
#ifndef RESUME
	SaveMode = 0;
#endif
	Lots = 1;
	Strike = slider(1,5,-30,30,"Strike","Strike offset in $");
	Expiry = slider(2,30,1,100,"Expiry","Expiration in days");
	//slider(3,0,0,200,"Limit","Order limit in ask-bid percent");

	static int DoUpdateTime;

	if(Init) {
		panel(4,1,GREY,80);
		int row = 0;
		panelSet(row++,0,Asset,ColorPanel[0],1,1);
		panelSet(row++,0,"Call",ColorPanel[3],1,4);
		panelSet(row++,0,"Buy",ColorPanel[3],3,4);
		panelSet(row++,0,"Sell",ColorPanel[3],3,4);
	}

	if(Init)	{
		printf("\nNY Time: %02d:%02d",lhour(ET,NOW),minute(NOW));
		DoUpdateTime = 60;
		printf("\nRetrieving %s data.. ",Asset);
	}
#ifdef TICKER
	asset(TICKER);
#else
	asset(Asset);
#endif
	Multiplier = 100;
	if(Init) printf("ok");
	
	if(DoUpdateTime++ >= 60) { // every 60 bars
		DoUpdateTime = 0;
		printf("\nDownloading %s chain.. ",Asset);
		Chain = contractUpdate(Asset,0,PUT|CALL); 
		printf("\n%d contracts",Chain);
		if(!Chain) { quit("No Options Chain"); return; }
		panelSet(2,0,0,0,1,0);
		panelSet(3,0,0,0,1,0);
	}	
	
#ifdef PRINT_GREEKS
	if(contract(CALL,Expiry,priceClose()+Strike)) {
		//brokerCommand(SET_PRICETYPE,8); // fast mode, no greeks
		contractPrice(ThisContract);
		printf("\nCall %i %.2f: Ask %.2f Bid %.2f",
			ContractExpiry,ContractStrike,ContractAsk,ContractBid);
		var Greeks[5] = { 0,0,0,0,0 };
		brokerCommand(GET_GREEKS,Greeks);
		printf("\nIV %.2f Delta %.2f Gamma %.2f Vega %.2f Theta %.2f",
			Greeks[0],Greeks[1],Greeks[2],Greeks[3],Greeks[4]);
	}
#endif	

	for(open_trades) {
		printf("\n%s Strike %2f Unl %.2f Prem %.0f",
			ifelse(TradeIsCall,"Call","Put"),
			TradeStrike,TradeUnderlying,TradePriceOpen);
		//contractCheck(ThisTrade,3);
	}	
}
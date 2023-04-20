// Options combination, payoff diagram //////////////////////////
#include <contract.c>

//#define ASSET		"QQQ"
#define UNDERL		325	// if no asset
#define HISTVOL	0.15	// if no asset
#define DIVIDEND	0.01
#define RISKFREE	0.01
#define EXPIRY		6*7	// in days
#define RANGE		12		// underlying range, percent
//#define PLOT_HALFTIME	// plot value prior to expiration 
#define PLOT_DELTA	// plot average delta as green line

/////////////////////////////////////////////////
void contractAdd(int Num,int Type,var StrikeDist);
void contractAdd(int Num,int Type,var StrikeDist,int Days);

void Call() {
	contractAdd(1,CALL,0);
}

void Put() {
	contractAdd(1,PUT,0);
}

void Butterfly() {
	contractAdd(1,PUT,-5);
	contractAdd(-2,PUT,0);
	contractAdd(1,PUT,5);
}

void ButterflyC() {
	contractAdd(1,CALL,-5);
	contractAdd(-2,CALL,0);
	contractAdd(1,CALL,5);
}

void CallSpread() {
	contractAdd(1,CALL,-5);
	contractAdd(-1,CALL,5);
}

void PutSpread() {
	contractAdd(1,PUT,5);
	contractAdd(-1,PUT,-5);
}

void Strangle() {
	contractAdd(1,CALL,5);
	contractAdd(1,PUT,-5);
}

void Condor() {
	contractAdd(1,PUT,-15);
	contractAdd(-1,PUT,-5);
	contractAdd(-1,CALL,5);
	contractAdd(1,CALL,15);
}

void doCombo() {
	Condor(); // <= put here the combo to test
	//contractAdd(-2,CALL,2,5);
	//contractAdd(3,CALL,2,10);
	//contractAdd(3,PUT,-2,10);
	//contractAdd(-2,PUT,-2,5);
}

//////////////////////////////////////////////////
var UnderL,HistVol,StrikeFactor;
int ExpiryMax,StrikeMax,Days;


void contractAdd(int Num,int Type,var StrikeDist,int Expiry)
{
	static CONTRACT C;
	C.Type = Type;
	C.Expiry = ifelse(Expiry,Expiry,EXPIRY);
	C.fStrike = round(UnderL+StrikeDist*StrikeFactor,0.5);
	C.fUnl = UnderL;
	C.fBid = C.fAsk = contractVal(&C,C.fUnl,HistVol,DIVIDEND,RISKFREE);
	if(Days)
		printf("\n%s %d %s %.1f Exp %i at %.2f",
			ifelse(Num>0,"Buy","Sell"),abs(Num),
			ifelse(C.Type&CALL,"Call","Put"),(var)C.fStrike,C.Expiry,(var)C.fAsk);
	ExpiryMax = max(ExpiryMax,C.Expiry);
	StrikeMax = max(StrikeMax,abs(StrikeDist));
	comboAdd(&C,Num);
}

void contractAdd(int Num,int Type,var StrikeDist)
{ contractAdd(Num,Type,StrikeDist,0); }


void main() 
{
	HistVol = HISTVOL;
	UnderL = UNDERL;
#ifdef PLOT_HALFTIME
	if(!initRQL())
		return quit("R and RQuantLib required!");
#endif
	Multiplier = 100;
	Days = 0;
	ExpiryMax = StrikeMax = 0;
	StrikeFactor = 1;
	comboAdd(0,0);
	doCombo();
	var Min = (1-RANGE/100.)*UnderL, Max = (1+RANGE/100.)*UnderL; 
	
	slider(1,StrikeMax,max(1,StrikeMax/3),StrikeMax*3,"Strike","");
	slider(2,ExpiryMax,1,ExpiryMax,"Days","");

	while(wait(100)) {
		if(is(SLIDERS)) {
			StrikeFactor = slider(1)/StrikeMax;
			Days = slider(2);
			comboAdd(0,0);
			doCombo();
			plotCombo(HistVol,Min,Max,0,1);
#ifdef PLOT_HALFTIME
			if(Days < ExpiryMax)
				plotCombo(HistVol,Min,Max,Days,2);
#endif
#ifdef PLOT_DELTA
			plotCombo(HistVol,Min,Max,0,3);
#endif
			printf("\nUnderlying: %.2f Volatility: %.2f",UnderL,HistVol);
			printf("\nRisk %.2f Prem %.2f Exp %i days",
				Multiplier*comboRisk(1),Multiplier*comboPremium(1),Days);
			plotChart(0);
			printf("\n");
			set(SLIDERS|OFF);
		}
	}
	quit("Ok!");
}	
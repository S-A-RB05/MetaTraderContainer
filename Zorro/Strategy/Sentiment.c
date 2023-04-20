// Get today's or historical market sentiment //////////////////////////////////////

#define DATE  20110501	// some historical date
#define HORIZON 45	// prediction horizon in days

void main() 
{
#ifndef DATE	
	if(!Live || Broker != "IB") {
		printf("\nStart in Trade mode with IB TWS!");
		return;
	}
	StartDate = NOW;
#else
	StartDate = DATE;
#endif
	LookBack = 0;
	History = ".t8";
	assetList("AssetsIB");
	asset("SPY");

// load today's contract chain
	contractUpdate(0,0,CALL|PUT);
	printf("\n%i contracts today",NumContracts);
	if(!NumContracts) return;
	
// get min and max strike
	contract(CALL,HORIZON,0);
	var StrikeMin = ContractStrike;
	contract(CALL,HORIZON,9999);
	var StrikeMax = ContractStrike;
	printf("\nStrike range %.2f...%.2f",StrikeMin,StrikeMax);

// get underlying price
	var Price,Current = priceClose(0);
	printf("\nCurrent price %.2f",Current);

// plot CPD histogram	
	if(Live) printf("\nWait time approx %i minutes",1+NumContracts/200);
	brokerCommand(SET_PRICETYPE,8); // speed up price loading
	contractCPD(HORIZON);
	var Expected = cpdv(50);
	printf("\nExpected price %.2f",Expected);
	int N = 0;
	for(Price = 0.75*Current; Price <= 1.55*Current; N++,Price += 0.01*Current) {
		if(between(Price,0.99*Current,Current))
			plotBar("Current",N,floor(Price),cpd(Price),BARS|LBL2,BLACK);
		else if(between(Price,Expected-0.01*Current,Expected))
			plotBar("Expected",N,floor(Price),cpd(Price),BARS|LBL2,BLUE);
		else
			plotBar("CPD",N,floor(Price),cpd(Price),BARS|LBL2,RED);
	}
}
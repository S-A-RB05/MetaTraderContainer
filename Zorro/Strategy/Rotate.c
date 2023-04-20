// Portfolio rotation script //////////////////////////
// (c) 2021 oP group Germany (RE) /////////////////////

/* Example for generating a rotation list from weights and margin costs:
print(TO_CSVHDR,"Asset,Weight%%,Margin,Lots,MarginCost");
for(i = 0; i < NumAssetsListed; i++)
  print(TO_CSV,"\n%s,%.0f,%.0f,%.0f,%.0f",
    Assets[i],
    Weights[i]*100,
    Weights[i]*Capital,
    roundto(Weights[i]*Capital/MarginCosts[i],1),
	 MarginCosts[i]);
*/

string Filename = "Data\\MyStrategy.csv"; // rotation list
string Fileformat = "ss,f,f,f";
string Outfile	= "Log\\Rotation.log";
//string TickerSymbol = "*";
string TickerSymbol = "IB:*-STK-SMART-USD!STOOQ:*";
#define NEWWEIGHT 1 // weight percent field number
#define NEWLOTS	3 // lots field number
#define LEVERAGE	1

#define NewLots	AssetInt[10]
#define OpenLots	AssetInt[11]

void folioTrade(int LS)
{
	for(used_assets) {
		asset(Asset);
		if((OpenLots > NewLots && LS < 0) // first short
			|| (OpenLots < NewLots && LS > 0)) // then long
			enterLong(NewLots-OpenLots);
	}
}

function main() 
{
	LookBack = 0;
   StartDate = NOW;
	set(LOGFILE);
	SaveMode = 0;
   
	//var TotalMargin = slider(1, 10000, 5000, 50000, "Capital", "Total invested margin");
	int i,Records = dataParse(1,Fileformat,Filename);
	if(!Records) return quit("CSV file not found");
	
	printf("\nSubscribing %i assets..",Records);
	brokerCommand(SET_PRICETYPE,8); // get prices fast
	string Box = zalloc(50000);
	strcpy(Box,"\n---------------------------------");
	int Changed = 0;
	var Invest = 0;
   for(i=0; i<Records; i++) {
		printf(".");
		string Ticker = dataStr(1,i,0);
		assetAdd(Ticker,TickerSymbol);
		if(!asset(Ticker)) continue;
		NewLots = dataVar(1,i,NEWLOTS);
		OpenLots = brokerCommand(GET_POSITION,Ticker);
		Margin = NewLots*priceClose()/LEVERAGE;
		Invest += Margin;
		if(OpenLots != NewLots) {
			Changed = 1;
			strcat(Box,strf("\n%s \t%d -> %d@%s ($%.2f)",
				Asset,OpenLots,NewLots,sftoa(priceClose(),2),Margin));
		} else if(OpenLots)
			strcat(Box,strf("\n%s \t%d@%s ($%.2f)",
				Asset,OpenLots,sftoa(priceClose(),2),Margin));
	}
	strcat(Box,"\n---------------------------------");
	strcat(Box,strf("\nTotal margin $%s at %s",
		sftoa(Invest,4),strdate(YMDHMS,NOW)));

	if(!Changed)
		printf("\nPortfolio unchanged");
	else {
		strcat(Box,"\nModify Portfolio?");
		if(msg(Box)) {
			strcat(Box," Y");
			setf(TradeMode,TR_GTC);
			folioTrade(-1); // Short
			folioTrade(1);  // Long
			file_append(Outfile,Box,0);
			exec("Editor",Outfile,0); 
		} else
			printf("\nNo positions entered");
	}
}
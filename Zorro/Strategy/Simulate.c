//script for simulating a strategy 
//by reading trades from a CSV file 

string Name = "Log\\testtrades.csv";  // name of the CSV file
//Name,Type,Asset,ID,Lots,Open,Close,Entry,Exit,Profit,Roll,ExitType
//Workshop4,Short,EUR/USD,51806,1,2012-02-01 07:00,2012-02-01 09:00,1.30372,1.30811,-79.96,0.00,Reverse
string Format = "s1,s2,sssss3,i8,i9,%Y-%m-%d %H:%M,sssss10,f15,f16,f17";	
#define STARTDATE 2012  // simulation start date
#define ENDDATE	2017  

// check all trades and enter the trades opened on the current date
void readTrade()
{
	int i,Records = dataFind(1,0);
	for(i=0; i<Records; i++) {
		
		TRADE T; memset(&T,0,sizeof(T)); 
		T.tEntryDate = dataVar(1,i,0);
		T.tExitDate = wdatef("%Y-%m-%d %H:%M",dataStr(1,i,10));
		T.flags = TR_OPEN|TR_SIMULATED;
		if(dataStr(1,i,2) == "Sho") T.flags |= TR_SHORT;
		strcpy(T.Skill,dataStr(1,i,3)); // Asset
		T.nID = dataInt(1,i,8);
		T.nLots = dataInt(1,i,9);
		T.fEntryPrice = dataVar(1,i,15);
		T.fExitPrice = dataVar(1,i,16);
		T.fResult = dataVar(1,i,17);

		if(T.tEntryDate <= wdate(0) && T.tEntryDate > wdate(1))
			enterTrade(&T);
	}

}
	
function run()
{
	StartDate = STARTDATE;
	EndDate = ENDDATE;
	LookBack = 0;
	BarPeriod = 60;
	set(PLOTNOW);
	if(Init) {
		dataParse(1,Format,Name);
		dataSort(1);
	}
	readTrade();
}
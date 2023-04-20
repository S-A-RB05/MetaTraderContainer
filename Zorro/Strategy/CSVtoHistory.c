////////////////////////////////////////////////
// Convert price history from .csv to .t6
// The Format string determines the CSV format (see examples)
////////////////////////////////////////////////

//#define SPLIT_YEARS	// split into separate years
//#define FIX_ZONE	-1 // add a time zone difference, f.i. for converting CST -> EST

/* T6 Target format:
	DATE	time;	
	float fHigh, fLow;	// f1,f2
	float fOpen, fClose;	// f3,f4	
	float fVal, fVol;		// f5,f6
*/

// HISTDATA line format: "20100103 170000;1.430100;1.430400;1.430100;1.430400;0"
//string Format = "+%Y%m%d %H%M%S;f3;f1;f2;f4";

// YAHOO line format "2015-05-29,43.45,43.59,42.81,42.94,10901500,42.94"
//string Format = "%Y-%m-%d,f3,f1,f2,f4,f6,f5"; // unadjusted

// TRADESTATION line format "06/30/2016,17:00:00,2086.50,2086.50,2086.50,2086.50,319,0"
//string Format = "+%m/%d/%Y,%H:%M:%S,f3,f1,f2,f4,f6,f5";

// STK line format "12/23/2016,2300.00,SPY, 225.63, 225.68, 225.72, 225.62,1148991"
//string Format = "+%m/%d/%Y,%H%M,,f3,f4,f1,f2,f6";

// CHRIS_ICE line format: Date,Open,High,Low,Settle,Change,Wave,Volume,...
// 2020-04-08,10.34,10.46,10.22,10.37,-0.01,10.32,54520.0,268936.0,4008.0,50.0,500.0
//string Format = "%Y-%m-%d,f3,f1,f2,f4,,,f6";

// MKTS Daily line format: Date,Open,High,Low,Close,Volume
// 02/28/2020,108.4,110,107.475,107.575,44239
string Format = "+%m/%d/%Y,f3,f1,f2,f4,f6";

function main()
{
	string InName = file_select("History","CSV file\0*.csv\0\0");
	if(!InName) return quit("No file"); 
	int Records = dataParse(1,Format,InName);
	printf("\n%d lines read",Records);
#ifdef FIX_ZONE
	int i;
	for(i=0; i<Records; i++)
		dataSet(1,i,0,dataVar(1,i,0)+FIX_ZONE/24.);
#endif
#ifndef SPLIT_YEARS
	string OutName = strx(InName,".csv",".t6");
	if(Records) dataSave(1,OutName);
	printf("\n%s",OutName);		
#else
	int i, Start = 0, Year, LastYear = 0;
	for(i=0; i<Records; i++) {
		Year = ymd(dataVar(1,i,0))/10000;
		if(!LastYear) LastYear = Year;
		if(i == Records-1) { // end of file
			LastYear = Year; Year = 0; i++;
		}
		if(Year != LastYear) {
			string OutName = strf("%s_%4i.t6",strxc(InName,'.',0),LastYear);
			printf("\n%s",OutName);		
			dataSave(1,OutName,Start,i-Start);
			Start = i; LastYear = Year;
		}
	}
#endif
}
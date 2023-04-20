//////////////////////////////////////////////////////////
// Example conversion for CSV price data to .t1 //////////
//////////////////////////////////////////////////////////

// Tickdata .csv to .t1 (ask prices only) ////////////////
//Date and Time,Bid Price,Bid Size,Ask Price,Ask Size,Market Flag,Quote Condition
//06/05/2014 17:00:00.021000000,1938.25,134,1938.50,137,E,

string InName = "History\\ES.csv";  // name of the CSV file
string OutName = "ES_%i.t1";

string Format = "+u30%m/%d/%Y %H:%M:%S,,,f";

void main()
{
	int Year;
	for(Year = 2012; Year <= 2019; Year++) {
		dataNew(1,0,0);
		int Records = dataParse(1,Format,InName,strf("/%i",Year));	// read BBO data
		printf("\n%i - %d rows read",Year,Records);
		if(Records) dataSave(1,strf(OutName,Year));		// store year dataset
	}
}

// T6,T8 -> CSV conversion

// Date,O,H,L,C,Spread,Volume
string FormatT6 = "%Y-%m-%dT%H:%M:%S,f3,f1,f2,f4,f5,f6";
// Date,Expiry,Strike,Unl,Ask,Bid,Vol,Val
string FormatT8 = "%Y-%m-%dT%H:%M:%S,i8,i7,f6,f5,f1,f2,f4,f3";

function main()
{
	LookBack = 0;
	GapDays = 3;
	Verbose = 3;
	StartDate = 2000;
	string InName = Define; // name from the command line
	if(!strrchr(InName,'.'))
		InName = file_select("History",
			"All (t1,t6,t8)\0*.t1;*.t6;*.t8\0Candles (t6)\0*.t6\0Ticks (t1)\0*.t1\0Contracts (t8)\0*.t8\0\0");
	if(!InName) return quit("No file"); 
	char* Dot = strrchr(InName,'.');
	if(!Dot) return quit("Invalid name");
	if(0 == strcmp(Dot+1,"t6")) {
		dataLoad(1,InName,7);
		dataSaveCSV(1,FormatT6,strx(InName,".t6","t6.csv"),0,0);
	} else if(0 == strcmp(Dot+1,"t8")) {
		dataLoad(1,InName,9);
		dataSaveCSV(1,FormatT8,strx(InName,".t8","t8.csv"),0,0);
	} else
		return quit("Invalid name");
	strcpy(Define,"");
}

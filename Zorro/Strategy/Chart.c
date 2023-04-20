function run()
{
	LookBack = 0;
	GapDays = 4; // weekend plus holiday
	Verbose = 3;
	resf(BarMode,BR_WEEKEND); // don't suppress weekend bars
	set(COMMONSTART,PLOTNOW);
	//set(LOGFILE);
	static int PlotVol = 0, PlotVal = 0;
	if(is(INITRUN)) {
		StartDate = 2000;
		string InName = Define; // name from the command line
		if(!strrchr(InName,'.'))
			InName = file_select("History",
				"All (t1,t6,t8)\0*.t1;*.t6;*.t8\0Candles (t6)\0*.t6\0Ticks (t1)\0*.t1\0Contracts (t8)\0*.t8\0\0");
		if(!InName) return quit("No file"); 
		char* Dot = strrchr(InName,'.');
		if(!Dot) { quit("Invalid name"); return; }
		printf(" %s",Dot+1);
		strcpy(History,Dot); // years and suffixes are treated as part of the asset name
		*Dot = 0;
		char* Name = strrchr(InName,'\\');
		if(Name) Name++;
		printf(" %s",Name);
		assetAdd(Name);
		if(!asset(Name)) { 
			print("Can't load %s",Name); quit(""); return; 
		}
		if(is(SPONSORED)) {
			PlotVol = marketVol() != 0.;
			PlotVal = marketVal() != 0.;
		}
	}
	priceC(); // check for outliers
	if(PlotVol)
		plot("MarketVol",marketVol(),BARS|NEW,BLUE);
	if(PlotVal)
		plot("MarketVal",marketVal(),LINE|NEW,RED);
	strcpy(Define,"");
}

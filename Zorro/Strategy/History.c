///////////////////////////////////////////////////////////////////////////////
// Display price or options history in panel
///////////////////////////////////////////////////////////////////////////////

int MaxRecords=1000000;
int Precision=5; 
int i,j,Records,OutliersFound=0;
var PrevDate = 0;
string InName;


void panelInit(int Fields,int Width)
{
	printf(" %s - %s",strdate(YMDHMS,dataVar(1,Records-1,0)),strdate(YMDHMS,dataVar(1,0,0)));
	if(Records > MaxRecords) {
		Records = MaxRecords;
		printf(", clipped to %i",Records);
	}
	panel(Records+1,Fields,GREY,Width);
	panelFix(1,0);
	print(TO_PANEL,InName);
}

void checkOverlap(var Date)
{
	if(PrevDate > 0 && PrevDate < Date)
		printf("\nOverlap at %s < %s",
			strdate("%y-%m-%d %H:%M",PrevDate),
			strdate("%y-%m-%d %H:%M",Date));
	PrevDate = Date;
}

/*void click(int Row,int Col)
{
	if(Col == 0) { // date field clicked?
		sound("Click.wav");	
	}
}*/

void main() 
{
	if(!is(SPONSORED)) {
		printf("Need Zorro S for panel!");
		return;
	}
	if(Live || Train) return;
// load history from command line or with file open dialog	
	InName = Define; // name from the command line
	if(!strrchr(InName,'.'))
		InName = file_select("History",
			"All (t8,t6,t2,t1)\0*.t8;*.t6;*.t2;*.t1\0Candles (t6)\0*.t6\0Orders (t2)\0*.t2\0Ticks (t1)\0*.t1\0Options (t8)\0*.t8\0\0");
	if(!InName) return;
	printf("%s",InName);
	if(strstr(InName,".t8")) {  // options 
		Records = dataLoad(1,InName,9);
		if(!Records) { printf("\nCan't open %s",InName); return; }
		printf("\n%i contracts",Records);
		panelInit(9,64);
		panelSet(0,0,"Date",0,1,1); //4);
		panelSet(0,1,"Ask",0,1,1);
		panelSet(0,2,"Bid",0,1,1);
		panelSet(0,3,"Val",0,1,1);
		panelSet(0,4,"Vol",0,1,1);
		panelSet(0,5,"Unl",0,1,1);
		panelSet(0,6,"Strike",0,1,1);
		panelSet(0,7,"Expiry",0,1,1);
		panelSet(0,8,"Type",0,1,1);
		
		CONTRACT* Contract = dataStr(1,0,0);
		int LastDay = 0;
		for(i=1; i<=Records; i++,Contract++)
		{
			string Format = ifelse(modf(Contract->time,0) < 0.0001,"%y-%m-%d","%y-%m-%d %H:%M");
			panelSet(i,0,strdate(Format,Contract->time),0,0,0); //1,4);
			if(i > 1 && LastDay != (int)Contract->time)
				panelSet(i,0,panelGet(i,0),GREY,0,0); //1,4); // mark day changes grey
			LastDay = (int)Contract->time;
			panelSet(i,1,sftoa(Contract->fAsk,-2),0,0,0);
			panelSet(i,2,sftoa(Contract->fBid,-2),0,0,0);
			panelSet(i,3,sftoa(Contract->fVal,-2),0,0,0);
			panelSet(i,4,sftoa(Contract->fVol,0),0,0,0);
			panelSet(i,5,sftoa(Contract->fUnl,-2),0,0,0);
			panelSet(i,6,sftoa(Contract->fStrike,-2),0,0,0);
			panelSet(i,7,sftoa(Contract->Expiry,0),0,0,0);
			string CType = strf("");
			if(Contract->Type&CALL) strcat(CType,"Call ");
			if(Contract->Type&PUT) strcat(CType,"Put ");
			if(Contract->Type&FUTURE) strcat(CType,"F");
			if(Contract->Type&EUROPEAN) strcat(CType,"E");
			panelSet(i,8,CType,0,0,0);
			if(Contract->time < 30000) {
				panelSet(i,0,panelGet(i,0),RED,0,0); // mark missing dates red
				if(once(1)) printf("\nError: Missing time stamp");
			}
			if(Contract->fUnl == 0. || ((Contract->Type&(CALL|PUT)) && Contract->fStrike == 0.)) {
				panelSet(i,0,panelGet(i,0),RED,0,0); // mark missing prices red
				if(once(1)) printf("\nError: Missing price at %s exp %i",
					strdate(YMD,Contract->time),Contract->Expiry);
			}
			if(!progress((100*i)/Records,0)) break;
		}
	} else if(strstr(InName,".t6")) {  // M1 history
		Records = dataLoad(1,InName,7);
		if(!Records) { printf("\nCan't open %s",InName); return; }
		printf("\n%i bars",Records);
		panelInit(7,90);
		panelSet(0,0,"Date",0,1,1);
		panelSet(0,1,"Open",0,1,1);
		panelSet(0,2,"High",0,1,1);
		panelSet(0,3,"Low",0,1,1);
		panelSet(0,4,"Close",0,1,1);
		panelSet(0,5,"Val",0,1,1);
		panelSet(0,6,"Vol",0,1,1);
		
		T6* Tick = dataStr(1,0,0);
		for(i=1; i<=Records; i++,Tick++)
		{
			var TimeStamp = roundto(Tick->time,1./1440);
			checkOverlap(TimeStamp);
			panelSet(i,0,strdate("%y-%m-%d %H:%M",TimeStamp),0,0,0);
			panelSet(i,1,sftoa(Tick->fOpen,Precision),0,0,0);
			panelSet(i,2,sftoa(Tick->fHigh,Precision),0,0,0);
			panelSet(i,3,sftoa(Tick->fLow,Precision),0,0,0);
			panelSet(i,4,sftoa(Tick->fClose,Precision),0,0,0);
			panelSet(i,5,sftoa(Tick->fVal,Precision),0,0,0);
			panelSet(i,6,sftoa(Tick->fVol,0),0,0,0);
			if(!progress((100*i)/Records,0)) break;
		}
	} else if(strstr(InName,".t2")) {  // market depth data 
		Records = dataLoad(1,InName,3);
		if(!Records) { printf("\nCan't open %s",InName); return; }
		printf("\n%i ticks",Records);
		MaxRecords *= 2;
		panelInit(3,130);
		panelSet(0,0,"Date",0,1,1);
		panelSet(0,1,"Ask/Bid",0,1,1);
		panelSet(0,2,"Volume",0,1,1);
		
		T2* Order = dataStr(1,0,0);
		for(i=1; i<=Records; i++,Order++)
		{
			panelSet(i,0,strdate("%y-%m-%d %H:%M:%S.",Order->time),0,0,0);
			panelSet(i,1,sftoa(Order->fVal,5),0,0,0);
			panelSet(i,2,sftoa(Order->fVol,2),0,0,0);
			if(!progress((100*i)/Records,0)) break;
		}
		Order--;
		printf("\n%.5f, %.5f",(var)(Order)->fVal,(var)Order->fVol);
	} else if(strstr(InName,".t1")) {  // tick data
		Records = dataLoad(1,InName,2);
		if(!Records) { printf("\nCan't open %s",InName); return; }
		printf("\n%i ticks",Records);
		MaxRecords *= 2;
		panelInit(3,110);
		panelSet(0,0,"Date",0,1,1);
		panelSet(0,1,"Ask/Bid",0,1,1);
		panelSet(0,2,"Spread",0,1,1);
		
		T1* Tick = dataStr(1,0,0);
		var LastBid = 0, LastVal = 0;
		OutliersFound = 0;
		for(i=1; i<=Records; i++,Tick++)
		{
			var Val = Tick->fVal;
			var TimeStamp = roundto(Tick->time,1./(60*1440));
			panelSet(i,0,strdate("%y-%m-%d %H:%M:%S",TimeStamp),0,0,0);
			panelSet(i,1,sftoa(Val,6),0,0,0);
			if(i > 1 && abs(Val) > 2*LastVal || abs(Val) < 0.5*LastVal) {
				panelSet(i,1,panelGet(i,1),RED,0,0);
				OutliersFound++;
			}
			LastVal = abs(Val);
			if(Tick->fVal < 0)
				LastBid = Val;
			else if(LastBid < 0) {
				panelSet(i,2,sftoa(Val+LastBid,2),0,0,0);
				LastBid = 0; // no spread from too old bids
			}
			if(!progress((100*i)/Records,0)) break;
		}
	}
	if(OutliersFound)
		printf("\n%i outliers detected!",OutliersFound);
}

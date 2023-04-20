// WFO Profile Framework ///////////////////////
// 1. Put script name in #include line below
// 2. Set CYCLES to max number of wfo cycles 
// 3. Outcomment NumWFOCycles/WFOPeriod in the script 
// 4. Insert setStartDate(..) if you want a fixed test start
// 5. Start in [Train] mode 
//////////////////////////////////////////////////
#include <default.c>
#define	run strategy
#include	"trend.c" // <= your script
#undef	run
#define CYCLES	13	// max WFO cycles

// adjust StartDate to a fixed WFO test start
// needs EndDate, DataSplit to be set
void setStartDate(int TestDate) // YYYYMMDD
{
	if(!Init) return;
	var Split = ifelse(DataSplit > 0,DataSplit/100.,0.85);
	var TrainDays; 
	if(WFOPeriod > 0)
		TrainDays = Split*WFOPeriod*BarPeriod/1440;
	else if(NumWFOCycles > 1)
		TrainDays = Split/(1.-Split)*(dmy(EndDate)-dmy(TestDate))/(NumWFOCycles-1);
	else return;
	StartDate = ymd(dmy(TestDate)-TrainDays);
}


function run()
{
	set(TESTNOW);
	NumTotalCycles = CYCLES-1;
	NumWFOCycles = TotalCycle+1; 
	strategy();
	if(NumWFOCycles != TotalCycle+1)
		quit("Outcomment NumWFOCycles!");
}

function evaluate()
{
	var Perf = ifelse(LossTotal > 0,WinTotal/LossTotal,10);
	if(Perf > 1)
		plotBar("WFO+",NumWFOCycles,NumWFOCycles,Perf,BARS,BLACK);	
	else
		plotBar("WFO-",NumWFOCycles,NumWFOCycles,Perf,BARS,RED);	
}

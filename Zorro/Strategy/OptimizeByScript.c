// user-provided optimization algo

int parameters()
{
// set parameters for brute force optimization
   int i;
   for(i=0; i<NumParameters; i++) {
      if(StepCycle == 1 || Parameters[i].Value >= Parameters[i].Max)
         Parameters[i].Value = Parameters[i].Min;
		else {
         Parameters[i].Value += Parameters[i].Step;
         break;
      } 
   }
   for(i=0; i<NumParameters; i++)
		if(Parameters[i].Value < Parameters[i].Max)
			return 1;
   return 0;   // optimization complete, last cycle
}

 
var objective()
{
   var Result = ifelse(LossTotal == 0,9.99,WinTotal/LossTotal);
// store best parameters
	int i;
   if(BestResult < Result) {
      BestResult = Result;
      for(i=0; i<NumParameters; i++)
         Parameters[i].Best = Parameters[i].Value;
   }
   return Result;
}

function run()
{
	//require(-2.32);
   set(LOGFILE,PARAMETERS);
	//TrainMode = BRUTE|PEAK;
	//LogNumber = 2;
   LookBack = 500;
   Slippage=0;
   StartDate = 2010;
   EndDate = 2015;
   BarPeriod = 240;

   vars Price = series(price());
   var P1 = optimize(300,100,700,100);
   vars Trend = series(LowPass(Price,P1));
   var P2 = optimize(3,1,5,1);
   Stop = P2 * 3 * ATR(100);
   
   if(valley(Trend))
      enterLong();
   else if(peak(Trend))
      enterShort();
}
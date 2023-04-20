// Price Distribution Chart /////////////////////

function run()
{
	StartDate = 20110920; // First day of the CHF price cap
	EndDate = 20150114; // Last day of the CHF price cap
	BarPeriod = 60;

	asset("EUR/CHF");
	var PriceCHF = price();
	asset("EUR/USD");
	var PriceUSD = price();
	PlotLabels = 4;
// plot distributions separately (otherwise only the selected asset is plotted)	
	plotHistogram("EUR/CHF",PriceCHF,0.004,1,RED+TRANSP);	
	plotHistogram("EUR/USD",PriceUSD,0.004,1,BLACK+TRANSP);	
}
// performance report analysis example /////////////////////

#define TEST

// does a string begin with a number?
int strnum(string S)
{
	if(*S == '+' || *S == '-' || *S == '.' || (*S >= '0' && *S <= '9'))
		return 1;
	else return 0;
}

// return a value from the performance report
var performance(string Content,string Name,int Pos)
{
	string S = strstr(Content,Name);
	if(!S) return 0;
	S += strlen(Name); 
	if(!Pos) Pos++;
	for(; Pos>0; Pos--) {
		while(*S && *S != 0x0d && *S != 0x0a && !strnum(S)) S++;
		if(Pos == 1 && strnum(S)) return atof(S);
		while(strnum(S)) S++; // next number
	}
	return 0; // not found
}

#ifdef TEST
void main()
{
	string Name = file_select("Log","Reports (*.txt)\0*.txt\0\0");
	if(!Name) return;
	string Content = file_content(Name);
	if(!Content) return;
// store montecarlo results in a csv file	
	Name = "Log\\Evaluation.csv";
	file_write(Name,"Level,AR,DD,Capital\n",0);
	for(Confidence = 10; Confidence <= 100; Confidence += 10) {
		string Level = strf("\n%3i%%",Confidence);
		var AR = performance(Content,Level,1);
		var DD = performance(Content,Level,2);
		var Cap = performance(Content,Level,3);
		file_append(Name,strf("%i,%.0f,%.0f,%.0f\n",Confidence,AR,DD,Cap));
	}
// print 50% results
	string Level = "\n 50%";
	var AR = performance(Content,Level,1);
	var DD = performance(Content,Level,2);
	var Cap = performance(Content,Level,3);
	printf("\nLevel 50: AR %.0f%% DD %.0f Cap %.0f$",AR,DD,Cap);
}
#endif
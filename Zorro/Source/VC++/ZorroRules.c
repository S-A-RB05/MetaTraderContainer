int eq0(double diff) 
{ 
	if(diff <= 0.00001 && diff >= -0.00001)
		return 1;
	return 0;
}

#define FUZZYRANGE	0.0001

int eqF(double diff) 
{ 
	if(diff <= FUZZYRANGE && diff >= -FUZZYRANGE)
		return 1;
	return 0;
}

double belowF(double a,double b) 
{
	double halfrange = 0.5*FUZZYRANGE;
	double diff = b - a;
	if(diff > halfrange) return 1.;
	if(diff < -halfrange) return 0.;
	return (diff+halfrange)/FUZZYRANGE;
}


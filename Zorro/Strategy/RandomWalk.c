// Rise/Fall sequence distribution ///////////////////

function run()
{
	vars Price = series(price());
	int num = NumRiseFall(Price,20);
	int x = 3*num; 
	if(num < 0) x += 3; // eliminate the 0 gap
	plotBar("Price",x,num,1,SUM+BARS,RED);	
	
	vars Random = series(0);
	Random[0] = Random[1] + random();
	num = NumRiseFall(Random,20);
	x = 3*num+1;
	if(num < 0) x += 3;
	plotBar("Random",x,NIL,1,SUM+BARS,BLUE);	
}
// Date calculator ///////////////////////////////////

void main()
{
	slider(1,year(NOW),2000,year(NOW),"Year","");
	slider(2,1,1,12,"Month","");
	slider(3,1,1,31,"Day",""); 
	printf("Win,Unix,Day, [Stop] to end");
	while(wait(100)) {
		var WinDate = dmy(slider(1)*10000+slider(2)*100+slider(3));
		print(TO_INFO,"%.0f, %i, %s",WinDate,utm(WinDate),strdate("%a",WinDate));
	}
	printf("\n");
}
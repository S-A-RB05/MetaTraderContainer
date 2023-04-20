// Fibonacci example ///////////////////////

int fibo(int n)
{
  if(n <= 1) return 1;
  return fibo(n-1) + fibo(n-2);
}

function  main()
{
  int n;
  for(n = 0; n < 10; n++)
    printf("%i ",fibo(n));
}

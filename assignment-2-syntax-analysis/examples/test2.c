int foo(int n, int m){
	return m+n;
}

int too(){
	return 0;
}

int bar(int k){
	foo(
  3,
       foo (
 bar( 9   ) ,
   foo(
                    5  ,  too (   )    )
               ));      
	return 0;
}

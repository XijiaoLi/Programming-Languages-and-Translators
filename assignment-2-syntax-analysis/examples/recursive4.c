int afunc(int x, int y){
  return x+y;
}

int afunc(int x){
  if (x==0){
    return x;
  } else {
    return afunc(x,x);
  }
}

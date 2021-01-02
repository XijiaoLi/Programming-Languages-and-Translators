typedef int (*FuncPtr)(int, int);

int addNum(int a, int b) {
    return a + b;
}

int mulNum(int a, int b) {
  return a * b;
}

FuncPtr getFunc(int op1, int op2) {
    if (op1 == op2){
        return &addNum;
    }
    return &mulNum;
}


FuncPtr getFunc(int op) {
    if (op == 0){
        return getFunc(0, 0);
    }
    return getFunc(0, 1);
}

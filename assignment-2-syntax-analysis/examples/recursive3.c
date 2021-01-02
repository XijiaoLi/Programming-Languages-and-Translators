typedef int (*FuncPtr)(int, int);

int addNum(int a, int b) {
    return a + b;
}

int mulNum(int a, int b) {
  return a * b;
}

FuncPtr getFunc(int op) {
    if (op == 0){
        return getFunc(true);
    }
    return getFunc(false);
}

FuncPtr getFunc(bool op) {
    if (op == true){
        return &addNum;
    }
    return &mulNum;
}

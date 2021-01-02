typedef int (*FuncPtr)(int, int);

int addNum(int a) {
    return a + a;
}

int mulNum(int a) {
  return a * a;
}

FuncPtr getFunc2(int op) {
    if (op == 1){
        return &addNum;
    }
    return &mulNum;
}

FuncPtr getFunc(int op) {
    if (op == 1){
        return &getFunc2;
    }
    return getFunc(op-1)(op);
}

int main() {
    int ret = getFunc(2)(5);
    return 0;
}

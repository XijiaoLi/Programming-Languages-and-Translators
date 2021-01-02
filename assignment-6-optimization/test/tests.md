## Rubric:
There are 2 test cases. Suppose the solutions *as a whole* consist of `D` dead functions. Suppose one student outputs `d` correct dead functions and `d'` wrong dead functions. Duplicates in the output, whether correct or incorrect, count only once.

* Correct dead function worth: `X1 = 50 / E`
* Incorrect dead function Deduction: `Y1 = 1 * X1`

Same for removed dead functions (`X2`, `Y2`) and dead instructions (`X3`, `Y3`).

Total point:
```
min(0, d*X1 - d’*Y1) + min(...) + min(...)
```

## Test Cases:

```
int func1() {
    return 0;
}

int main() {
    int a = 1, b = 2;
    int c = 0;

    if (c == 0) {
        c = c + 1;
    }
    a = a * 2;
    b = b + c;
    
    if (c == 0) {
        return c + 1;
    } else {
        return c + 2;
    }
    return a + b;
}
```

```
#include <stdio.h>

int func2(int n, int m) {	 
    return n + m;
}
int func3(int m, int n) {
    int res = 0, t = m + n, x = m * n;
    while(res != x) {
        res = res + m;
    }
    return res;
}
int func4(int n) {
    if (n == 0) return 1;
    else {
        return func3(n, func4(func2(n, -1)));
    }
}

int main() {
    int n = 9, m = 8;
    printf("%d\n", func3(m, n));
    return 0;
}
```

```
#include <stdio.h>

int func5(int n);
int func6(int n);
int func7(int n);
int func8(int n);

int func5(int n) {
    return func6(n);
    return func7(n) + func8(n + n);
}

int func6(int n) {
    return func8(n);
}

int func7(int n) {
    return func5(n);
}

int func8(int n) {
    return func6(func8(func5(n)));
}

int main() {
    int n = 9, m = 8;
    printf("%d\n", func5(n));
    return 0;
}
```
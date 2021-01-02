int gcd_recursive(int m2, int n2) {
    int r = m2 % n2, k4 = 0;
    if(r != 0) {
	int c = 9;
        return gcd_recursive(n2,r);
    }
    else {
        int k4 = n2;
	int c = k4 + 0;
        return c;
    }
}

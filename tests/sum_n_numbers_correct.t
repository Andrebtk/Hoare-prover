sum = 0;
i = 0;
while (i <= n) invariant (sum == (i * (i - 1)) / 2) variant (n - i + 1) {
    sum = sum + i;
    i = i + 1;
}


PRECONDITION: n >= 0
POSTCONDITION: sum == (n * (n + 1)) / 2
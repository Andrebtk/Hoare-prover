sum = 0;
i = 0;

while (i != n) INVARIANT (sum == (i * (i + 1)) / 2) VARIANT (n - i) {
    i = i + 1;
    sum = sum + i;
}

PRECONDITION: n >= 0
POSTCONDITION: sum == (n * (n + 1) / 2) + 1
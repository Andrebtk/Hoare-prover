total = 0;
i = 1;
while (i <= n) INVARIANT (total == (i - 1) * i * (i + 1) / 6) VARIANT (n - i + 1) {
    j = 1;
    while (j <= i) INVARIANT (total == ((i - 1) * i * (i + 1) / 6) + (j - 1) * i) VARIANT (i - j + 1) {
        total = total + i;
        j = j + 1;
    }
    i = i + 1;
}

PRECONDITION: n >= 0
POSTCONDITION: total == n * (n + 1) * (n + 2) / 6 + 5
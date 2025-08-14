sumodd = 0;
i = 1;

while (i <= n) INVARIANT (
    sumodd == (i / 2) * (i / 2)
    and i >= 1 and i <= n + 1
) VARIANT (n - i + 1) {
    if (i % 2 != 0) {
        sumodd = sumodd + i;
    } else {}

    i = i + 1;
}

PRECONDITION: n >= 0
POSTCONDITION: sumodd == ((n + 1) / 2) * ((n + 1) / 2) + 1

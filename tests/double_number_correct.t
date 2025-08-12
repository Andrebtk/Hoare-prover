y = 0;
i = 0;
while (i < x) invariant (y == 2 * i) variant (x - i) {
    y = y + 2;
    i = i + 1;
}

PRECONDITION: x > 0
POSTCONDITION: y == 2 * x
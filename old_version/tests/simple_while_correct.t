x = 0;
while (x < 10) invariant (x >= 0 and x <= 10) variant (10 - x) {
    x = x + 1;
}

PRECONDITION: true
POSTCONDITION: x == 3
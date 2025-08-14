if (x > 5) {
    y = 10;
} else {
    y = 0;
}

PRECONDITION: true
POSTCONDITION: (x > 5 AND y == 10) OR (NOT (x > 5) AND y == 0)

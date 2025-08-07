x = 3;
y = 0;
if (x > 0) {
    if (x < 5) {
        y = 10;
    } else {
        y = 20;
    }
} else {
    y = 30;
}

PRECONDITION: true
POSTCONDITION: y == 10

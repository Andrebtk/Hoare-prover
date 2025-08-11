y = 1;
z = 0;
x = 5;

while(z != x) invariant (y == fact(z)) variant (x - z) {
    z = z + 1;
    y = y * z;
}

PRECONDITION: true
POSTCONDITION: y == fact(x)

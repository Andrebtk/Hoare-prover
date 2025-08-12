y = 1;
z = 0;
while (z != x) invariant (y == fact(z)) variant (x - z) {
    z = z + 1;
    y = y + z; 
}

PRECONDITION: x >= 0
POSTCONDITION: y == fact(x)
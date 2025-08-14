q = 0;
r = x;
while (r >= y) invariant (x == q * y + r AND r >= 0) variant (r)
{
    r = r - y;
    q = q + 1;
}


PRECONDITION: x >= 0 AND y > 0
POSTCONDITION: x == q * y + r AND 0 <= r AND r < y
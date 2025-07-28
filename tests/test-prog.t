y = 1 + 3;
z = 0;

if (y == 1) {
	z = 1;
	y = y + 1;

	while (y < 5) {
		z = y * z;
		y = y + 1;
	}
}


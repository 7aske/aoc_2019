#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	FILE* fptr = fopen("day01-2/input.txt", "r");
	char buf[16];
	unsigned long total = 0;
	unsigned long x = 0, y = 0;

	if (fptr == NULL) {
		fprintf(stderr, "Failed to open file.\n");
		return 1;
	}
	while (fgets(buf, 16, fptr) != NULL) {
		x = strtol(buf, NULL, 10);
		y = (x / 3);
		if (y <= 2) {
			continue;
		}
		y -= 2;
		total += y;
		while (y > 0) {
			y = (y / 3);
			if (y <= 2) {
				break;
			}
			y -= 2;
			total += y;
		}
	}
	fclose(fptr);
	printf("%lu\n", total);
	return 0;
}

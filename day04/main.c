//
// Created by nik on 12/13/19.
//

#include <stdio.h>
#include <assert.h>

int check_digits(int num) {
	int n = num;
	int x, i;
	int nums[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	int last = 10;
	while (n != 0) {
		x = n % 10;
		n /= 10;
		if (x > last) {
			return 0;
		}
		if (x == last) {
			nums[x]++;
		}
		last = x;
	}

	for (i = 0; i < 10; ++i) {
		if (nums[i] == 2) {
			return 1;
		}
	}
	return 0;
}

int main(int argc, char** argv) {
	int pmax = 679128, pmin = 206938, i, count = 0;
	assert(check_digits(223450) == 0);
	assert(check_digits(123789) == 0);
	assert(check_digits(112233) == 1);
	assert(check_digits(123444) == 0);
	assert(check_digits(111122) == 1);
	for (i = pmin; i <= pmax; ++i) {
		if (check_digits(i)) {
			printf("%d\n", i);
			count++;
		}
	}
	printf("%d\n", count);
}
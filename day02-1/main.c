#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

enum opcode {
	HLT = 99,
	ADD = 1,
	MUL = 2,
};

int running = 1;


void run(unsigned int* bp, unsigned int** gpc) {
	unsigned int* pc = *gpc;
	enum opcode opcode = *pc;
	unsigned int reg1, reg2, dest;
	// printf("%d %d %d %d\n", opcode, *(pc + 1), *(pc + 2), *(pc + 3));
	switch (opcode) {
		case HLT:
			running = 0;
			break;
		case ADD:
			reg1 = *(pc + 1);
			reg1 = *(bp + reg1);
			reg2 = *(pc + 2);
			reg2 = *(bp + reg2);
			dest = *(pc + 3);
			*(bp + dest) = reg1 + reg2;
			*gpc += 4;
			break;
		case MUL:
			reg1 = *(pc + 1);
			reg1 = *(bp + reg1);
			reg2 = *(pc + 2);
			reg2 = *(bp + reg2);
			dest = *(pc + 3);
			*(bp + dest) = reg1 * reg2;
			*gpc += 4;
			break;
		default:
			running = 0;
	}
}

int main() {
	unsigned int test_program[] = {
			ADD, 9, 10, 3,
			MUL, 3, 11, 0,
			HLT,
			30, 40, 50
	};
	unsigned int* bp = test_program;
	unsigned int* pc = test_program;

	while (running) {
		run(bp, &pc);
	}
	running = 1;
	assert(test_program[0] == 3500);

	FILE* fptr = fopen("day02-1/input.txt", "r");
	if (fptr == NULL) {
		fprintf(stderr, "Failed to open file.\n");
		return 1;
	}
	unsigned char buf[3];
	int c;
	unsigned int x;
	unsigned char curr = 0;
	unsigned int ctotal = 32;
	unsigned int* program = calloc(ctotal, sizeof(unsigned int));
	unsigned int total = 0;
	do {
		c = (char) fgetc(fptr);
		if (c == ',' || c == EOF) {

			buf[curr] = '\0';
			curr = 0;
			x = (unsigned int) strtol(buf, NULL, 10);
			memcpy(program + total, &x, sizeof(unsigned int));
			total++;
			if (total == ctotal) {
				ctotal += 32;
				program = reallocarray(program, ctotal, sizeof(unsigned int));
			}
			if (c == EOF) {
				break;
			}
		} else {
			buf[curr++] = c;
		}

	} while (1);

	pc = program;
	bp = program;

	program[1] = 12;
	program[2] = 2;

	while (running) {
		run(bp, &pc);
	}

	printf("%d\n", program[0]);
	fclose(fptr);
	return 0;
}

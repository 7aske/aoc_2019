#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum opcode {
	HLT = 99,
	ADD = 1,
	MUL = 2,
};

int running = 1;

struct program {
	uint* mem;
	uint size;
};

void run(uint* bp, uint** gpc) {
	uint* pc = *gpc;
	enum opcode opcode = *pc;
	uint reg1, reg2, dest;
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

struct program* load_program() {
	FILE* fptr = fopen("day02-2/input.txt", "r");
	if (fptr == NULL) {
		fprintf(stderr, "Failed to open file.\n");
		return NULL;
	}
	unsigned char buf[3];
	unsigned char curr = 0;
	int c;
	uint x;
	uint ctotal = 32;
	uint total = 0;
	struct program* out = malloc(sizeof(struct program));
	uint* program = calloc(ctotal, sizeof(uint));
	do {
		c = (char) fgetc(fptr);
		if (c == ',' || c == EOF) {

			buf[curr] = '\0';
			curr = 0;
			x = (uint) strtol(buf, NULL, 10);
			memcpy(program + total, &x, sizeof(uint));
			total++;
			if (total == ctotal) {
				ctotal += 32;
				program = reallocarray(program, ctotal, sizeof(uint));
			}
			if (c == EOF) {
				break;
			}
		} else {
			buf[curr++] = c;
		}

	} while (1);
	fclose(fptr);
	out->mem = program;
	out->size = total;
	return out;
}

int main() {
	uint* bp = NULL;
	uint* pc = NULL;

	struct program* prog = load_program();
	uint* active = NULL;
	uint target = 19690720;

	uint noun;
	uint verb;
	for (noun = 0; noun <= 99 && running; ++noun) {
		for (verb = 0; verb <= 99 && running; ++verb) {
			if (active != NULL) {
				free(active);
			}
			active = calloc(prog->size, sizeof(uint));
			memcpy(active, prog->mem, prog->size * sizeof(uint));
			active[1] = noun;
			active[2] = verb;

			pc = active;
			bp = active;
			while (running) {
				run(pc, &bp);
			}

			if (active[0] == target){
				printf("%u %u %d\n", active[0], noun, verb);
			} else {
				running = 1;
			}
		}
	}
	return 0;
}
